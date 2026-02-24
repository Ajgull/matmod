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
    Solver(int num_steps) : num_steps(num_steps) {
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

        step = time_to_around / num_steps;
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
    double m_craft;
    double time;

    SpaceCraftState() : craft_pos({0.0, 0.0}), v_craft({0.0, 0.0}), m_craft(0.0), time(0.0) {}
};

struct Result {
    bool found;
    double fuel;
    double angle;
    vector<SpaceCraftState> traj;
};

class SpaceCraftSolver {
   private:
    vector<State>* planet_sputnik_trajectory;
    double planet_time_to_around;
    double m_min_fuel, r_apparat_orbit;
    double best_angle;
    bool found_solution;
    int num_steps_burn, num_steps_coast;
    vector<SpaceCraftState> best_trajectory;

    pair<double, double> grav_accel(double x, double y, double x_body, double y_body, double M) {
        double dx = x_body - x;
        double dy = y_body - y;
        const double eps = 1e-12;
        double r_sq = max(dx * dx + dy * dy, eps);
        double r_cub = r_sq * sqrt(r_sq);
        return {Constants::G * M * dx / r_cub, Constants::G * M * dy / r_cub};
    }

    bool check_crash(const SpaceCraftState& sc, const pair<double, double>& sputnik_pos) {
        double dx = sc.craft_pos.first - sputnik_pos.first;
        double dy = sc.craft_pos.second - sputnik_pos.second;
        double r = sqrt(dx * dx + dy * dy);
        return r <= Constants::R_SPUTNIK;
    }

    // Вычисление производных (уравнение Мещерского + гравитация)
    void compute_rocket_derivatives(const SpaceCraftState& sc,
                                    const pair<double, double>& planet_pos,
                                    const pair<double, double>& sputnik_pos,
                                    pair<double, double>& d_pos, pair<double, double>& d_vel,
                                    double& d_mass, double m_fuel_0, double m_struct) {
        // dx/dt = vx, dy/dt = vy
        d_pos = sc.v_craft;

        // dm/dt = -m_fuel_0 / T_BURN (линейное выгорание)
        bool thrust_active = (sc.time < Constants::T_BURN);
        d_mass = thrust_active ? -m_fuel_0 / Constants::T_BURN : 0.0;

        // Гравитационные ускорения от Солнца, планеты, спутника
        auto a_sun =
            grav_accel(sc.craft_pos.first, sc.craft_pos.second, 0.0, 0.0, Constants::M_SUN);
        auto a_planet = grav_accel(sc.craft_pos.first, sc.craft_pos.second, planet_pos.first,
                                   planet_pos.second, Constants::M_PLANET);
        auto a_sputnik = grav_accel(sc.craft_pos.first, sc.craft_pos.second, sputnik_pos.first,
                                    sputnik_pos.second, Constants::M_SPUTNIK);

        double ax_grav = a_sun.first + a_planet.first + a_sputnik.first;
        double ay_grav = a_sun.second + a_planet.second + a_sputnik.second;

        // Реактивное ускорение (уравнение Мещерского): a_react = -u * (v/|v|) * (dm/dt) / m
        double ax_react = 0.0, ay_react = 0.0;
        if (thrust_active && sc.m_craft > 1e-12) {
            double v =
                sqrt(sc.v_craft.first * sc.v_craft.first + sc.v_craft.second * sc.v_craft.second);
            if (v > 1e-12) {
                // Тяга направлена ПО касательной к траектории (по скорости)
                double thrust_acc = Constants::V_EXP * (-d_mass) / sc.m_craft;
                ax_react = thrust_acc * sc.v_craft.first / v;
                ay_react = thrust_acc * sc.v_craft.second / v;
            }
        }

        d_vel.first = ax_grav + ax_react;
        d_vel.second = ay_grav + ay_react;
    }

    // Шаг РК4 для корабля с переменной массой
    void rocket_runge_step(SpaceCraftState& sc, double dt, const pair<double, double>& planet_pos,
                           const pair<double, double>& sputnik_pos, double m_fuel_0,
                           double m_struct) {
        pair<double, double> k1_pos, k1_vel;
        double k1_mass;
        pair<double, double> k2_pos, k2_vel;
        double k2_mass;
        pair<double, double> k3_pos, k3_vel;
        double k3_mass;
        pair<double, double> k4_pos, k4_vel;
        double k4_mass;

        compute_rocket_derivatives(sc, planet_pos, sputnik_pos, k1_pos, k1_vel, k1_mass, m_fuel_0,
                                   m_struct);

        SpaceCraftState temp = sc;
        temp.craft_pos.first += k1_pos.first * dt / 2;
        temp.craft_pos.second += k1_pos.second * dt / 2;
        temp.v_craft.first += k1_vel.first * dt / 2;
        temp.v_craft.second += k1_vel.second * dt / 2;
        temp.m_craft += k1_mass * dt / 2;
        temp.time += dt / 2;
        compute_rocket_derivatives(temp, planet_pos, sputnik_pos, k2_pos, k2_vel, k2_mass, m_fuel_0,
                                   m_struct);

        temp = sc;
        temp.craft_pos.first += k2_pos.first * dt / 2;
        temp.craft_pos.second += k2_pos.second * dt / 2;
        temp.v_craft.first += k2_vel.first * dt / 2;
        temp.v_craft.second += k2_vel.second * dt / 2;
        temp.m_craft += k2_mass * dt / 2;
        temp.time += dt / 2;
        compute_rocket_derivatives(temp, planet_pos, sputnik_pos, k3_pos, k3_vel, k3_mass, m_fuel_0,
                                   m_struct);

        temp = sc;
        temp.craft_pos.first += k3_pos.first * dt;
        temp.craft_pos.second += k3_pos.second * dt;
        temp.v_craft.first += k3_vel.first * dt;
        temp.v_craft.second += k3_vel.second * dt;
        temp.m_craft += k3_mass * dt;
        temp.time += dt;
        compute_rocket_derivatives(temp, planet_pos, sputnik_pos, k4_pos, k4_vel, k4_mass, m_fuel_0,
                                   m_struct);

        sc.craft_pos.first +=
            (k1_pos.first + 2 * k2_pos.first + 2 * k3_pos.first + k4_pos.first) * dt / 6;
        sc.craft_pos.second +=
            (k1_pos.second + 2 * k2_pos.second + 2 * k3_pos.second + k4_pos.second) * dt / 6;
        sc.v_craft.first +=
            (k1_vel.first + 2 * k2_vel.first + 2 * k3_vel.first + k4_vel.first) * dt / 6;
        sc.v_craft.second +=
            (k1_vel.second + 2 * k2_vel.second + 2 * k3_vel.second + k4_vel.second) * dt / 6;
        sc.m_craft += (k1_mass + 2 * k2_mass + 2 * k3_mass + k4_mass) * dt / 6;
        sc.time += dt;
    }

   public:
    SpaceCraftSolver(vector<State>* planet_sputnik_trajectory, double planet_time_to_around,
                     double m_min_fuel, int num_steps_burn, int num_steps_coast)
        : planet_sputnik_trajectory(planet_sputnik_trajectory),
          planet_time_to_around(planet_time_to_around),
          r_apparat_orbit(Constants::R_PLANET + Constants::H),
          m_min_fuel(m_min_fuel),
          best_angle(0),
          found_solution(false),
          num_steps_burn(num_steps_burn),
          num_steps_coast(num_steps_coast) {
        cout << "SpaceCraftSolver initialized: orbit radius = " << r_apparat_orbit << " km" << endl;
    }

    bool try_apparat_angle(vector<SpaceCraftState>& trajectory, double angle, double m_fuel_0) {
        // Структурная масса: m_struct = α * m0 / (1-α), где m0 = m_fuel + m_struct + M0
        // => m_struct = α * (m_fuel_0 + M0) / (1 - α)
        double m_struct = Constants::PART_STRUCT_FRACTION * (m_fuel_0 + Constants::M0) /
                          (1.0 - Constants::PART_STRUCT_FRACTION);

        // Первая космическая скорость на орбите высотой H
        double v_circ = sqrt(Constants::G * Constants::M_PLANET / r_apparat_orbit);

        // === Начальные условия с поворотом на угол α ===
        // Позиция корабля в системе планеты:
        double rel_x = r_apparat_orbit * cos(angle);
        double rel_y = r_apparat_orbit * sin(angle);
        // Скорость (касательная, проградная) в системе планеты:
        double rel_vx = -v_circ * sin(angle);
        double rel_vy = v_circ * cos(angle);

        // Преобразование в инерциальную систему (планета на (R_SUN_PLANET, 0) в t=0)
        SpaceCraftState rocket;
        rocket.craft_pos = {Constants::R_SUN_PLANET + rel_x, 0.0 + rel_y};
        rocket.v_craft = {Constants::V_PLANET + rel_vx, 0.0 + rel_vy};
        rocket.m_craft = m_fuel_0 + m_struct + Constants::M0;  // стартовая масса
        rocket.time = 0.0;

        trajectory.clear();
        trajectory.push_back(rocket);

        // Параметры интегрирования
        double t_end = Constants::T_BURN * 1000.0;  // как в Python: t_fuel * 1000
        double dt_burn = Constants::T_BURN / num_steps_burn;
        double dt_coast = (t_end - Constants::T_BURN) / num_steps_coast;

        // === АКТИВНЫЙ УЧАСТОК: работа двигателя ===
        for (int i = 0; i < num_steps_burn; i++) {
            double t = rocket.time;
            // Круговые орбиты планеты и спутника
            double omega_p = Constants::V_PLANET / Constants::R_SUN_PLANET;
            double omega_s = (Constants::V_PLANET + Constants::V_SPUTNIK) /
                             (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK);

            pair<double, double> planet_pos = {Constants::R_SUN_PLANET * cos(omega_p * t),
                                               Constants::R_SUN_PLANET * sin(omega_p * t)};
            pair<double, double> sputnik_pos = {
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * cos(omega_s * t),
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * sin(omega_s * t)};

            rocket_runge_step(rocket, dt_burn, planet_pos, sputnik_pos, m_fuel_0, m_struct);
            if (i % 50 == 0) trajectory.push_back(rocket);
        }

        // === ОТДЕЛЕНИЕ ПОЛЕЗНОГО ГРУЗА ===
        // После выгорания топлива масса = конструкция + полезный груз
        rocket.m_craft = m_struct + Constants::M0;

        // === ПАССИВНЫЙ УЧАСТОК: только гравитация ===
        bool impact = false;
        for (int i = 0; i < num_steps_coast; i++) {
            if (rocket.time >= t_end) break;

            double t = rocket.time;
            double omega_p = Constants::V_PLANET / Constants::R_SUN_PLANET;
            double omega_s = (Constants::V_PLANET + Constants::V_SPUTNIK) /
                             (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK);

            pair<double, double> planet_pos = {Constants::R_SUN_PLANET * cos(omega_p * t),
                                               Constants::R_SUN_PLANET * sin(omega_p * t)};
            pair<double, double> sputnik_pos = {
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * cos(omega_s * t),
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * sin(omega_s * t)};

            rocket_runge_step(rocket, dt_coast, planet_pos, sputnik_pos, 0.0, 0.0);  // dm/dt = 0

            if (check_crash(rocket, sputnik_pos)) {
                impact = true;
                trajectory.push_back(rocket);
                break;
            }
            if (i % 1000 == 0) trajectory.push_back(rocket);
        }

        return impact;
    }

    void run_computations() {
        cout << "\n=== Transfer Optimization (Variant 11) ===" << endl;
        cout << "Searching: " << Constants::NUM_ANGLES << " angles × "
             << (Constants::M_FUEL_MAX - Constants::M_FUEL_MIN) / Constants::M_FUEL_STEP
             << " fuel values" << endl;

        double best_fuel = Constants::M_FUEL_MAX + 1;
        double best_angle_found = 0;
        bool found = false;
        vector<SpaceCraftState> best_traj;

        for (int i = 0; i < Constants::NUM_ANGLES; i++) {
            double angle = 2 * M_PI * i / Constants::NUM_ANGLES;

            // Линейный поиск по массе топлива
            for (double m_fuel = Constants::M_FUEL_MIN; m_fuel <= Constants::M_FUEL_MAX;
                 m_fuel += Constants::M_FUEL_STEP) {
                vector<SpaceCraftState> traj;
                if (try_apparat_angle(traj, angle, m_fuel)) {
                    if (m_fuel < best_fuel) {
                        best_fuel = m_fuel;
                        best_angle_found = angle;
                        best_traj = traj;
                        found = true;
                        cout << "  [New best] φ=" << angle * 180 / M_PI << "°, fuel=" << m_fuel
                             << " kg" << endl;
                    }
                    break;  // найдено для этого угла
                }
            }
            if (i % 36 == 0)
                cout << "  Processed " << i << "/" << Constants::NUM_ANGLES << " angles..." << endl;
        }

        // Вывод результатов
        cout << "\n=== RESULTS ===" << endl;
        if (found) {
            cout << "✓ Solution found!" << endl;
            cout << "Optimal phase angle: " << best_angle_found << " rad ("
                 << best_angle_found * 180 / M_PI << "°)" << endl;
            cout << "Minimum fuel mass: " << best_fuel << " kg" << endl;

            double m_struct = Constants::PART_STRUCT_FRACTION * (best_fuel + Constants::M0) /
                              (1.0 - Constants::PART_STRUCT_FRACTION);
            double m0 = best_fuel + m_struct + Constants::M0;
            cout << "Initial spacecraft mass: " << m0 << " kg" << endl;
            cout << "  - Payload: " << Constants::M0 << " kg" << endl;
            cout << "  - Structure: " << m_struct << " kg" << endl;
            cout << "  - Fuel: " << best_fuel << " kg" << endl;
        } else {
            cout << "✗ No solution found in search range." << endl;
            cout << "Try: expand [M_FUEL_MIN, M_FUEL_MAX], increase integration time, or refine "
                    "angles."
                 << endl;
        }
    }

    void run_computations_async() {
        cout << "\n=== Transfer Optimization (std::async) ===" << endl;
        vector<future<Result>> futures;
        mutex result_mutex;

        double global_best_fuel = Constants::M_FUEL_MAX + 1;
        double global_best_angle = 0;
        bool global_found = false;
        vector<SpaceCraftState> global_best_traj;

        for (int i = 0; i < Constants::NUM_ANGLES; i++) {
            futures.push_back(async(launch::async, [this, i, &result_mutex]() -> Result {
                double angle = 2 * M_PI * i / Constants::NUM_ANGLES;

                for (double m_fuel = Constants::M_FUEL_MIN; m_fuel <= Constants::M_FUEL_MAX;
                     m_fuel += Constants::M_FUEL_STEP) {
                    vector<SpaceCraftState> traj;
                    if (try_apparat_angle(traj, angle, m_fuel)) {
                        cout << "YES" << " " << m_fuel << endl;
                        best_trajectory = traj;
                        return {true, m_fuel, angle, traj};
                    }
                }
                return {false, 0, 0, {}};
            }));

            if (i % 36 == 0)
                cout << "  Processed " << i << "/" << Constants::NUM_ANGLES << " angles..." << endl;
        }

        // Сбор результатов
        for (auto& f : futures) {
            Result r = f.get();  // ожидание завершения
            if (r.found) {
                lock_guard<mutex> lock(result_mutex);
                if (r.fuel < global_best_fuel) {
                    global_best_fuel = r.fuel;
                    global_best_angle = r.angle;
                    global_best_traj = r.traj;
                    best_trajectory = r.traj;
                    global_found = true;
                    cout << "  New best: φ=" << r.angle * 180 / M_PI << "°, fuel=" << r.fuel
                         << " kg" << endl;
                }
            }
        }
    }

    void save_to_csv(const string& filename) const {
        ofstream ship_file(filename);
        if (!ship_file.is_open()) {
            cerr << "Error opening spacecraft_trajectory.csv" << endl;
            return;
        }

        ship_file << "x_ship,y_ship,x_planet,y_planet,x_sputnik,y_sputnik," << endl;
        ship_file << scientific << setprecision(10);

        for (const auto& sc : best_trajectory) {
            double t = sc.time;

            double omega_p = Constants::V_PLANET / Constants::R_SUN_PLANET;
            double omega_s = (Constants::V_PLANET + Constants::V_SPUTNIK) /
                             (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK);

            double x_planet = Constants::R_SUN_PLANET * cos(omega_p * t);
            double y_planet = Constants::R_SUN_PLANET * sin(omega_p * t);
            double x_sputnik =
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * cos(omega_s * t);
            double y_sputnik =
                (Constants::R_SUN_PLANET + Constants::R_PLANET_SPUTNIK) * sin(omega_s * t);

            double vx_planet = -Constants::V_PLANET * sin(omega_p * t);
            double vy_planet = Constants::V_PLANET * cos(omega_p * t);
            double vx_sputnik = -(Constants::V_PLANET + Constants::V_SPUTNIK) * sin(omega_s * t);
            double vy_sputnik = (Constants::V_PLANET + Constants::V_SPUTNIK) * cos(omega_s * t);

            ship_file << t << "," << sc.craft_pos.first << "," << sc.craft_pos.second << ","
                      << sc.v_craft.first << "," << sc.v_craft.second << "," << sc.m_craft << ","
                      << x_planet << "," << y_planet << "," << x_sputnik << "," << y_sputnik << ","
                      << vx_planet << "," << vy_planet << "," << vx_sputnik << "," << vy_sputnik
                      << endl;
        }
        ship_file.close();
    }
};

int main() {
    cout << "sun, planet, sputnik, part 1" << endl;

    auto start = std::chrono::steady_clock::now();
    bool add_spaace_apparat = false;
    int num_steps = 1000000;
    unique_ptr<Solver> task1 = make_unique<Solver>(num_steps);
    task1->run_computations();
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "time of 1 part computations = " << duration.count() << endl;

    task1->saveToCSV("../labs/lr1/trajectory_full.csv");
    cout << "haha" << endl;
    system("python ../labs/lr1/main.py");

    cout << "space apparat part 2" << endl;

    int num_steps_burn = 1000;
    int num_steps_coast = 10000;
    double m_fuel_init = 10.0;  // начальное значение для поиска

    start = std::chrono::steady_clock::now();
    auto task2 =
        make_unique<SpaceCraftSolver>(&task1->get_trajectory(), task1->get_time_to_around(),
                                      m_fuel_init, num_steps_burn, num_steps_coast);

    task2->run_computations_async();
    end = std::chrono::steady_clock::now();
    duration = end - start;

    task2->save_to_csv("../labs/lr1/trajectory_space_craft.csv");
    system("python ../labs/lr1/part.py");

    cout << "time of 2 part computations = " << duration.count() << endl;

    // {
    //     cout << "space apparat part 2.2" << endl;

    //     int num_steps_burn = 500;
    //     int num_steps_coast = 5000;
    //     double m_fuel_init = 320.0;  // начальное значение для поиска

    //     start = std::chrono::steady_clock::now();
    //     auto task2 =
    //         make_unique<SpaceCraftSolver>(&task1->get_trajectory(), task1->get_time_to_around(),
    //                                       m_fuel_init, num_steps_burn, num_steps_coast);

    //     task2->run_computations();
    //     end = std::chrono::steady_clock::now();

    //     duration = end - start;
    //     cout << "time of 2 part computations = " << duration.count() << endl;
    // }
    return 0;
}