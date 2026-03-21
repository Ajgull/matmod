#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "base.h"
#include "consts.h"
#include "pattern.h"

using namespace std;

class GameLive : public BaseCellAutomaton {
   private:
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    string current_pattern;
    unique_ptr<Pattern> pattern_manager;
    sf::Color cell_color;

    void initGrid() override;
    void updateGrid(bool force_update) override;
    void drawCells(sf::RenderWindow& window) override;
    void handleKeyPress(const sf::Event::KeyPressed& key_event) override;

    int countNeighbors(unsigned x, unsigned y);
    void clearGrid();
    void toggleCell(unsigned x, unsigned y);
    void setCell(unsigned x, unsigned y, bool state = true);

   public:
    GameLive(unsigned num_cells = LiveGameConsts::DEFAULT_NUM_CELLS,
             unsigned cell_size = LiveGameConsts::DEFAULT_CELL_SIZE,
             const string& pattern = "random");
    ~GameLive() override;

    void run() override;
    void loadPattern(const string& pattern_name);
    void randomizeGrid();
    void reset() override;
};