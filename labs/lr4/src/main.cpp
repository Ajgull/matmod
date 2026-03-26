#include <omp.h>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

namespace py = pybind11;

struct Statistics {
    double mean_x, mean_y;
    double var_x, var_y;
    double var_R, mean_R;
    std::vector<double> x_coords, y_coords;
    std::vector<std::vector<double>> trajectories_x;
    std::vector<std::vector<double>> trajectories_y;
};

class RandomWalksVisualizer {
   private:
    const double PI = M_PI;

   public:
    double invert_func(double r) { return pow(tan(PI * r / 2.0), 1.0 / 3.0); }

    struct Trajectory {
        std::vector<double> x_coords;
        std::vector<double> y_coords;
    };

    Trajectory simulate_with_trajectory(int N, std::mt19937& rng) {
        Trajectory traj;
        traj.x_coords.reserve(N + 1);
        traj.y_coords.reserve(N + 1);

        double x = 0.0, y = 0.0;
        traj.x_coords.push_back(x);
        traj.y_coords.push_back(y);

        std::uniform_real_distribution<> dis(0.0, 1.0);
        std::uniform_int_distribution<> dir_dis(0, 3);

        for (int step = 0; step < N; step++) {
            double r = dis(rng);
            double length = invert_func(r);
            int direction = dir_dis(rng);

            switch (direction) {
                case 0:
                    y += length;
                    break;
                case 1:
                    y -= length;
                    break;
                case 2:
                    x -= length;
                    break;
                case 3:
                    x += length;
                    break;
            }

            traj.x_coords.push_back(x);
            traj.y_coords.push_back(y);
        }

        return traj;
    }

    std::vector<Trajectory> simulate_parallel_with_trajectories(int M, int N, int num_threads = 4) {
        if (num_threads > 0) {
            omp_set_num_threads(num_threads);
        }

        std::vector<Trajectory> results(M);

#pragma omp parallel for
        for (int i = 0; i < M; i++) {
            std::mt19937 thread_rng(std::random_device{}() + omp_get_thread_num() * 12345);
            results[i] = simulate_with_trajectory(N, thread_rng);
        }

        return results;
    }

    Statistics compute_statistics_from_trajectories(const std::vector<Trajectory>& trajectories) {
        Statistics stats;
        int M = trajectories.size();

        stats.x_coords.reserve(M);
        stats.y_coords.reserve(M);
        stats.trajectories_x.reserve(M);
        stats.trajectories_y.reserve(M);

        double sum_x = 0.0, sum_y = 0.0;
        double sum_x2 = 0.0, sum_y2 = 0.0;

        for (const auto& traj : trajectories) {
            double x_final = traj.x_coords.back();
            double y_final = traj.y_coords.back();

            stats.x_coords.push_back(x_final);
            stats.y_coords.push_back(y_final);

            stats.trajectories_x.push_back(traj.x_coords);
            stats.trajectories_y.push_back(traj.y_coords);

            sum_x += x_final;
            sum_y += y_final;
            sum_x2 += x_final * x_final;
            sum_y2 += y_final * y_final;
        }

        stats.mean_x = sum_x / M;
        stats.mean_y = sum_y / M;
        stats.var_x = sum_x2 / M - stats.mean_x * stats.mean_x;
        stats.var_y = sum_y2 / M - stats.mean_y * stats.mean_y;
        stats.var_R = stats.var_x + stats.var_y;
        stats.mean_R = sqrt(stats.mean_x * stats.mean_x + stats.mean_y * stats.mean_y);

        return stats;
    }

    void send_to_python_and_plot(const std::vector<Trajectory>& trajectories, int M, int N,
                                 double elapsed_time) {
        Statistics stats = compute_statistics_from_trajectories(trajectories);

        py::array_t<double> x_coords(stats.x_coords.size());
        py::array_t<double> y_coords(stats.y_coords.size());

        auto x_buf = x_coords.request();
        auto y_buf = y_coords.request();

        double* x_ptr = static_cast<double*>(x_buf.ptr);
        double* y_ptr = static_cast<double*>(y_buf.ptr);

        for (size_t i = 0; i < stats.x_coords.size(); i++) {
            x_ptr[i] = stats.x_coords[i];
            y_ptr[i] = stats.y_coords[i];
        }

        py::list traj_x_list, traj_y_list;

        int num_trajectories_to_save = std::min(10, (int)trajectories.size());
        for (int i = 0; i < num_trajectories_to_save; i++) {
            py::list traj_x, traj_y;
            for (size_t j = 0; j < trajectories[i].x_coords.size(); j++) {
                traj_x.append(trajectories[i].x_coords[j]);
                traj_y.append(trajectories[i].y_coords[j]);
            }
            traj_x_list.append(traj_x);
            traj_y_list.append(traj_y);
        }

        py::dict data;
        data["x_coords"] = x_coords;
        data["y_coords"] = y_coords;
        data["trajectories_x"] = traj_x_list;
        data["trajectories_y"] = traj_y_list;
        data["mean_x"] = stats.mean_x;
        data["mean_y"] = stats.mean_y;
        data["var_x"] = stats.var_x;
        data["var_y"] = stats.var_y;
        data["var_R"] = stats.var_R;
        data["mean_R"] = stats.mean_R;
        data["M"] = M;
        data["N"] = N;
        data["time"] = elapsed_time;

        try {
            py::module_ plotter = py::module_::import("main");
            plotter.attr("plot_simulation_results")(data);
        } catch (const py::error_already_set& e) {
            std::cerr << "Python error: " << e.what() << std::endl;
        }
    }

    void run_with_visualization(const std::vector<int>& M_values, const std::vector<int>& N_values,
                                int num_threads = 4) {
        try {
            py::module_::import("matplotlib");
            std::cout << "Matplotlib imported successfully\n";
        } catch (const py::error_already_set& e) {
            std::cerr << "Failed to import matplotlib: " << e.what() << std::endl;
            return;
        }

        for (int M : M_values) {
            for (int N : N_values) {
                std::cout << "M = " << M << ", N = " << N << std::flush;

                auto start = std::chrono::high_resolution_clock::now();

                auto trajectories = simulate_parallel_with_trajectories(M, N, num_threads);

                auto end = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(end - start).count();

                std::cout << ", done in " << elapsed << "s" << std::endl;

                send_to_python_and_plot(trajectories, M, N, elapsed);
            }
        }

        try {
            py::module_ plotter = py::module_::import("main");
            plotter.attr("plot_summary_plots")();
        } catch (const py::error_already_set& e) {
            std::cerr << "python error: " << e.what() << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    py::scoped_interpreter guard{};

    try {
        py::module_ sys = py::module_::import("sys");
        py::list path = sys.attr("path");

        path.attr("insert")(0, ".");

        std::string src_path = "/home/aigul/Documents/vs_code_projects/matmod/labs/lr4/src";
        path.attr("insert")(0, src_path);

    } catch (const py::error_already_set& e) {
        std::cerr << "failed to configure python path: " << e.what() << std::endl;
    }

    RandomWalksVisualizer simulator;

    std::vector<int> M_values = {100, 1000, 10000};
    std::vector<int> N_values;
    for (int N = 500; N <= 5000; N += 500) {
        N_values.push_back(N);
    }

    simulator.run_with_visualization(M_values, N_values, 8);

    std::cin.get();

    return 0;
}