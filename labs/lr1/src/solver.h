#pragma once
#include <string>
#include <vector>

#include "state.h"

using namespace std;

class Solver {
   private:
    pair<double, double> sun_initial_pos, planet_initial_pos, sputnik_initial_pos;
    pair<double, double> v_planet, v_sputnik;
    double time_to_around;
    int num_steps;
    double step;
    vector<State> trajectory;

   public:
    Solver(int num_steps, int num_years);

    vector<State>& get_trajectory();
    double get_time_to_around();

    double dist(double x, double y);
    void compute_derivatives(const State& s, pair<double, double>& d_planet_pos,
                             pair<double, double>& d_sputnik_pos, pair<double, double>& d_v_planet,
                             pair<double, double>& d_v_sputnik);
    void runge_step(State& s, double dt);
    void run_computations();
    void save_to_csv(const string& filename) const;
};
