#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Constants {
constexpr double M_SUN = 2e30;        // kg
constexpr double M_PLANET = 1.9e27;   // kg
constexpr double M_SPUTNIK = 4.8e22;  // kg

constexpr double R_PLANET = 71500.0;        // km
constexpr double R_SUN_PLANET = 780e6;      // km
constexpr double R_SPUTNIK = 1561.0;        // km
constexpr double R_PLANET_SPUTNIK = 671e3;  // km

constexpr double V_PLANET = 13.0;   // km/s
constexpr double V_SPUTNIK = 13.7;  // km/s

constexpr double G = 6.67430e-11 / 1e9;

constexpr double H = 2000.0;                    // km
constexpr double T_BURN = 5100.0;               // s
constexpr double M0 = 40.0;                     // kg
constexpr double V_EXP = 3.375;                 // km/s
constexpr double PART_STRUCT_FRACTION = 0.001;  // mass fraction
}  // namespace Constants