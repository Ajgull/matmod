#include <omp.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
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
    Solver(int num_steps, int num_years) : num_steps(num_steps) {
        sun_initial_pos = {0.0, 0.0};
        planet_initial_pos = {Constants::R_SUN_PLANET, 0.0};
        sputnik_initial_pos = {Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK, 0.0};

        v_planet = {0.0, Constants::V_PLANET};
        v_sputnik = {0.0, Constants::V_PLANET + Constants::V_SPUTNIK};

        time_to_around =
            2 * M_PI *
            sqrt(Constants::R_SUN_PLANET * Constants::R_SUN_PLANET * Constants::R_SUN_PLANET /
                 (Constants::G * Constants::M_SUN));  // Третий закон Кеплера
        cout << "time to make a circle around sun = " << time_to_around << endl;

        step = num_years * time_to_around / num_steps;
    }

    vector<State>& get_trajectory() { return trajectory; }

    double get_time_to_around() { return time_to_around; }

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

struct SpaceCraftState {
    pair<double, double> craft_pos, v_craft;
    double mass_space_craft;
    double time;

    SpaceCraftState()
        : craft_pos({0.0, 0.0}), v_craft({0.0, 0.0}), mass_space_craft(0.0), time(0.0) {}

    SpaceCraftState(pair<double, double> pos, pair<double, double> vel, double mass, double t)
        : craft_pos(pos), v_craft(vel), mass_space_craft(mass), time(t) {}
};

class SpaceCraftSolver {
   private:
    vector<State>* base_traj;
    double planet_period;
    double orbit_radius;  // R_PLANET + H
    int steps_burn, steps_coast;
    double dt_burn, dt_coast;
    double best_mass, best_angle, best_length;
    vector<SpaceCraftState> trajectory;

    double norm(const pair<double, double>& v) {
        return sqrt(v.first * v.first + v.second * v.second);
    }

    pair<double, double> gravity_acc(const SpaceCraftState& craft, double t) {
        State sys_state = getStateAtTime(t);

        double r_sun_x = craft.craft_pos.first;
        double r_sun_y = craft.craft_pos.second;

        double r_planet_x = craft.craft_pos.first - sys_state.planet_pos.first;
        double r_planet_y = craft.craft_pos.second - sys_state.planet_pos.second;

        double r_sputnik_x = craft.craft_pos.first - sys_state.sputnik_pos.first;
        double r_sputnik_y = craft.craft_pos.second - sys_state.sputnik_pos.second;

        double r_sun_norm = norm({r_sun_x, r_sun_y});
        double r_planet_norm = norm({r_planet_x, r_planet_y});
        double r_sputnik_norm = norm({r_sputnik_x, r_sputnik_y});

        double a_sun_x =
            -Constants::G * Constants::M_SUN * r_sun_x / (r_sun_norm * r_sun_norm * r_sun_norm);
        double a_sun_y =
            -Constants::G * Constants::M_SUN * r_sun_y / (r_sun_norm * r_sun_norm * r_sun_norm);

        double a_planet_x = -Constants::G * Constants::M_PLANET * r_planet_x /
                            (r_planet_norm * r_planet_norm * r_planet_norm);
        double a_planet_y = -Constants::G * Constants::M_PLANET * r_planet_y /
                            (r_planet_norm * r_planet_norm * r_planet_norm);

        double a_sputnik_x = -Constants::G * Constants::M_SPUTNIK * r_sputnik_x /
                             (r_sputnik_norm * r_sputnik_norm * r_sputnik_norm);
        double a_sputnik_y = -Constants::G * Constants::M_SPUTNIK * r_sputnik_y /
                             (r_sputnik_norm * r_sputnik_norm * r_sputnik_norm);

        return {a_sun_x + a_planet_x + a_sputnik_x, a_sun_y + a_planet_y + a_sputnik_y};
    }

    State getStateAtTime(double t) const {
        if (base_traj->empty()) return State();

        int left = 0;
        int right = static_cast<int>(base_traj->size()) - 1;

        while (right - left > 1) {
            int mid = (left + right) / 2;
            if ((*base_traj)[mid].time < t) {
                left = mid;
            } else {
                right = mid;
            }
        }

        const State& s1 = (*base_traj)[left];
        const State& s2 = (*base_traj)[right];

        double dt = s2.time - s1.time;
        double alpha = (t - s1.time) / dt;
        double beta = 1.0 - alpha;

        State result;
        result.sun_pos = s1.sun_pos;

        result.planet_pos = {beta * s1.planet_pos.first + alpha * s2.planet_pos.first,
                             beta * s1.planet_pos.second + alpha * s2.planet_pos.second};

        result.sputnik_pos = {beta * s1.sputnik_pos.first + alpha * s2.sputnik_pos.first,
                              beta * s1.sputnik_pos.second + alpha * s2.sputnik_pos.second};

        result.v_planet = {beta * s1.v_planet.first + alpha * s2.v_planet.first,
                           beta * s1.v_planet.second + alpha * s2.v_planet.second};

        result.v_sputnik = {beta * s1.v_sputnik.first + alpha * s2.v_sputnik.first,
                            beta * s1.v_sputnik.second + alpha * s2.v_sputnik.second};

        result.time = t;
        return result;
    }

    SpaceCraftState derivatives_active(const SpaceCraftState& craft, double t, double fuel_rate) {
        pair<double, double> g_acc = gravity_acc(craft, t);

        double v_norm = norm(craft.v_craft);
        pair<double, double> thrust_dir =
            (v_norm > 1e-10)
                ? make_pair(craft.v_craft.first / v_norm, craft.v_craft.second / v_norm)
                : make_pair(1.0, 0.0);

        double thrust_acc_val = Constants::V_EXP * fuel_rate / craft.mass_space_craft;
        pair<double, double> thrust_acc = {thrust_dir.first * thrust_acc_val,
                                           thrust_dir.second * thrust_acc_val};

        SpaceCraftState deriv;
        deriv.craft_pos = craft.v_craft;
        deriv.v_craft = {g_acc.first + thrust_acc.first, g_acc.second + thrust_acc.second};
        deriv.mass_space_craft = -fuel_rate;
        return deriv;
    }

    SpaceCraftState derivatives_passive(const SpaceCraftState& craft, double t) {
        pair<double, double> total_acc = gravity_acc(craft, t);

        SpaceCraftState deriv;
        deriv.craft_pos = craft.v_craft;
        deriv.v_craft = total_acc;
        deriv.mass_space_craft = 0.0;
        deriv.time = 1.0;
        return deriv;
    }

    SpaceCraftState rk4_step(const SpaceCraftState& sc, double t, double dt, bool active,
                             double fuel_rate) {
        auto get_deriv = [&](const SpaceCraftState& s, double tt) -> SpaceCraftState {
            return active ? derivatives_active(s, tt, fuel_rate) : derivatives_passive(s, tt);
        };

        SpaceCraftState k1 = get_deriv(sc, t);
        SpaceCraftState sc2 = {{sc.craft_pos.first + k1.craft_pos.first * dt * 0.5,
                                sc.craft_pos.second + k1.craft_pos.second * dt * 0.5},
                               {sc.v_craft.first + k1.v_craft.first * dt * 0.5,
                                sc.v_craft.second + k1.v_craft.second * dt * 0.5},
                               sc.mass_space_craft + k1.mass_space_craft * dt * 0.5,
                               sc.time + dt * 0.5};

        SpaceCraftState k2 = get_deriv(sc2, t + dt * 0.5);
        SpaceCraftState sc3 = {{sc.craft_pos.first + k2.craft_pos.first * dt * 0.5,
                                sc.craft_pos.second + k2.craft_pos.second * dt * 0.5},
                               {sc.v_craft.first + k2.v_craft.first * dt * 0.5,
                                sc.v_craft.second + k2.v_craft.second * dt * 0.5},
                               sc.mass_space_craft + k2.mass_space_craft * dt * 0.5,
                               sc.time + dt * 0.5};

        SpaceCraftState k3 = get_deriv(sc3, t + dt * 0.5);
        SpaceCraftState sc4 = {
            {sc.craft_pos.first + k3.craft_pos.first * dt,
             sc.craft_pos.second + k3.craft_pos.second * dt},
            {sc.v_craft.first + k3.v_craft.first * dt, sc.v_craft.second + k3.v_craft.second * dt},
            sc.mass_space_craft + k3.mass_space_craft * dt,
            sc.time + dt};

        SpaceCraftState k4 = get_deriv(sc4, t + dt);

        auto weighted_sum = [](const pair<double, double>& a, const pair<double, double>& b,
                               const pair<double, double>& c,
                               const pair<double, double>& d) -> pair<double, double> {
            return {(a.first + 2.0 * b.first + 2.0 * c.first + d.first) / 6.0,
                    (a.second + 2.0 * b.second + 2.0 * c.second + d.second) / 6.0};
        };

        return {
            {sc.craft_pos.first +
                 weighted_sum(k1.craft_pos, k2.craft_pos, k3.craft_pos, k4.craft_pos).first * dt,
             sc.craft_pos.second +
                 weighted_sum(k1.craft_pos, k2.craft_pos, k3.craft_pos, k4.craft_pos).second * dt},
            {sc.v_craft.first +
                 weighted_sum(k1.v_craft, k2.v_craft, k3.v_craft, k4.v_craft).first * dt,
             sc.v_craft.second +
                 weighted_sum(k1.v_craft, k2.v_craft, k3.v_craft, k4.v_craft).second * dt},
            sc.mass_space_craft + (k1.mass_space_craft + 2.0 * k2.mass_space_craft +
                                   2.0 * k3.mass_space_craft + k4.mass_space_craft) *
                                      dt / 6.0,
            sc.time + dt};
    }

    double run(double angle, double mass, double total_time) {
        double phi = angle * M_PI / 180.0;
        double fuel = mass - Constants::M0 - mass * Constants::PART_STRUCT_FRACTION;

        if (fuel <= 0.0) {
            return 1e20;
        }

        double fuel_rate = fuel / Constants::T_BURN;
        State s0 = getStateAtTime(0.0);

        double height = orbit_radius;
        double vel = sqrt(Constants::G * Constants::M_PLANET / height);

        pair<double, double> rel_pos = {height * cos(phi), height * sin(phi)};
        pair<double, double> rel_vel = {-vel * sin(phi), vel * cos(phi)};  // орбитальная скорость

        SpaceCraftState craft(
            {s0.planet_pos.first + rel_pos.first, s0.planet_pos.second + rel_pos.second},
            {s0.v_planet.first + rel_vel.first, s0.v_planet.second + rel_vel.second}, mass, 0.0);

        double t = 0.0;
        double min_dist = 1e20;

        while (t < Constants::T_BURN) {
            double step = min(dt_burn, Constants::T_BURN - t);
            craft = rk4_step(craft, t, step, true, fuel_rate);
            t += step;

            State sys = getStateAtTime(t);
            double dist_planet = norm({craft.craft_pos.first - sys.planet_pos.first,
                                       craft.craft_pos.second - sys.planet_pos.second}) -
                                 Constants::R_PLANET;
            if (dist_planet <= 0.0) {
                return 1e20;
            }
        }

        craft.mass_space_craft = Constants::M0;

        while (t < total_time) {
            State sys = getStateAtTime(t);

            double dist_center = norm({craft.craft_pos.first - sys.sputnik_pos.first,
                                       craft.craft_pos.second - sys.sputnik_pos.second});
            double dist_surface = dist_center - Constants::R_SPUTNIK;

            if (dist_surface < min_dist) {
                min_dist = dist_surface;
            }

            if (dist_surface <= 0.0) {
                return -1.0;
            }

            double dist_planet = norm({craft.craft_pos.first - sys.planet_pos.first,
                                       craft.craft_pos.second - sys.planet_pos.second}) -
                                 Constants::R_PLANET;
            if (dist_planet <= 0.0) return 1e20;

            double step = min(dt_coast, total_time - t);
            craft = rk4_step(craft, t, step, false, 0.0);
            t += step;
        }

        return min_dist;
    }

    void find_best_fuel_angle(double total_time) {
        best_mass = 1e20;
        best_angle = 0.0;
        best_length = 1e20;

        for (double mass = 60000.0; mass <= 70000.0; mass += 1000.0) {
            double local_best_angle = 0.0;
            double local_best_dist = 1e20;

            for (double angle = 180.0; angle <= 240.0; angle += 1.0) {
                double dist = run(angle, mass, total_time);

                if (dist <= 0.0) {
                    local_best_dist = dist;
                    local_best_angle = angle;
                    break;
                }

                if (dist < local_best_dist) {
                    local_best_dist = dist;
                    local_best_angle = angle;
                }
            }

            if (local_best_dist < best_length) {
                best_length = local_best_dist;
                best_angle = local_best_angle;
                best_mass = mass;
            }

            cout << "Mass: " << mass << ", Best angle: " << local_best_angle
                 << ", Min dist: " << scientific << local_best_dist << endl;

            if (best_length <= 0.0) {
                break;
            }
        }
    }

   public:
    SpaceCraftSolver(vector<State>* traj, double period, int n_burn, int n_coast)
        : base_traj(traj), planet_period(period), orbit_radius(Constants::R_PLANET + Constants::H) {
        steps_burn = n_burn;
        steps_coast = n_coast;
        dt_burn = Constants::T_BURN / steps_burn;
        dt_coast = (2.0 * period) / steps_coast;
    }

    void run_computations() {
        double total_time = 2.0 * planet_period + Constants::T_BURN;
        find_best_fuel_angle(total_time);
        saveOptimalTrajectory(total_time);
    }

    void saveOptimalTrajectory(double total_time) {
        const string filename = "../labs/lr1/trajectory_space_craft.csv";
        ofstream file(filename);

        file << "time,x_spacecraft,y_spacecraft,x_sputnik,y_sputnik" << endl;
        file << scientific << setprecision(10);

        double phi = best_angle * M_PI / 180.0;
        double fuel = best_mass - Constants::M0 - best_mass * Constants::PART_STRUCT_FRACTION;
        double fuel_rate = fuel / Constants::T_BURN;

        State s0 = getStateAtTime(0.0);
        double height = orbit_radius;
        double vel = sqrt(Constants::G * Constants::M_PLANET / height);

        SpaceCraftState craft(
            {s0.planet_pos.first + height * cos(phi), s0.planet_pos.second + height * sin(phi)},
            {s0.v_planet.first - vel * sin(phi), s0.v_planet.second + vel * cos(phi)}, best_mass,
            0.0);

        double t = 0.0;

        file << 0.0 << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
             << s0.sputnik_pos.first << "," << s0.sputnik_pos.second << endl;

        while (t < Constants::T_BURN) {
            double step = min(dt_burn, Constants::T_BURN - t);
            craft = rk4_step(craft, t, step, true, fuel_rate);
            t += step;

            State sys = getStateAtTime(t);
            file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                 << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << endl;
        }

        craft.mass_space_craft = Constants::M0;

        while (t < total_time) {
            double step = min(dt_coast, total_time - t);
            craft = rk4_step(craft, t, step, false, 0.0);
            t += step;

            State sys = getStateAtTime(t);
            file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                 << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << endl;

            double dist = norm({craft.craft_pos.first - sys.sputnik_pos.first,
                                craft.craft_pos.second - sys.sputnik_pos.second}) -
                          Constants::R_SPUTNIK;
            if (dist <= 0.0) break;
        }

        file.close();
    }
};

int main() {
    cout << "sun, planet, sputnik, part 1" << endl;

    auto start = std::chrono::steady_clock::now();
    bool add_spaace_apparat = false;
    int num_steps = 1000000;
    int num_years = 1;
    unique_ptr<Solver> task1 = make_unique<Solver>(num_steps, num_years);
    task1->run_computations();
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "time of 1 part computations = " << duration.count() << endl;

    // task1->saveToCSV("../labs/lr1/trajectory_full.csv");
    cout << "haha" << endl;
    // system("python ../labs/lr1/main.py");

    cout << "space apparat part 2" << endl;

    auto start_2 = std::chrono::steady_clock::now();
    int num_steps_burn = 10000;
    int num_steps_coast = 10000;

    auto task2 = make_unique<SpaceCraftSolver>(
        &task1->get_trajectory(), task1->get_time_to_around(), num_steps_burn, num_steps_coast);

    task2->run_computations();

    auto end_2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> total_time = end_2 - start_2;

    cout << "\nTotal computation time: " << total_time.count() << " s" << endl;

    system("python ../labs/lr1/part.py");

    return 0;
}