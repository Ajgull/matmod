#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "consts.h"

using namespace std;

struct State {
    pair<double, double> sun_pos, planet_pos, sputnik_pos;
    pair<double, double> v_planet, v_sputnik;
    double time;

    State() {
        sun_pos = {0.0, 0.0};
        planet_pos = {0.0, 0.0};
        sputnik_pos = {0.0, 0.0};
        time = 0;
    }

    State(pair<double, double>& sun_pos, pair<double, double>& planet_pos,
          pair<double, double>& sputnik_pos, pair<double, double>& v_planet,
          pair<double, double>& v_sputnik)
        : sun_pos(sun_pos),
          planet_pos(planet_pos),
          sputnik_pos(sputnik_pos),
          v_planet(v_planet),
          v_sputnik(v_sputnik) {
        time = 0;
    }

    State(const State& other) {
        sun_pos = other.sun_pos;
        planet_pos = other.planet_pos;
        sputnik_pos = other.sputnik_pos;
        v_planet = other.v_planet;
        v_sputnik = other.v_sputnik;
        time = other.time;
    }
};

class Solver {
   private:
    pair<double, double> sun_initial_pos, planet_initial_pos, sputnik_initial_pos;
    pair<double, double> v_planet, v_sputnik;
    double time_to_around;  // for planet
    int num_steps;
    double step;
    vector<State> trajectory;

   public:
    Solver(int num_steps) : num_steps(num_steps) {
        sun_initial_pos = {0.0, 0.0};
        planet_initial_pos = {Constants::R_SUN_PLANET, 0.0};
        sputnik_initial_pos = {Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK, 0.0};

        v_planet = {0.0, Constants::V_PLANET};
        v_sputnik = {0.0, Constants::V_PLANET + Constants::V_SPUTNIK};

        time_to_around = 2 * M_PI *
                         sqrt(Constants::R_SUN_PLANET * Constants::R_SUN_PLANET *
                              Constants::R_SUN_PLANET / (Constants::G * Constants::M_SUN));
        cout << "time to make a circle around sun = " << time_to_around << endl;

        step = time_to_around / num_steps;
    }

    double dist(double x, double y) {
        double r = sqrt(x * x + y * y);
        return r;
    }

    void compute_derivatives(const State& s, pair<double, double>& d_planet_pos,
                             pair<double, double>& d_sputnik_pos, pair<double, double>& d_v_planet,
                             pair<double, double>& d_v_sputnik) {
        double dx_sputnik_planet = s.sputnik_pos.first - s.planet_pos.first;
        double dy_sputnik_planet = s.sputnik_pos.second - s.planet_pos.second;

        double r_sun_sputnik = dist(s.sputnik_pos.first, s.sputnik_pos.second);
        double r_sun_planet = dist(s.planet_pos.first, s.planet_pos.second);
        double r_planet_sputnik = dist(dx_sputnik_planet, dy_sputnik_planet);

        d_planet_pos.first = s.v_planet.first;
        d_planet_pos.second = s.v_planet.second;
        d_sputnik_pos.first = s.v_sputnik.first;
        d_sputnik_pos.second = s.v_sputnik.second;

        double ax_planet_sun = -Constants::G * Constants::M_SUN * s.planet_pos.first /
                               (r_sun_planet * r_sun_planet * r_sun_planet);
        double ay_planet_sun = -Constants::G * Constants::M_SUN * s.planet_pos.second /
                               (r_sun_planet * r_sun_planet * r_sun_planet);

        double ax_planet_sputnik = Constants::G * Constants::M_SPUTNIK * dx_sputnik_planet /
                                   (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);
        double ay_planet_sputnik = Constants::G * Constants::M_SPUTNIK * dy_sputnik_planet /
                                   (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);

        double ax_sputnik_sun = -Constants::G * Constants::M_SUN * s.sputnik_pos.first /
                                (r_sun_sputnik * r_sun_sputnik * r_sun_sputnik);
        double ay_sputnik_sun = -Constants::G * Constants::M_SUN * s.sputnik_pos.second /
                                (r_sun_sputnik * r_sun_sputnik * r_sun_sputnik);

        double ax_sputnik_planet = -Constants::G * Constants::M_PLANET * dx_sputnik_planet /
                                   (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);
        double ay_sputnik_planet = -Constants::G * Constants::M_PLANET * dy_sputnik_planet /
                                   (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);

        d_v_planet.first = ax_planet_sun + ax_planet_sputnik;
        d_v_planet.second = ay_planet_sun + ay_planet_sputnik;
        d_v_sputnik.first = ax_sputnik_sun + ax_sputnik_planet;
        d_v_sputnik.second = ay_sputnik_sun + ay_sputnik_planet;
    }

    void runge_step(State& s, double dt) {
        pair<double, double> k1_planet_pos, k1_sputnik_pos, k1_v_planet, k1_v_sputnik;
        pair<double, double> k2_planet_pos, k2_sputnik_pos, k2_v_planet, k2_v_sputnik;
        pair<double, double> k3_planet_pos, k3_sputnik_pos, k3_v_planet, k3_v_sputnik;
        pair<double, double> k4_planet_pos, k4_sputnik_pos, k4_v_planet, k4_v_sputnik;

        State temp;

        compute_derivatives(s, k1_planet_pos, k1_sputnik_pos, k1_v_planet, k1_v_sputnik);

        temp = s;
        temp.planet_pos.first += k1_planet_pos.first * dt / 2;
        temp.planet_pos.second += k1_planet_pos.second * dt / 2;
        temp.sputnik_pos.first += k1_sputnik_pos.first * dt / 2;
        temp.sputnik_pos.second += k1_sputnik_pos.second * dt / 2;
        temp.v_planet.first += k1_v_planet.first * dt / 2;
        temp.v_planet.second += k1_v_planet.second * dt / 2;
        temp.v_sputnik.first += k1_v_sputnik.first * dt / 2;
        temp.v_sputnik.second += k1_v_sputnik.second * dt / 2;
        temp.time = s.time + dt / 2;
        compute_derivatives(temp, k2_planet_pos, k2_sputnik_pos, k2_v_planet, k2_v_sputnik);

        temp = s;
        temp.planet_pos.first += k2_planet_pos.first * dt / 2;
        temp.planet_pos.second += k2_planet_pos.second * dt / 2;
        temp.sputnik_pos.first += k2_sputnik_pos.first * dt / 2;
        temp.sputnik_pos.second += k2_sputnik_pos.second * dt / 2;
        temp.v_planet.first += k2_v_planet.first * dt / 2;
        temp.v_planet.second += k2_v_planet.second * dt / 2;
        temp.v_sputnik.first += k2_v_sputnik.first * dt / 2;
        temp.v_sputnik.second += k2_v_sputnik.second * dt / 2;
        temp.time = s.time + dt / 2;
        compute_derivatives(temp, k3_planet_pos, k3_sputnik_pos, k3_v_planet, k3_v_sputnik);

        temp = s;
        temp.planet_pos.first += k3_planet_pos.first * dt;
        temp.planet_pos.second += k3_planet_pos.second * dt;
        temp.sputnik_pos.first += k3_sputnik_pos.first * dt;
        temp.sputnik_pos.second += k3_sputnik_pos.second * dt;
        temp.v_planet.first += k3_v_planet.first * dt;
        temp.v_planet.second += k3_v_planet.second * dt;
        temp.v_sputnik.first += k3_v_sputnik.first * dt;
        temp.v_sputnik.second += k3_v_sputnik.second * dt;
        temp.time = s.time + dt;
        compute_derivatives(temp, k4_planet_pos, k4_sputnik_pos, k4_v_planet, k4_v_sputnik);

        s.planet_pos.first += (k1_planet_pos.first + 2 * k2_planet_pos.first +
                               2 * k3_planet_pos.first + k4_planet_pos.first) *
                              dt / 6;
        s.planet_pos.second += (k1_planet_pos.second + 2 * k2_planet_pos.second +
                                2 * k3_planet_pos.second + k4_planet_pos.second) *
                               dt / 6;
        s.sputnik_pos.first += (k1_sputnik_pos.first + 2 * k2_sputnik_pos.first +
                                2 * k3_sputnik_pos.first + k4_sputnik_pos.first) *
                               dt / 6;
        s.sputnik_pos.second += (k1_sputnik_pos.second + 2 * k2_sputnik_pos.second +
                                 2 * k3_sputnik_pos.second + k4_sputnik_pos.second) *
                                dt / 6;

        s.v_planet.first += (k1_v_planet.first + 2 * k2_v_planet.first + 2 * k3_v_planet.first +
                             k4_v_planet.first) *
                            dt / 6;
        s.v_planet.second += (k1_v_planet.second + 2 * k2_v_planet.second + 2 * k3_v_planet.second +
                              k4_v_planet.second) *
                             dt / 6;
        s.v_sputnik.first += (k1_v_sputnik.first + 2 * k2_v_sputnik.first + 2 * k3_v_sputnik.first +
                              k4_v_sputnik.first) *
                             dt / 6;
        s.v_sputnik.second += (k1_v_sputnik.second + 2 * k2_v_sputnik.second +
                               2 * k3_v_sputnik.second + k4_v_sputnik.second) *
                              dt / 6;

        s.time += dt;
    }

    void run_computations() {
        State current(sun_initial_pos, planet_initial_pos, sputnik_initial_pos, v_planet,
                      v_sputnik);
        trajectory.clear();
        trajectory.push_back(current);

        for (int i = 0; i <= num_steps; i++) {
            runge_step(current, step);
            if (i % 10 == 0) {
                trajectory.push_back(current);
            }
        }
    }

    void saveToCSV(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        file << "time,x_planet,y_planet,x_sputnik,y_sputnik,"
             << "vx_planet,vy_planet,vx_sputnik,vy_sputnik" << endl;

        file << scientific << setprecision(20);

        for (const auto& s : trajectory) {
            file << s.time << "," << s.planet_pos.first << "," << s.planet_pos.second << ","
                 << s.sputnik_pos.first << "," << s.sputnik_pos.second << "," << s.v_planet.first
                 << "," << s.v_planet.second << "," << s.v_sputnik.first << ","
                 << s.v_sputnik.second << endl;
        }

        file.close();
    }
};

int main() {
    cout << "sun, planet, sputnik part 1" << endl;

    int num_steps = 100000;
    unique_ptr<Solver> task1 = make_unique<Solver>(num_steps);
    task1->run_computations();

    task1->saveToCSV("../labs/lr1/trajectory_full.csv");

    system("python ../labs/lr1/main.py");

    return 0;
}