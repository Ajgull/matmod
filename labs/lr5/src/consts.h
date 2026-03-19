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

namespace NeuralNetworkConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 256;
constexpr unsigned DEFAULT_CELL_SIZE = 3;

const sf::Color CELL_COLOR_ACTIVE(255, 0, 0);      // red
const sf::Color CELL_COLOR_REST(0, 255, 0);        // green
const sf::Color CELL_COLOR_RECOVERY(255, 255, 0);  // blue

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

const vector<tuple<string, string, unsigned>> PATTERNS = {
    {"random", "Случайный", 0},
    {"block", "Блок статичный", 1},
    {"beehive", "Улей статичный", 2},
    {"loaf", "Буханка статичный", 3},
    {"blinker", "Мигалка цикличный", 4},
    {"toad", "Жаба", 5},
    {"glider", "Планер цикличный", 6},
    {"pulsar", "Пульсар цикличный не сразу", 7},
    {"gosper_glider_gun", "Пушка Госпера цикличный не сразу на большом поле", 8}};

}  // namespace LiveGameConsts