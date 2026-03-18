#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace LiveGameConsts {
constexpr unsigned DEFAULT_NUM_CELLS = 32;
constexpr unsigned DEFAULT_CELL_SIZE = 20;

const sf::Color GRID_COLOR(50, 50, 50);
const sf::Color CELL_COLOR(255, 0, 0);
const sf::Color BACKGROUND_COLOR(20, 20, 25);

constexpr float DEFAULT_UPDATE_INTERVAL = 0.15f;
constexpr unsigned FRAME_LIMIT = 60;
constexpr float SPEED_STEP = 0.02f;
constexpr float MIN_SPEED = 0.5f;
constexpr float MAX_SPEED = 0.02f;

const std::vector<std::tuple<std::string, std::string, unsigned>> PATTERNS = {
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