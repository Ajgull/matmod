#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Constants {
constexpr double M_SUN = 2e30;        // кг
constexpr double M_PLANET = 1.9e27;   // кг
constexpr double M_SPUTNIK = 4.8e22;  // кг

constexpr double R_PLANET = 71500.0;        // км
constexpr double R_SUN_PLANET = 780e6;      // км
constexpr double R_SPUTNIK = 1561.0;        // км
constexpr double R_PLANET_SPUTNIK = 671e3;  // км

constexpr double V_PLANET = 13.0;   // км/с
constexpr double V_SPUTNIK = 13.7;  // км/с

constexpr double G = 6.67430e-11 / 1e9;

constexpr double H = 2000.0;                    // км
constexpr double T_BURN = 5100.0;               // с
constexpr double M0 = 40.0;                     // кг
constexpr double V_EXP = 3.375;                 // км/с
constexpr double PART_STRUCT_FRACTION = 0.005;  // доля конструкции от стартовой массы
}  // namespace Constants