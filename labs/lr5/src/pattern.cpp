#include "pattern.h"

enum class GameRule { B3S23, B2S012 };

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

void Pattern::fillStripedBackground(vector<vector<bool>>& grid) {
    for (unsigned r = 0; r < num_cells; r += 2) {
        for (unsigned c = 0; c < num_cells; c++) {
            setCell(grid, c, r, true);
        }
    }

    if (num_cells % 2 == 0) {
        for (unsigned c = 0; c < num_cells; c++) {
            setCell(grid, c, num_cells - 2, false);
        }
        unsigned last_filled_row = num_cells - 2;
        setCell(grid, 1, num_cells - 2);
        setCell(grid, num_cells - 1, num_cells - 2);
    }
}

void Pattern::applyPattern(const string& pattern_name, vector<vector<bool>>& grid, GameRule rule) {
    clearGrid(grid);

    if (pattern_name == "random" || pattern_name == "rand") {
        randomizeGrid(grid);
        return;
    }

    unsigned center_x = num_cells / 2;
    unsigned center_y = num_cells / 2;

    if (rule != GameRule::B3S23) {
        if (pattern_name == "striped_background") {
            fillStripedBackground(grid);
            return;
        } else if (pattern_name == "vertical_element") {
            addVerticalElement(grid, center_x, center_y);
            return;
        } else if (pattern_name == "background_with_vertical") {
            addBackgroundWithVertical(grid, center_x, center_y);
            return;
        } else if (pattern_name == "background_with_eye") {
            addBackgroundWithEye(grid, center_x, center_y);
            return;
        } else if (pattern_name == "background_with_eight") {
            addBackgroundWithEight(grid, center_x, center_y);
            return;
        } else if (pattern_name == "background_with_nail") {
            addBackgroundWithNail(grid, center_x, center_y);
            return;
        } else if (pattern_name == "background_with_gates") {
            addBackgroundWithGates(grid, center_x, center_y);
            return;
        }
    }

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

void Pattern::addVerticalElement(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int offset_x = static_cast<int>(cx) - 12;
    int offset_y = static_cast<int>(cy) - 12;

    setCell(grid, offset_x + 10, offset_y + 10, false);
    setCell(grid, offset_x + 10, offset_y + 12, false);
    setCell(grid, offset_x + 11, offset_y + 11, true);
    setCell(grid, offset_x + 12, offset_y + 10, false);
    setCell(grid, offset_x + 12, offset_y + 12, false);
}

void Pattern::addEyePattern(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int offset_x = static_cast<int>(cx) - 12;
    int offset_y = static_cast<int>(cy) - 12;

    setCell(grid, offset_x + 5, offset_y + 4, false);
    setCell(grid, offset_x + 6, offset_y + 4, false);
    setCell(grid, offset_x + 5, offset_y + 6, false);
    setCell(grid, offset_x + 6, offset_y + 6, false);
}

void Pattern::addEightPattern(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int offset_x = static_cast<int>(cx) - 12;
    int offset_y = static_cast<int>(cy) - 12;

    setCell(grid, offset_x + 20, offset_y + 4, false);
    setCell(grid, offset_x + 21, offset_y + 4, false);
    setCell(grid, offset_x + 20, offset_y + 6, false);
    setCell(grid, offset_x + 21, offset_y + 6, false);
    setCell(grid, offset_x + 20, offset_y + 8, false);
    setCell(grid, offset_x + 21, offset_y + 8, false);
    setCell(grid, offset_x + 20, offset_y + 10, false);
    setCell(grid, offset_x + 21, offset_y + 10, false);
}

void Pattern::addNailPattern(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int offset_x = static_cast<int>(cx) - 12;
    int offset_y = static_cast<int>(cy) - 12;

    setCell(grid, offset_x + 5, offset_y + 14, false);
    setCell(grid, offset_x + 6, offset_y + 14, false);
    setCell(grid, offset_x + 7, offset_y + 14, false);
    setCell(grid, offset_x + 6, offset_y + 15, true);
}

void Pattern::addGatesPattern(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    int offset_x = static_cast<int>(cx) - 12;
    int offset_y = static_cast<int>(cy) - 12;

    setCell(grid, offset_x + 20, offset_y + 15, true);
    setCell(grid, offset_x + 21, offset_y + 14, false);
    setCell(grid, offset_x + 23, offset_y + 14, false);
    setCell(grid, offset_x + 21, offset_y + 16, false);
    setCell(grid, offset_x + 23, offset_y + 16, false);
}

void Pattern::addBackgroundWithVertical(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    fillStripedBackground(grid);
    addVerticalElement(grid, cx, cy);
}

void Pattern::addBackgroundWithEye(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    fillStripedBackground(grid);
    addEyePattern(grid, cx, cy);
}

void Pattern::addBackgroundWithEight(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    fillStripedBackground(grid);
    addEightPattern(grid, cx, cy);
}

void Pattern::addBackgroundWithNail(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    fillStripedBackground(grid);
    addNailPattern(grid, cx, cy);
}

void Pattern::addBackgroundWithGates(vector<vector<bool>>& grid, unsigned cx, unsigned cy) {
    fillStripedBackground(grid);
    addGatesPattern(grid, cx, cy);
}
