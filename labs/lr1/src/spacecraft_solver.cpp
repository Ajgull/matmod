#include "spacecraft_solver.h"

#include <omp.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "consts.h"
#include "solver.h"

using namespace std;

SpaceCraftSolver::SpaceCraftSolver(vector<State>* traj, double period, int n_burn, int n_coast)
    : base_traj(traj), planet_period(period), orbit_radius(Constants::R_PLANET + Constants::H) {
    steps_burn = n_burn;
    steps_coast = n_coast;
    dt_burn = Constants::T_BURN / steps_burn;

    planet_period =
        2.0 * M_PI *
        sqrt(pow(Constants::R_PLANET_SPUTNIK, 3) / (Constants::G * Constants::M_PLANET));
    dt_coast = (2.0 * period) / steps_coast;
}

double SpaceCraftSolver::norm(const pair<double, double>& v) {
    return sqrt(v.first * v.first + v.second * v.second);
}

pair<double, double> SpaceCraftSolver::gravity_acc(const SpaceCraftState& craft, double t) {
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

State SpaceCraftSolver::get_state_at_time(double t) const {
    if (base_traj->empty()) {
        return State();
    }

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

SpaceCraftState SpaceCraftSolver::compute_derivatives(const SpaceCraftState& craft, double t,
                                                      bool active, double fuel_rate) {
    pair<double, double> g_acc = gravity_acc(craft, t);
    SpaceCraftState deriv;
    deriv.craft_pos = craft.v_craft;
    deriv.time = craft.time;

    if (active) {
        double v_norm = norm(craft.v_craft);
        pair<double, double> thrust_dir;
        if (v_norm > 1e-10) {
            thrust_dir = make_pair(craft.v_craft.first / v_norm, craft.v_craft.second / v_norm);
        } else {
            thrust_dir = make_pair(1.0, 0.0);
        }

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

SpaceCraftState SpaceCraftSolver::runge_step(const SpaceCraftState& sc, double t, double dt,
                                             bool active, double fuel_rate) {
    SpaceCraftState k1 = compute_derivatives(sc, t, active, fuel_rate);

    SpaceCraftState s2;
    s2.craft_pos = {sc.craft_pos.first + k1.craft_pos.first * dt * 0.5,
                    sc.craft_pos.second + k1.craft_pos.second * dt * 0.5};
    s2.v_craft = {sc.v_craft.first + k1.v_craft.first * dt * 0.5,
                  sc.v_craft.second + k1.v_craft.second * dt * 0.5};
    s2.mass_space_craft = sc.mass_space_craft + k1.mass_space_craft * dt * 0.5;
    s2.time = sc.time + dt * 0.5;
    SpaceCraftState k2 = compute_derivatives(s2, t + dt * 0.5, active, fuel_rate);

    SpaceCraftState s3;
    s3.craft_pos = {sc.craft_pos.first + k2.craft_pos.first * dt * 0.5,
                    sc.craft_pos.second + k2.craft_pos.second * dt * 0.5};
    s3.v_craft = {sc.v_craft.first + k2.v_craft.first * dt * 0.5,
                  sc.v_craft.second + k2.v_craft.second * dt * 0.5};
    s3.mass_space_craft = sc.mass_space_craft + k2.mass_space_craft * dt * 0.5;
    s3.time = sc.time + dt * 0.5;
    SpaceCraftState k3 = compute_derivatives(s3, t + dt * 0.5, active, fuel_rate);

    SpaceCraftState s4;
    s4.craft_pos = {sc.craft_pos.first + k3.craft_pos.first * dt,
                    sc.craft_pos.second + k3.craft_pos.second * dt};
    s4.v_craft = {sc.v_craft.first + k3.v_craft.first * dt,
                  sc.v_craft.second + k3.v_craft.second * dt};
    s4.mass_space_craft = sc.mass_space_craft + k3.mass_space_craft * dt;
    s4.time = sc.time + dt;
    SpaceCraftState k4 = compute_derivatives(s4, t + dt, active, fuel_rate);

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

SpaceCraftState SpaceCraftSolver::init_space_craft(double angle_deg, double mass) {
    double phi = angle_deg * M_PI / 180.0;
    State s0 = get_state_at_time(0.0);
    double vel = sqrt(Constants::G * Constants::M_PLANET / orbit_radius);

    return SpaceCraftState(
        {s0.planet_pos.first + orbit_radius * cos(phi),
         s0.planet_pos.second + orbit_radius * sin(phi)},
        {s0.v_planet.first - vel * sin(phi), s0.v_planet.second + vel * cos(phi)}, mass, 0.0);
}

double SpaceCraftSolver::try_flight(double angle, double mass, double total_time) {
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

void SpaceCraftSolver::find_best_fuel_angle_omp(double total_time) {
    cout << "parallel find fuel and angle" << endl;
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

        double mass_start = 39947.0;
        double mass_end = 49978.0;
        double mass_step = 0.01;
        int num_masses = static_cast<int>((mass_end - mass_start) / mass_step) + 1;

#pragma omp for schedule(dynamic)
        for (int mass_idx = 0; mass_idx < num_masses; mass_idx++) {
            if (found_collision) {
                continue;
            }

            double mass = mass_start + mass_idx * mass_step;

            double local_angle = 0.0;
            double local_dist = 1e20;
            bool collision_found = false;

            for (double angle = 119.90; angle <= 119.99; angle += 0.01) {
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
                cout << "mass " << mass << ", angle: " << local_angle << ", min_dist " << scientific
                     << local_dist << endl;
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

    cout << "mass " << best_mass << ", angle " << best_angle << ", dist " << best_length << endl;
}

void SpaceCraftSolver::find_best_fuel_angle(double total_time) {
    best_mass = 1e20;
    best_angle = 0.0;
    best_length = 1e20;

    cout << "NOT parallel find fuel and angle" << endl;

    for (double mass = 39947.0; mass <= 39978.0; mass += 0.01) {
        double local_best_angle = 0.0;
        double local_best_dist = 1e20;

        for (double angle = 119.80; angle <= 119.00; angle += 0.01) {
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

void SpaceCraftSolver::run_computations(bool type_of_finding) {
    double total_time = planet_period + Constants::T_BURN;

    if (type_of_finding) {
        find_best_fuel_angle_omp(total_time);
        save_to_csv(total_time);
    } else {
        find_best_fuel_angle(total_time);
        save_to_csv(total_time);
    }
}

void SpaceCraftSolver::save_to_csv(double total_time) {
    ofstream file("../labs/lr1/trajectory_space_craft.csv");
    file << "time,x_spacecraft,y_spacecraft,x_sputnik,y_sputnik\n";

    double fuel = best_mass - Constants::M0 - best_mass * Constants::PART_STRUCT_FRACTION;

    cout << "fuel mass = " << fuel << ", best angle = " << best_angle
         << ", best total mass = " << best_mass << endl;
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
                file << t << "," << craft.craft_pos.first << "," << craft.craft_pos.second << ","
                     << sys.sputnik_pos.first << "," << sys.sputnik_pos.second << "\n";
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