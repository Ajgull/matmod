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

class Organism : public BaseCellAutomaton {
   private:
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    sf::Color cell_color;

    void initGrid() override;
    void updateGrid(bool force_update) override;
    void drawCells(sf::RenderWindow& window) override;

   public:
    Organism(unsigned num_cells = OrganismsConsts::DEFAULT_NUM_CELLS,
             unsigned cell_size = OrganismsConsts::DEFAULT_CELL_SIZE);
    ~Organism() override;

    void reset() override;
};