#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class Grid {
   private:
    unsigned num_cells;
    unsigned cell_size;
    unsigned width;
    unsigned height;

    sf::Color grid_color;
    sf::VertexArray grid_lines;
    vector<vector<sf::RectangleShape>> cell_shapes;

    bool initialized = false;

   public:
    Grid(unsigned num_cells, unsigned cell_size, sf::Color grid_color);
    ~Grid();

    void init();

    unsigned getWidth() const { return width; }
    unsigned getHeight() const { return height; }
    unsigned getNumCells() const { return num_cells; }
    unsigned getCellSize() const { return cell_size; }

    void drawGrid(sf::RenderWindow& window);

    void drawCells(sf::RenderWindow& window, const vector<vector<sf::Color>>& cell_colors);

    sf::RectangleShape& getCellShape(unsigned x, unsigned y) { return cell_shapes[y][x]; }

    const sf::RectangleShape& getCellShape(unsigned x, unsigned y) const {
        return cell_shapes[y][x];
    }
};