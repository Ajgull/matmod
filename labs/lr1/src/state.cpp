#include "state.h"

State::State() {
    sun_pos = {0.0, 0.0};
    planet_pos = {0.0, 0.0};
    sputnik_pos = {0.0, 0.0};
    time = 0;
}

State::State(pair<double, double>& sun_pos, pair<double, double>& planet_pos,
             pair<double, double>& sputnik_pos, pair<double, double>& v_planet,
             pair<double, double>& v_sputnik)
    : sun_pos(sun_pos),
      planet_pos(planet_pos),
      sputnik_pos(sputnik_pos),
      v_planet(v_planet),
      v_sputnik(v_sputnik) {
    time = 0;
}

State::State(const State& other) {
    sun_pos = other.sun_pos;
    planet_pos = other.planet_pos;
    sputnik_pos = other.sputnik_pos;
    v_planet = other.v_planet;
    v_sputnik = other.v_sputnik;
    time = other.time;
}

SpaceCraftState::SpaceCraftState()
    : craft_pos({0.0, 0.0}), v_craft({0.0, 0.0}), mass_space_craft(0.0), time(0.0) {}

SpaceCraftState::SpaceCraftState(pair<double, double> pos, pair<double, double> vel, double mass,
                                 double t)
    : craft_pos(pos), v_craft(vel), mass_space_craft(mass), time(t) {}