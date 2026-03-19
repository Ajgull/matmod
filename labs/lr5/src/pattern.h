#pragma once

#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

class Pattern {
   private:
    unsigned num_cells;
    mt19937 rng;
    void setCell(vector<vector<bool>>& grid, unsigned x, unsigned y, bool state = true);

   public:
    Pattern(unsigned num_cells);
    void applyPattern(const string& pattern_name, vector<vector<bool>>& grid);
    void randomizeGrid(vector<vector<bool>>& grid, int percentage = 20);
    void clearGrid(vector<vector<bool>>& grid);
    void addBlock(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBeehive(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addLoaf(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBlinker(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addToad(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addGlider(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addPulsar(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addGosperGliderGun(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
};
