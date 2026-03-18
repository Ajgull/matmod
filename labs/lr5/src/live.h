#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "pattern.h"

using namespace std;

class GameLive {
   private:
    unsigned width, height, cell_size, num_cells;
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    bool is_running;
    float update_interval;
    chrono::steady_clock::time_point last_update;
    string current_pattern;
    unique_ptr<Pattern> pattern_manager;

    sf::Color grid_color;
    sf::Color cell_color;
    sf::Color background_color;

    void drawGrid(sf::RenderWindow& window);
    void drawCells(sf::RenderWindow& window);
    int countNeighbors(unsigned x, unsigned y);
    void updateGrid(bool force_update);
    void clearGrid();
    void toggleCell(unsigned x, unsigned y);
    void setCell(unsigned x, unsigned y, bool state = true);

   public:
    GameLive(unsigned num_cells, unsigned cell_size, const string& pattern = "random");
    ~GameLive();
    void run();
    void loadPattern(const string& pattern_name);
    void randomizeGrid();
    void setRunning(bool running) { is_running = running; }
    bool isRunning() const { return is_running; }
    void increaseSpeed();
    void decreaseSpeed();
};
