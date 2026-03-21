#pragma once

#include <string>
#include <vector>

#include "state.h"

using namespace std;

class SpaceCraftSolver {
   private:
    vector<State>* base_traj;
    double planet_period;
    double orbit_radius;
    int steps_burn, steps_coast;
    double dt_burn, dt_coast;
    double best_mass, best_angle, best_length;

    double norm(const pair<double, double>& v);
    pair<double, double> gravity_acc(const SpaceCraftState& craft, double t);
    State get_state_at_time(double t) const;
    SpaceCraftState compute_derivatives(const SpaceCraftState& craft, double t, bool active,
                                        double fuel_rate);
    SpaceCraftState runge_step(const SpaceCraftState& sc, double t, double dt, bool active,
                               double fuel_rate);
    SpaceCraftState init_space_craft(double angle_deg, double mass);
    double try_flight(double angle, double mass, double total_time);
    void find_best_fuel_angle_omp(double total_time);
    void find_best_fuel_angle(double total_time);

   public:
    SpaceCraftSolver(vector<State>* traj, double period, int n_burn, int n_coast);

    void run_computations(bool type_of_finding);
    void save_to_csv(double total_time);
};
