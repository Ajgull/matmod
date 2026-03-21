#pragma once

#include <iostream>
#include <utility>

using namespace std;

struct State {
    pair<double, double> sun_pos, planet_pos, sputnik_pos;
    pair<double, double> v_planet, v_sputnik;
    double time;

    State();
    State(pair<double, double>& sun_pos, pair<double, double>& planet_pos,
          pair<double, double>& sputnik_pos, pair<double, double>& v_planet,
          pair<double, double>& v_sputnik);
    State(const State& other);
};

struct SpaceCraftState {
    pair<double, double> craft_pos, v_craft;
    double mass_space_craft;
    double time;

    SpaceCraftState();
    SpaceCraftState(pair<double, double> pos, pair<double, double> vel, double mass, double t);
};
