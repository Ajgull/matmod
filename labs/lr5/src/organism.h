#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "consts.h"

using namespace std;

class Organism {
   private:
    unsigned width, height, cell_size, num_cells;
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    bool is_running;
    float update_interval;
    chrono::steady_clock::time_point last_update;
    bool rendering_initialized = false;

    sf::Color grid_color;
    sf::Color cell_color;
    sf::Color background_color;

    sf::VertexArray grid_lines;
    vector<vector<sf::RectangleShape>> cell_shapes;

    void drawGrid(sf::RenderWindow& window);
    void drawCells(sf::RenderWindow& window);
    void initRendering();
    void updateGrid(bool force_update);

   public:
    Organism(unsigned num_cells = OrganismsConsts::DEFAULT_NUM_CELLS,
             unsigned cell_size = OrganismsConsts::DEFAULT_CELL_SIZE);
    ~Organism();
    void run();
    void increaseSpeed();
    void decreaseSpeed();
};