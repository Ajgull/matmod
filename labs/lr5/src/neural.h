#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "base.h"
#include "consts.h"

using namespace std;

enum class CellState { REST, ACTIVE, RECOVERY };

class NeuralNetwork : public BaseCellAutomaton {
   private:
    vector<vector<CellState>> grid;
    vector<vector<CellState>> next_grid;
    vector<vector<float>> activator_level;
    vector<vector<int>> state_timer;
    unsigned int current_tick;

    sf::Color cell_color_active;
    sf::Color cell_color_rest;
    sf::Color cell_color_recovery;

    void initGrid() override;
    void updateGrid(bool force_update) override;
    void drawCells(sf::RenderWindow& window) override;

    void initFlatFront();
    void initGenerator();

   public:
    NeuralNetwork(unsigned num_cells = NeuralNetworkConsts::DEFAULT_NUM_CELLS,
                  unsigned cell_size = NeuralNetworkConsts::DEFAULT_CELL_SIZE);
    ~NeuralNetwork() override;
    void handleKeyPress(const sf::Event::KeyPressed& key_event) override;
    void reset() override;
};