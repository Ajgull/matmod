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
    double orbit_radius;
    int steps_burn, steps_coast;
    double dt_burn, dt_coast;
    double best_mass, best_angle, best_length;

    double norm(const pair<double, double>& v) {
        return sqrt(v.first * v.first + v.second * v.second);
    }

    pair<double, double> gravity_acc(const SpaceCraftState& craft, double t) {
        State sys = get_state_at_time(t);

        double rsx = craft.craft_pos.first;
        double rsy = craft.craft_pos.second;
        double rpx = craft.craft_pos.first - sys.planet_pos.first;
        double rpy = craft.craft_pos.second - sys.planet_pos.second;
        double rspx = craft.craft_pos.first - sys.sputnik_pos.first;
        double rspy = craft.craft_pos.second - sys.sputnik_pos.second;

        double rs = norm({rsx, rsy});
        double rp = norm({rpx, rpy});
        double rsp = norm({rspx, rspy});

        const double eps = 1e-6;
        rs = max(rs, eps);
        rp = max(rp, eps);
        rsp = max(rsp, eps);

        double ax = -Constants::G * Constants::M_SUN * rsx / (rs * rs * rs) -
                    Constants::G * Constants::M_PLANET * rpx / (rp * rp * rp) -
                    Constants::G * Constants::M_SPUTNIK * rspx / (rsp * rsp * rsp);
        double ay = -Constants::G * Constants::M_SUN * rsy / (rs * rs * rs) -
                    Constants::G * Constants::M_PLANET * rpy / (rp * rp * rp) -
                    Constants::G * Constants::M_SPUTNIK * rspy / (rsp * rsp * rsp);

        return {ax, ay};
    }

    State get_state_at_time(double t) const {
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
        double alpha = (t - s1.time) / (s2.time - s1.time);
        double beta = 1.0 - alpha;

        State res;
        res.sun_pos = s1.sun_pos;
        res.planet_pos = {beta * s1.planet_pos.first + alpha * s2.planet_pos.first,
                          beta * s1.planet_pos.second + alpha * s2.planet_pos.second};
        res.sputnik_pos = {beta * s1.sputnik_pos.first + alpha * s2.sputnik_pos.first,
                           beta * s1.sputnik_pos.second + alpha * s2.sputnik_pos.second};
        res.v_planet = {beta * s1.v_planet.first + alpha * s2.v_planet.first,
                        beta * s1.v_planet.second + alpha * s2.v_planet.second};
        res.v_sputnik = {beta * s1.v_sputnik.first + alpha * s2.v_sputnik.first,
                         beta * s1.v_sputnik.second + alpha * s2.v_sputnik.second};
        res.time = t;
        return res;
    }

    SpaceCraftState computeDerivatives(const SpaceCraftState& craft, double t, bool active,
                                       double fuel_rate) {
        pair<double, double> g_acc = gravity_acc(craft, t);
        SpaceCraftState deriv;
        deriv.craft_pos = craft.v_craft;
        deriv.time = craft.time;

        if (active) {
            double v_norm = norm(craft.v_craft);
            pair<double, double> thrust_dir =
                (v_norm > 1e-10)
                    ? make_pair(craft.v_craft.first / v_norm, craft.v_craft.second / v_norm)
                    : make_pair(1.0, 0.0);

            double thrust_acc = Constants::V_EXP * fuel_rate / craft.mass_space_craft;
            deriv.v_craft = {g_acc.first + thrust_dir.first * thrust_acc,
                             g_acc.second + thrust_dir.second * thrust_acc};
            deriv.mass_space_craft = -fuel_rate;
        } else {
            deriv.v_craft = g_acc;
            deriv.mass_space_craft = 0.0;
        }
        return deriv;
    }

    SpaceCraftState runge_step(const SpaceCraftState& sc, double t, double dt, bool active,
                               double fuel_rate) {
        SpaceCraftState k1 = computeDerivatives(sc, t, active, fuel_rate);

        SpaceCraftState s2;
        s2.craft_pos = {sc.craft_pos.first + k1.craft_pos.first * dt * 0.5,
                        sc.craft_pos.second + k1.craft_pos.second * dt * 0.5};
        s2.v_craft = {sc.v_craft.first + k1.v_craft.first * dt * 0.5,
                      sc.v_craft.second + k1.v_craft.second * dt * 0.5};
        s2.mass_space_craft = sc.mass_space_craft + k1.mass_space_craft * dt * 0.5;
        s2.time = sc.time + dt * 0.5;
        SpaceCraftState k2 = computeDerivatives(s2, t + dt * 0.5, active, fuel_rate);

        SpaceCraftState s3;
        s3.craft_pos = {sc.craft_pos.first + k2.craft_pos.first * dt * 0.5,
                        sc.craft_pos.second + k2.craft_pos.second * dt * 0.5};
        s3.v_craft = {sc.v_craft.first + k2.v_craft.first * dt * 0.5,
                      sc.v_craft.second + k2.v_craft.second * dt * 0.5};
        s3.mass_space_craft = sc.mass_space_craft + k2.mass_space_craft * dt * 0.5;
        s3.time = sc.time + dt * 0.5;
        SpaceCraftState k3 = computeDerivatives(s3, t + dt * 0.5, active, fuel_rate);

        SpaceCraftState s4;
        s4.craft_pos = {sc.craft_pos.first + k3.craft_pos.first * dt,
                        sc.craft_pos.second + k3.craft_pos.second * dt};
        s4.v_craft = {sc.v_craft.first + k3.v_craft.first * dt,
                      sc.v_craft.second + k3.v_craft.second * dt};
        s4.mass_space_craft = sc.mass_space_craft + k3.mass_space_craft * dt;
        s4.time = sc.time + dt;
        SpaceCraftState k4 = computeDerivatives(s4, t + dt, active, fuel_rate);

        SpaceCraftState result;
        result.craft_pos = {sc.craft_pos.first + (k1.craft_pos.first + 2 * k2.craft_pos.first +
                                                  2 * k3.craft_pos.first + k4.craft_pos.first) *
                                                     dt / 6.0,
                            sc.craft_pos.second + (k1.craft_pos.second + 2 * k2.craft_pos.second +
                                                   2 * k3.craft_pos.second + k4.craft_pos.second) *
                                                      dt / 6.0};
        result.v_craft = {sc.v_craft.first + (k1.v_craft.first + 2 * k2.v_craft.first +
                                              2 * k3.v_craft.first + k4.v_craft.first) *
                                                 dt / 6.0,
                          sc.v_craft.second + (k1.v_craft.second + 2 * k2.v_craft.second +
                                               2 * k3.v_craft.second + k4.v_craft.second) *
                                                  dt / 6.0};
        result.mass_space_craft =
            sc.mass_space_craft + (k1.mass_space_craft + 2 * k2.mass_space_craft +
                                   2 * k3.mass_space_craft + k4.mass_space_craft) *
                                      dt / 6.0;
        result.time = sc.time + dt;

        return result;
    }

    SpaceCraftState init_space_craft(double angle_deg, double mass) {
        double phi = angle_deg * M_PI / 180.0;
        State s0 = get_state_at_time(0.0);
        double vel = sqrt(Constants::G * Constants::M_PLANET / orbit_radius);

        return SpaceCraftState(
            {s0.planet_pos.first + orbit_radius * cos(phi),
             s0.planet_pos.second + orbit_radius * sin(phi)},
            {s0.v_planet.first - vel * sin(phi), s0.v_planet.second + vel * cos(phi)}, mass, 0.0);
    }

    double try_flight(double angle, double mass, double total_time) {
        double fuel = mass - Constants::M0 - mass * Constants::PART_STRUCT_FRACTION;
        if (fuel <= 0.0) {
            return 1e20;
        }

        double fuel_rate = fuel / Constants::T_BURN;
        SpaceCraftState craft = init_space_craft(angle, mass);
        double t = 0.0, min_dist = 1e20;

        while (t < Constants::T_BURN) {
            State sys = get_state_at_time(t);
            double step = min(dt_burn, Constants::T_BURN - t);
            craft = runge_step(craft, t, step, true, fuel_rate);
            t += step;

            double d_planet = norm({craft.craft_pos.first - sys.planet_pos.first,
                                    craft.craft_pos.second - sys.planet_pos.second}) -
                              Constants::R_PLANET;
            if (d_planet <= 0.0) {
                return 1e20;
            }
        }

        craft.mass_space_craft = Constants::M0;

        while (t < total_time) {
            State sys = get_state_at_time(t);
            double d_center = norm({craft.craft_pos.first - sys.sputnik_pos.first,
                                    craft.craft_pos.second - sys.sputnik_pos.second});
            double d_surface = d_center - Constants::R_SPUTNIK;

            if (d_surface < min_dist) {
                min_dist = d_surface;
            }
            if (d_surface <= 0.0) {
                return -1.0;
            }

            double d_planet = norm({craft.craft_pos.first - sys.planet_pos.first,
                                    craft.craft_pos.second - sys.planet_pos.second}) -
                              Constants::R_PLANET;
            if (d_planet <= 0.0) {
                return 1e20;
            }

            double step = min(dt_coast, total_time - t);
            craft = runge_step(craft, t, step, false, 0.0);
            t += step;
        }
        return min_dist;
    }

    void find_best_fuel_angle_omp(double total_time) {
        best_mass = 1e20;
        best_angle = 0.0;
        best_length = 1e20;

        int num_threads = omp_get_max_threads();

        vector<double> local_best_mass(num_threads, 1e20);
        vector<double> local_best_angle(num_threads, 0.0);
        vector<double> local_best_dist(num_threads, 1e20);
        vector<bool> local_found_collision(num_threads, false);

#pragma omp parallel
        {
            int tid = omp_get_thread_num();
            bool found_collision = false;

#pragma omp for schedule(dynamic, 10) nowait
            for (int mass_idx = 0; mass_idx < 2500; mass_idx++) {
                if (found_collision) {
                    continue;
                }

                double mass = 32558.5 + mass_idx * 0.01;
                double local_angle = 0.0;
                double local_dist = 1e20;
                bool collision_found = false;

                for (double angle = 118.78; angle <= 119.0; angle += 0.01) {
                    double dist = try_flight(angle, mass, total_time);

                    if (dist <= 0.0) {
                        local_dist = dist;
                        local_angle = angle;
                        collision_found = true;
                        found_collision = true;
                        break;
                    }

                    if (dist < local_dist) {
                        local_dist = dist;
                        local_angle = angle;
                    }
                }

                if (local_dist < local_best_dist[tid]) {
                    local_best_dist[tid] = local_dist;
                    local_best_angle[tid] = local_angle;
                    local_best_mass[tid] = mass;
                    local_found_collision[tid] = collision_found;
                }

#pragma omp critical
                {
                    cout << "mass " << mass << ", angle: " << local_angle << ", min_dist "
                         << scientific << local_dist << endl;
                }
            }
        }

        for (int tid = 0; tid < num_threads; tid++) {
            if (local_best_dist[tid] < best_length) {
                best_length = local_best_dist[tid];
                best_angle = local_best_angle[tid];
                best_mass = local_best_mass[tid];
            }
        }

        cout << "mass " << best_mass << ", angle " << best_angle << ", dist " << best_length
             << endl;
    }

    void find_best_fuel_angle(double total_time) {
        best_mass = 1e20;
        best_angle = 0.0;
        best_length = 1e20;

        for (double mass = 32558.5; mass <= 32559.0; mass += 0.01) {
            double local_best_angle = 0.0, local_best_dist = 1e20;

            for (double angle = 118.78; angle <= 119.0; angle += 0.01) {
                double dist = try_flight(angle, mass, total_time);
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
            cout << "mass " << mass << ", angle " << local_best_angle << ", min_dist "
                 << local_best_dist << endl;

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

        planet_period =
            2.0 * M_PI *
            sqrt(pow(Constants::R_PLANET_SPUTNIK, 3) / (Constants::G * Constants::M_PLANET));
        dt_coast = (2.0 * period) / steps_coast;
        cout << dt_burn << " " << dt_coast << endl;
    }

    void run_computations() {
        double total_time = planet_period + Constants::T_BURN;
        find_best_fuel_angle_omp(total_time);
        saveOptimalTrajectory(total_time);
    }

    void saveOptimalTrajectory(double total_time) {
        ofstream file("../labs/lr1/trajectory_space_craft.csv");
        file << "time,x_spacecraft,y_spacecraft,x_sputnik,y_sputnik\n";
        file << scientific << setprecision(10);

        double fuel = best_mass - Constants::M0 - best_mass * Constants::PART_STRUCT_FRACTION;
        if (fuel <= 0.0) {
            file.close();
            return;
        }

        double fuel_rate = fuel / Constants::T_BURN;
        SpaceCraftState craft = init_space_craft(best_angle, best_mass);
        double t = 0.0;
        bool collision_happened = false;

        State sys0 = get_state_at_time(0.0);
        file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
             << sys0.sputnik_pos.first << "," << sys0.sputnik_pos.second << "\n";

        while (t < Constants::T_BURN && !collision_happened) {
            double step = min(dt_burn, Constants::T_BURN - t);
            craft = runge_step(craft, t, step, true, fuel_rate);
            t += step;
            State sys = get_state_at_time(t);

            double dist_to_sputnik = norm({craft.craft_pos.first - sys.sputnik_pos.first,
                                           craft.craft_pos.second - sys.sputnik_pos.second});

            if (dist_to_sputnik <= Constants::R_SPUTNIK) {
                file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                     << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << "\n";
                collision_happened = true;
                break;
            }

            file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                 << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << "\n";
        }

        if (!collision_happened) {
            craft.mass_space_craft = Constants::M0;

            while (t < total_time && !collision_happened) {
                double step = min(dt_coast, total_time - t);
                craft = runge_step(craft, t, step, false, 0.0);
                t += step;
                State sys = get_state_at_time(t);

                double dist_to_sputnik = norm({craft.craft_pos.first - sys.sputnik_pos.first,
                                               craft.craft_pos.second - sys.sputnik_pos.second});

                if (dist_to_sputnik <= Constants::R_SPUTNIK) {
                    file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second
                         << "," << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << "\n";
                    collision_happened = true;
                    break;
                }

                double dist_from_planet = norm({craft.craft_pos.first - sys.planet_pos.first,
                                                craft.craft_pos.second - sys.planet_pos.second});
                if (dist_from_planet > 10 * Constants::R_PLANET_SPUTNIK) {
                    break;
                }

                file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                     << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << "\n";
            }
        }

        file.close();
    }
};

int main() {
    cout << "sun, planet, sputnik, part 1" << endl;

    auto start = std::chrono::steady_clock::now();
    bool add_spaace_apparat = false;
    int num_steps = 1000000;
    int num_years = 10;
    unique_ptr<Solver> task1 = make_unique<Solver>(num_steps, num_years);
    task1->run_computations();
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "time of 1 part computations = " << duration.count() << endl;

    task1->saveToCSV("../labs/lr1/trajectory_full.csv");
    cout << "haha" << endl;
    system("python ../labs/lr1/main.py");

    cout << "space apparat part 2" << endl;

    auto start_2 = std::chrono::steady_clock::now();
    int num_steps_burn = 100000;
    int num_steps_coast = 1000000;

    auto task2 = make_unique<SpaceCraftSolver>(
        &task1->get_trajectory(), task1->get_time_to_around(), num_steps_burn, num_steps_coast);

    task2->run_computations();

    auto end_2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> total_time = end_2 - start_2;

    cout << "\nTotal computation time: " << total_time.count() << " s" << endl;

    system("python ../labs/lr1/part.py");

    return 0;
}