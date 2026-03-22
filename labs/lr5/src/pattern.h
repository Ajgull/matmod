#pragma once

#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

enum class GameRule;

class Pattern {
   private:
    unsigned num_cells;
    mt19937 rng;
    void setCell(vector<vector<bool>>& grid, unsigned x, unsigned y, bool state = true);
    void fillStripedBackground(vector<vector<bool>>& grid);

   public:
    Pattern(unsigned num_cells);
    void applyPattern(const string& pattern_name, vector<vector<bool>>& grid, GameRule rule);
    void randomizeGrid(vector<vector<bool>>& grid, int percentage = 20);
    void clearGrid(vector<vector<bool>>& grid);

    // ========== Паттерны для Conway B3/S23 ==========
    void addBlock(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBeehive(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addLoaf(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBlinker(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addToad(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addGlider(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addPulsar(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addGosperGliderGun(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);

    // ========== Базовые компоненты для B2/S012 ==========
    void addVerticalElement(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addEyePattern(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addEightPattern(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addNailPattern(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addGatesPattern(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);

    // ========== Комбинированные паттерны на полосатом фоне ==========
    void addBackgroundWithVertical(vector<vector<bool>>& grid, unsigned center_x,
                                   unsigned center_y);
    void addBackgroundWithEye(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBackgroundWithEight(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBackgroundWithNail(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
    void addBackgroundWithGates(vector<vector<bool>>& grid, unsigned center_x, unsigned center_y);
};