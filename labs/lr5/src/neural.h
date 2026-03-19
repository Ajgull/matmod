// #pragma once

// #include <SFML/Graphics.hpp>
// #include <chrono>
// #include <iostream>
// #include <memory>
// #include <string>
// #include <vector>

// #include "consts.h"

// using namespace std;

// enum class CellState { REST, ACTIVE, RECOVERY };

// class NeuralNetwork {
//    private:
//     unsigned width, height, cell_size, num_cells;
//     vector<vector<CellState>> grid;
//     vector<vector<CellState>> next_grid;
//     vector<vector<int>> activation_timer;
//     vector<vector<int>> recovery_timer;
//     bool is_running;
//     float update_interval;
//     chrono::steady_clock::time_point last_update;
//     unsigned int current_tick;
//     bool rendering_initialized;
//     vector<vector<sf::RectangleShape>> cell_shapes;

//     sf::Color grid_color;
//     sf::Color cell_color_active;
//     sf::Color cell_color_rest;
//     sf::Color cell_color_recovery;
//     sf::Color background_color;
//     sf::VertexArray grid_lines;

//     void initRendering();

//     void drawGrid(sf::RenderWindow& window);
//     void drawCells(sf::RenderWindow& window);
//     void updateGrid(bool force_update);
//     void initFlatFront();
//     void updateSource();
//     int countNeighbors(unsigned x, unsigned y, CellState state);
//     float calculateActivatorLevel(unsigned x, unsigned y);

//    public:
//     NeuralNetwork(unsigned num_cells = NeuralNetworkConsts::DEFAULT_NUM_CELLS,
//                   unsigned cell_size = NeuralNetworkConsts::DEFAULT_CELL_SIZE);

//     ~NeuralNetwork();
//     void run();
//     void increaseSpeed();
//     void decreaseSpeed();
// };

#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "consts.h"

using namespace std;

enum class CellState { REST, ACTIVE, RECOVERY };

class NeuralNetwork {
   private:
    unsigned width, height, cell_size, num_cells;
    vector<vector<CellState>> grid;
    vector<vector<CellState>> next_grid;
    vector<vector<float>> activator_level;
    vector<vector<int>> state_timer;
    bool is_running;
    float update_interval;
    chrono::steady_clock::time_point last_update;
    unsigned int current_tick;

    sf::Color grid_color;
    sf::Color cell_color_active;
    sf::Color cell_color_rest;
    sf::Color cell_color_recovery;
    sf::Color background_color;

    bool rendering_initialized = false;
    sf::VertexArray grid_lines;
    vector<vector<sf::RectangleShape>> cell_shapes;

    void drawGrid(sf::RenderWindow& window);
    void drawCells(sf::RenderWindow& window);
    void initRendering();
    void updateGrid(bool force_update);
    void initFlatFront();
    void initGenerator();

   public:
    NeuralNetwork(unsigned num_cells = NeuralNetworkConsts::DEFAULT_NUM_CELLS,
                  unsigned cell_size = NeuralNetworkConsts::DEFAULT_CELL_SIZE);

    ~NeuralNetwork();
    void run();
    void increaseSpeed();
    void decreaseSpeed();
};