#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class FPUChain {
   private:
    int num_chain;
    double alpha, beta, mass, dt;
    const double xi = 0.1931833275037836;
    long long total_time;

    vector<double> q;  // displacement
    vector<double> v;  // velocity
    vector<double> a;  // acceleration

   public:
    FPUChain(int n, double alpha, double beta, double mass, double time_step, long long total_time)
        : num_chain(n),
          alpha(alpha),
          beta(beta),
          mass(mass),
          dt(time_step),
          total_time(total_time) {
        q.resize(num_chain, 0.0);
        v.resize(num_chain, 0.0);
        a.resize(num_chain, 0.0);
    }

    double fpu(double r) {
        return 0.5 * r * r + (alpha / 3.0) * r * r * r + (beta / 4.0) * r * r * r * r;
    }

    double fpu_deriv(double r) { return r + alpha * r * r + beta * r * r * r; }

    int prev_index(int i) const { return (i - 1 + num_chain) % num_chain; }

    int next_index(int i) const { return (i + 1) % num_chain; }

    double comput_force(int i) {
        int prev = prev_index(i);
        int next = next_index(i);

        double r_right = q[next] - q[i];
        double r_left = q[i] - q[prev];

        double force_right = fpu_deriv(r_right);
        double force_left = fpu_deriv(r_left);

        // double force = force_right - force_left;
        double force = q[next] - 2 * q[i] + q[prev] +
                       alpha * (r_right * r_right - r_left * r_left) +
                       beta * (r_right * r_right * r_right - r_left * r_left * r_left);
        return force;
    }

    void init_soliton_displacement(double q0) {
        fill(q.begin(), q.end(), 0.0);
        fill(v.begin(), v.end(), 0.0);

        int center = num_chain / 2;
        if (center + 1 < num_chain) {
            q[center] = -q0;
            q[center + 1] = q0;
        }

        for (int i = 0; i < num_chain; i++) {
            a[i] = comput_force(i) / mass;
        }
    }

    void init_soliton_impulse(double v0) {
        fill(q.begin(), q.end(), 0.0);
        fill(v.begin(), v.end(), 0.0);

        int center = num_chain / 2;
        if (center + 1 < num_chain) {
            v[center] = v0;
            v[center + 1] = -v0;
        }

        for (int i = 0; i < num_chain; i++) {
            a[i] = comput_force(i) / mass;
        }
    }

    double velocity_verlet(long long num_steps, bool save_to_file = false,
                           const string& filename = "velocity_verlet.csv") {
        ofstream f_out;
        if (save_to_file) {
            f_out.open(filename);
            f_out << "step,time";
            for (int i = 0; i < num_chain; i++) {
                f_out << ",q" << i << ",v" << i;
            }
            f_out << "\n";

            f_out << "0,0.0";
            for (int i = 0; i < num_chain; i++) {
                f_out << "," << scientific << setprecision(12) << q[i] << "," << v[i];
            }
            f_out << endl;
        }

        for (long long step = 1; step <= num_steps; ++step) {
            for (int i = 0; i < num_chain; i++) {
                q[i] = q[i] + v[i] * dt + 0.5 * a[i] * dt * dt;
                v[i] = v[i] + 0.5 * a[i] * dt;
            }

            for (int i = 0; i < num_chain; i++) {
                a[i] = comput_force(i) / mass;
            }

            for (int i = 0; i < num_chain; i++) {
                v[i] = v[i] + 0.5 * a[i] * dt;
            }

            if (save_to_file && step % 100 == 0) {
                double time = step * dt;
                f_out << step << "," << time;
                for (int i = 0; i < num_chain; i++) {
                    f_out << "," << scientific << setprecision(12) << q[i] << "," << v[i];
                }
                f_out << "\n";
            }
        }

        double E_final = hamilton();

        if (save_to_file) {
            f_out.close();
        }

        return E_final;
    }

    double simplex_velocity_verle(long long num_steps, bool save_to_file = false,
                                  const string& filename = "symplectic_verlet.csv") {
        ofstream f_out;
        if (save_to_file) {
            f_out.open(filename);
            f_out << "step,time";
            for (int i = 0; i < num_chain; i++) {
                f_out << ",q" << i << ",v" << i;
            }
            f_out << "\n";

            f_out << "0,0.0";
            for (int i = 0; i < num_chain; i++) {
                f_out << "," << scientific << setprecision(12) << q[i] << "," << v[i];
            }
            f_out << "\n";
        }

        for (long long step = 1; step <= num_steps; ++step) {
            for (int i = 0; i < num_chain; i++) {
                q[i] = q[i] + v[i] * xi * dt;
            }

            for (int i = 0; i < num_chain; i++) {
                a[i] = comput_force(i) / mass;
            }

            for (int i = 0; i < num_chain; i++) {
                v[i] = v[i] + 0.5 * a[i] * dt;
            }

            for (int i = 0; i < num_chain; i++) {
                q[i] = q[i] + v[i] * (1.0 - 2.0 * xi) * dt;
            }

            for (int i = 0; i < num_chain; i++) {
                a[i] = comput_force(i) / mass;
            }

            for (int i = 0; i < num_chain; i++) {
                v[i] = v[i] + 0.5 * a[i] * dt;
            }

            for (int i = 0; i < num_chain; i++) {
                q[i] = q[i] + v[i] * xi * dt;
            }

            if (save_to_file && step % 100 == 0) {
                double time = step * dt;
                f_out << step << "," << time;
                for (int i = 0; i < num_chain; i++) {
                    f_out << "," << scientific << setprecision(12) << q[i] << "," << v[i];
                }
                f_out << endl;
            }
        }

        double E_final = hamilton();

        if (save_to_file) {
            f_out.close();
        }

        return E_final;
    }

    double hamilton() {
        double energy = 0.0;

        for (int i = 0; i < num_chain; i++) {
            energy += 0.5 * mass * v[i] * v[i];
            int next = next_index(i);
            energy += fpu(q[next] - q[i]);
        }

        return energy;
    }

    void reset() {
        fill(q.begin(), q.end(), 0.0);
        fill(v.begin(), v.end(), 0.0);
        fill(a.begin(), a.end(), 0.0);
    }

    void compare_methods(long long num_steps, double q0) {
        cout << "num steps = " << num_steps << endl;
        cout << "Velocity Verlet" << endl;
        init_soliton_displacement(q0);
        double E0 = hamilton();
        auto start_vv = chrono::high_resolution_clock::now();
        double E_VV = velocity_verlet(num_steps, false, "velocity_verlet_energy.csv");
        auto end_vv = chrono::high_resolution_clock::now();

        auto duration_vv = chrono::duration_cast<chrono::milliseconds>(end_vv - start_vv);

        cout << "final energy: " << E_VV << endl;
        cout << "execution time: " << duration_vv.count() / 1000.0 << " s" << endl;

        cout << "Symplectic Velocity Verlet" << endl;
        reset();
        init_soliton_displacement(q0);
        auto start_sv = chrono::high_resolution_clock::now();
        double E_VVV = simplex_velocity_verle(num_steps, false, "symplectic_verlet_energy.csv");
        auto end_sv = chrono::high_resolution_clock::now();

        auto duration_sv = chrono::duration_cast<chrono::milliseconds>(end_sv - start_sv);
        cout << "final energy: " << E_VVV << endl;
        cout << "execution time: " << duration_sv.count() / 1000.0 << " s" << endl;

        cout << "Velocity Verlet error = " << scientific << abs(E0 - E_VV) << endl;
        cout << "Symplectic Verlet error = " << scientific << abs(E0 - E_VVV) << endl;
        cout << "\n\n\n";
    }
};

int main() {
    // {
    //     double alpha = 1.0;
    //     double beta = 1.0;
    //     int n = 1000;
    //     double dt = 0.01;
    //     long long total_steps = 1e6;
    //     double q0 = 0.19;
    //     double mass = 1.0;

    //     unique_ptr<FPUChain> chain = make_unique<FPUChain>(n, alpha, beta, mass, dt,
    //     total_steps);

    //     chain->compare_methods(total_steps, q0);
    //     // chain->compare_methods(total_steps * 10, q0);
    //     // chain->compare_methods(total_steps * 100, q0);
    // }

    {
        double alpha = 0.0;
        double beta = 47000.5;
        int n = 1000;
        double dt = 0.00001;
        long long total_steps = 1e6;
        double q0 = 0.19;
        double mass = 1.0;

        unique_ptr<FPUChain> chain = make_unique<FPUChain>(n, alpha, beta, mass, dt, total_steps);

        chain->init_soliton_displacement(q0);
        auto res = chain->velocity_verlet(total_steps, true, "../labs/lr3/src/velocity_verlet.csv");

        cout << "python part" << endl;
        system("python ../labs/lr3/src/main.py");
    }
    return 0;
}