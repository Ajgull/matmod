#include "pattern.h"

#include <iostream>
#include <random>

Pattern::Pattern(unsigned num_cells) : num_cells(num_cells) {
    random_device rd;
    rng = mt19937(rd());
}

void Pattern::setCell(vector<vector<bool>>& grid, unsigned x, unsigned y, bool state) {
    if (x < num_cells && y < num_cells) {
        grid[y][x] = state;
    }
}

void Pattern::randomizeGrid(vector<vector<bool>>& grid, int percentage) {
    uniform_int_distribution<> dis(0, 99);

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            grid[y][x] = (dis(rng) < percentage);
        }
    }
}

void Pattern::clearGrid(vector<vector<bool>>& grid) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            grid[y][x] = false;
        }
    }
}

void Pattern::applyPattern(const string& pattern_name, vector<vector<bool>>& grid) {
    clearGrid(grid);

    if (pattern_name == "random" || pattern_name == "rand") {
        randomizeGrid(grid);
        return;
    }

    unsigned center_x = num_cells / 2;
    unsigned center_y = num_cells / 2;

    if (pattern_name == "block") {
        addBlock(grid, center_x, center_y);
    } else if (pattern_name == "beehive") {
        addBeehive(grid, center_x, center_y);
    } else if (pattern_name == "loaf") {
        addLoaf(grid, center_x, center_y);
    } else if (pattern_name == "blinker") {
        addBlinker(grid, center_x, center_y);
    } else if (pattern_name == "toad") {
        addToad(grid, center_x, center_y);
    } else if (pattern_name == "glider") {
        addGlider(grid, center_x, center_y);
    } else if (pattern_name == "pulsar") {
        addPulsar(grid, center_x, center_y);
    } else if (pattern_name == "gosper_glider_gun") {
        addGosperGliderGun(grid, center_x, center_y);
    } else {
        randomizeGrid(grid);
    }
}

void Pattern::addBlock(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx, cy);
    setCell(grid, cx + 1, cy);
    setCell(grid, cx, cy + 1);
    setCell(grid, cx + 1, cy + 1);
}

void Pattern::addBeehive(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx, cy);
    setCell(grid, cx + 1, cy - 1);
    setCell(grid, cx + 1, cy + 1);
    setCell(grid, cx + 2, cy - 1);
    setCell(grid, cx + 2, cy + 1);
    setCell(grid, cx + 3, cy);
}

void Pattern::addLoaf(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx, cy);
    setCell(grid, cx + 1, cy - 1);
    setCell(grid, cx + 1, cy + 1);
    setCell(grid, cx + 2, cy - 2);
    setCell(grid, cx + 2, cy + 1);
    setCell(grid, cx + 3, cy - 1);
    setCell(grid, cx + 3, cy);
}

void Pattern::addBlinker(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx, cy - 1);
    setCell(grid, cx, cy);
    setCell(grid, cx, cy + 1);
}

void Pattern::addToad(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx - 1, cy);
    setCell(grid, cx, cy - 1);
    setCell(grid, cx, cy);
    setCell(grid, cx, cy + 1);
    setCell(grid, cx + 1, cy - 1);
    setCell(grid, cx + 1, cy);
    setCell(grid, cx + 1, cy + 1);
    setCell(grid, cx + 2, cy);
}

void Pattern::addGlider(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    setCell(grid, cx + 1, cy);
    setCell(grid, cx + 2, cy + 1);
    setCell(grid, cx, cy + 2);
    setCell(grid, cx + 1, cy + 2);
    setCell(grid, cx + 2, cy + 2);
}

void Pattern::addPulsar(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int px = cx - 7;
    int py = cy - 7;

    for (int i = 0; i < 3; i++) {
        setCell(grid, px + 2, py + 4 + i);
        setCell(grid, px + 7, py + 4 + i);
        setCell(grid, px + 9, py + 4 + i);
        setCell(grid, px + 14, py + 4 + i);
    }

    for (int i = 0; i < 3; i++) {
        setCell(grid, px + 4 + i, py + 2);
        setCell(grid, px + 4 + i, py + 7);
        setCell(grid, px + 4 + i, py + 9);
        setCell(grid, px + 4 + i, py + 14);
    }
}

void Pattern::addGosperGliderGun(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int gx = cx - 18;
    int gy = cy - 9;

    setCell(grid, gx + 24, gy + 0);
    setCell(grid, gx + 22, gy + 1);
    setCell(grid, gx + 24, gy + 1);
    setCell(grid, gx + 12, gy + 2);
    setCell(grid, gx + 13, gy + 2);
    setCell(grid, gx + 20, gy + 2);
    setCell(grid, gx + 21, gy + 2);
    setCell(grid, gx + 34, gy + 2);
    setCell(grid, gx + 35, gy + 2);
    setCell(grid, gx + 11, gy + 3);
    setCell(grid, gx + 15, gy + 3);
    setCell(grid, gx + 20, gy + 3);
    setCell(grid, gx + 21, gy + 3);
    setCell(grid, gx + 34, gy + 3);
    setCell(grid, gx + 35, gy + 3);
    setCell(grid, gx + 0, gy + 4);
    setCell(grid, gx + 1, gy + 4);
    setCell(grid, gx + 10, gy + 4);
    setCell(grid, gx + 16, gy + 4);
    setCell(grid, gx + 20, gy + 4);
    setCell(grid, gx + 21, gy + 4);
    setCell(grid, gx + 0, gy + 5);
    setCell(grid, gx + 1, gy + 5);
    setCell(grid, gx + 10, gy + 5);
    setCell(grid, gx + 14, gy + 5);
    setCell(grid, gx + 16, gy + 5);
    setCell(grid, gx + 17, gy + 5);
    setCell(grid, gx + 22, gy + 5);
    setCell(grid, gx + 24, gy + 5);
    setCell(grid, gx + 10, gy + 6);
    setCell(grid, gx + 16, gy + 6);
    setCell(grid, gx + 24, gy + 6);
    setCell(grid, gx + 11, gy + 7);
    setCell(grid, gx + 15, gy + 7);
    setCell(grid, gx + 12, gy + 8);
    setCell(grid, gx + 13, gy + 8);
}