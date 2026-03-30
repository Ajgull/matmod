// light.h
#pragma once

#include <stdint.h>
#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "base.h"
#include "consts.h"

using namespace std;

class Light : public BaseCellAutomaton {
   private:
    vector<vector<vector<float>>> wave_height;        // [x][y][rgb]
    vector<vector<vector<float>>> wave_velocity;      // [x][y][rgb]
    vector<vector<vector<float>>> accumulated_light;  // [x][y][rgb]
    vector<vector<float>> pixel_mass;                 // [x][y]

    unsigned int current_frame;
    float accumulated_exposure;
    vector<int> glass_colors;
    vector<float> color_shift;

    void initGrid() override;
    void updateCellColors() override;
    void updateGrid(bool force_update) override;
    void handleKeyPress(const sf::Event::KeyPressed& key_event) override;

    void generateWaves();       // Генерация волн из источника
    void updatePhysics();       // Обновление физики волн
    void updateAccumulation();  // Накопление света

   public:
    Light(unsigned num_cells = LightConsts::DEFAULT_NUM_CELLS,
          unsigned cell_size = LightConsts::DEFAULT_CELL_SIZE);
    ~Light() override;
    void reset() override;

    void setGridColor(const sf::Color& color) { BaseCellAutomaton::setGridColor(color); }
    void setBackgroundColor(const sf::Color& color) {
        BaseCellAutomaton::setBackgroundColor(color);
    }
};