#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

namespace Consts {
const sf::Color GRID_COLOR(100, 150, 255);
const sf::Color BACKGROUND_COLOR(0, 0, 0);

constexpr float DEFAULT_UPDATE_INTERVAL = 0.15f;
constexpr unsigned FRAME_LIMIT = 120;
constexpr float SPEED_STEP = 0.02f;
constexpr float MIN_SPEED = 0.5f;
constexpr float MAX_SPEED = 0.02f;
}  // namespace Consts

namespace LightConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 150;
constexpr unsigned DEFAULT_CELL_SIZE = 4;
constexpr float ACCUMULATED_EXPOSURE = 0.0005f;
constexpr float WAVE_FREQUENCY = 0.8f;
constexpr float WAVE_AMPLITUDE = 10.0f;
constexpr float PIXEL_MASS_OUT_FIGURE = 1.0f;
constexpr float PIXEL_MASS_FIGURE = 0.9f;
}  // namespace LightConsts

namespace OrganismsConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 256;
constexpr unsigned DEFAULT_CELL_SIZE = 4;
constexpr unsigned L = 15;
constexpr unsigned T = 3;
constexpr float P_MAX = 10.0f;
constexpr float P1 = 35.0f;
constexpr unsigned A = 30;
constexpr float R = 1.0f;
constexpr float DELTA_P = 5.0f;
constexpr float DELTA_E = 2.0f;
constexpr float DELTA_R = 3.0f;

const sf::Color ORGANISM_COLOR_HIGH(255, 0, 0);
const sf::Color ORGANISM_COLOR_MEDIUM(255, 200, 0);
const sf::Color ORGANISM_COLOR_LOW(255, 100, 0);

const sf::Color NUTRIENT_HIGH(0, 255, 0);
const sf::Color NUTRIENT_MEDIUM(0, 200, 0);
const sf::Color NUTRIENT_LOW(0, 100, 0);
}  // namespace OrganismsConsts

namespace NeuralNetworkConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 256;
constexpr unsigned DEFAULT_CELL_SIZE = 4;

const sf::Color CELL_COLOR_ACTIVE(255, 0, 0);    // red
const sf::Color CELL_COLOR_REST(0, 255, 0);      // green
const sf::Color CELL_COLOR_RECOVERY(0, 0, 255);  // blue

constexpr float A = 0.3f;
constexpr float P = 3.0f;
constexpr int T = 5;
constexpr int B = 8;
constexpr int SOURCE_PERIOD = 15;
}  // namespace NeuralNetworkConsts

namespace LiveGameConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 32;
constexpr unsigned DEFAULT_CELL_SIZE = 20;

const sf::Color CELL_COLOR(255, 0, 0);

inline const vector<tuple<string, string, unsigned>> CONWAY_PATTERNS = {
    {"random", "Случайный", 0},
    {"block", "Блок (статичный)", 1},
    {"beehive", "Улей (статичный)", 2},
    {"loaf", "Буханка (статичный)", 3},
    {"blinker", "Мигалка (период 2)", 4},
    {"toad", "Жаба (период 2)", 5},
    {"glider", "Планер (движущийся)", 6},
    {"pulsar", "Пульсар (период 3)", 7},
    {"gosper_glider_gun", "Пушка Госпера (генератор планеров)", 8}};

inline const vector<tuple<string, string, unsigned>> B2S012_PATTERNS = {
    {"random", "Случайный", 0},
    {"background", "1. статичный", 1},
    {"background_with_vertical", "2. Вертикаль (статичный)", 2},
    {"background_with_crest", "3. статичный", 3},
    {"background_with_eye", "5. цикличный", 4},
    {"background_with_eight", "5. цикличный", 5},
    {"background_with_nail", "6. цикличный", 6},
    {"background_with_gates", "7. цикличный", 7},
    {"background_with_cyclic_last", "8. цикличный", 8}};
}  // namespace LiveGameConsts