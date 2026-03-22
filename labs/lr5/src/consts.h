#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

namespace Consts {
const sf::Color GRID_COLOR(0, 0, 250);
const sf::Color BACKGROUND_COLOR(25, 25, 25);

constexpr float DEFAULT_UPDATE_INTERVAL = 0.15f;
constexpr unsigned FRAME_LIMIT = 60;
constexpr float SPEED_STEP = 0.02f;
constexpr float MIN_SPEED = 0.5f;
constexpr float MAX_SPEED = 0.02f;
}  // namespace Consts

namespace OrganismsConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 256;
constexpr unsigned DEFAULT_CELL_SIZE = 4;
constexpr unsigned L = 15;      // max len of life
constexpr unsigned T = 3;       // mature cell
constexpr float P_MAX = 10.0f;  // max energy
constexpr float P1 = 1.0f;      // increasing step energy
constexpr float A = 0.3f;
constexpr float DELTA_P = 5.0f;  // energy for organism per takt
constexpr float DELTA_E = 2.0f;  // energe for organism to live per takt
constexpr float DELTA_R = 3.0f;  // energy to increasing organisms

constexpr float INITIAL_POPULATION = 0.3f;

const sf::Color CELL_COLOR_HIGH(255, 0, 0);
const sf::Color CELL_COLOR_LOW(255, 0, 50);
const sf::Color CELL_COLOR(0, 50, 0);
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
    {"background_with_vertical", "Вертикаль (статичный)", 1},
    {"background_with_eye", "2. цикличный", 2},
    {"background_with_eight", "3. цикличный", 3},
    {"background_with_nail", "4. цикличный", 4},
    {"background_with_gates", "5. цикличный", 5}};
}  // namespace LiveGameConsts