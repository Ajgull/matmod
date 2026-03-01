#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class Runge {
   private:
    double a, b;
    double step;
    double x0, y0, z0;
    int num_steps;
    vector<double> t, x, y, z;

    double fx(double x, double y, double z) const { return a * (y - x); }

    double fy(double x, double y, double z) const { return b * x + y - x * z; }

    double fz(double x, double y, double z) const { return -8.0 / 3.0 * z + x * y; }

   public:
    Runge(double a, double b, double step, double x0, double y0, double z0, int steps)
        : a(a), b(b), step(step), x0(x0), y0(y0), z0(z0), num_steps(steps) {}

    void run() {
        t.clear();
        x.clear();
        y.clear();
        z.clear();

        double cur_t = 0.0;
        double cur_x = x0;
        double cur_y = y0;
        double cur_z = z0;

        t.push_back(cur_t);
        x.push_back(cur_x);
        y.push_back(cur_y);
        z.push_back(cur_z);

        for (int i = 0; i < num_steps; ++i) {
            double k_1_x = fx(cur_x, cur_y, cur_z);
            double k_1_y = fy(cur_x, cur_y, cur_z);
            double k_1_z = fz(cur_x, cur_y, cur_z);

            double k_2_x =
                fx(cur_x + step / 2 * k_1_x, cur_y + step / 2 * k_1_y, cur_z + step / 2 * k_1_z);
            double k_2_y =
                fy(cur_x + step / 2 * k_1_x, cur_y + step / 2 * k_1_y, cur_z + step / 2 * k_1_z);
            double k_2_z =
                fz(cur_x + step / 2 * k_1_x, cur_y + step / 2 * k_1_y, cur_z + step / 2 * k_1_z);

            double k_3_x =
                fx(cur_x + step / 2 * k_2_x, cur_y + step / 2 * k_2_y, cur_z + step / 2 * k_2_z);
            double k_3_y =
                fy(cur_x + step / 2 * k_2_x, cur_y + step / 2 * k_2_y, cur_z + step / 2 * k_2_z);
            double k_3_z =
                fz(cur_x + step / 2 * k_2_x, cur_y + step / 2 * k_2_y, cur_z + step / 2 * k_2_z);

            double k_4_x = fx(cur_x + step * k_3_x, cur_y + step * k_3_y, cur_z + step * k_3_z);
            double k_4_y = fy(cur_x + step * k_3_x, cur_y + step * k_3_y, cur_z + step * k_3_z);
            double k_4_z = fz(cur_x + step * k_3_x, cur_y + step * k_3_y, cur_z + step * k_3_z);

            cur_t += step;
            cur_x += step / 6 * (k_1_x + 2 * k_2_x + 2 * k_3_x + k_4_x);
            cur_y += step / 6 * (k_1_y + 2 * k_2_y + 2 * k_3_y + k_4_y);
            cur_z += step / 6 * (k_1_z + 2 * k_2_z + 2 * k_3_z + k_4_z);

            t.push_back(cur_t);
            x.push_back(cur_x);
            y.push_back(cur_y);
            z.push_back(cur_z);
        }
    }

    void save_to_csv(const string& filename) {
        ofstream file(filename);

        file << "#a=" << a << ", b=" << b << endl;

        file << "x,y,z" << endl;

        for (size_t i = 0; i < t.size(); ++i) {
            file << x[i] << "," << y[i] << "," << z[i] << endl;
        }

        file.close();
    }
};

int main() {
    double step = 0.01;
    int steps = 5000;
    double anti_stationarity_step = 0.1;

    // {
    //     double a = 2.0;
    //     double b = -2.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "O1 - stable, O2, O3 - not exist" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 0.5;
    //     double b = 5.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "O1 - unstable, O2, O3 - stable" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 1.2;
    //     double b = 2.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "all stable" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 2.5;
    //     double b = 1.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "all stable" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 2.5;
    //     double b = 6.5;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "O1 - stable, O2 - at boarder" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 10.0;
    //     double b = 28.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "O1 - stable, O2 - unstable" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 10.0;
    //     double b = 60.0;
    //     double x0 = 0.0 + anti_stationarity_step;
    //     double y0 = 0.0 + anti_stationarity_step;
    //     double z0 = 0.0 + anti_stationarity_step;
    //     cout << "big parameters" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 2.5;
    //     double b = 6.5;
    //     double val = sqrt(8.0 / 3.0 * (b + 1));
    //     double x0 = val + anti_stationarity_step;
    //     double y0 = val + anti_stationarity_step;
    //     double z0 = b + 1 + anti_stationarity_step;
    //     cout << "O1 - unstable, O2 - unstable, start - O2" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 10.0;
    //     double b = 28.0;
    //     double val = sqrt(8.0 / 3.0 * (b + 1));
    //     double x0 = val + anti_stationarity_step;
    //     double y0 = val + anti_stationarity_step;
    //     double z0 = b + 1 + anti_stationarity_step;
    //     cout << "O1 - unstable, O2 - unstable, start - O2" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 10.0;
    //     double b = 29.0;
    //     double val = sqrt(8.0 / 3.0 * (b + 1));
    //     double x0 = -val - anti_stationarity_step;
    //     double y0 = -val - anti_stationarity_step;
    //     double z0 = b + 1 + anti_stationarity_step;
    //     cout << "O1 - unstable, O2 - unstable, start - O3" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    // {
    //     double a = 10.0;
    //     double b = 100.0;
    //     double val = sqrt(8.0 / 3.0 * (b + 1));
    //     double x0 = val + anti_stationarity_step;
    //     double y0 = val + anti_stationarity_step;
    //     double z0 = b + 1 + anti_stationarity_step;
    //     cout << "big numbers again" << endl;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    {
        double a = 02.5;
        double b = 0.00;
        double val = sqrt(8.0 / 3.0 * (b + 1));
        double x0 = -val + anti_stationarity_step;
        double y0 = -val + anti_stationarity_step;
        double z0 = b + 1 + anti_stationarity_step;
        cout << "big numbers again" << endl;
        Runge solver(a, b, step, x0, y0, z0, steps);
        solver.run();
        solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
        system("python ../labs/lr2/src/main.py");
    }

    // {
    //     double a = 0.8;
    //     double b = 5.0;
    //     double val = sqrt(8.0 / 3.0 * (b + 1));
    //     double x0 = -val + anti_stationarity_step;
    //     double y0 = -val + anti_stationarity_step;
    //     double z0 = b + 1 + anti_stationarity_step;
    //     Runge solver(a, b, step, x0, y0, z0, steps);
    //     solver.run();
    //     solver.save_to_csv("../labs/lr2/src/strange_attractor.csv");
    //     system("python ../labs/lr2/src/main.py");
    // }

    return 0;
}