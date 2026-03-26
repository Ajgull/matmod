#include "grid.h"

Grid::Grid(unsigned num_cells, unsigned cell_size, sf::Color grid_color)
    : num_cells(num_cells), cell_size(cell_size), grid_color(grid_color) {
    width = cell_size * num_cells;
    height = cell_size * num_cells;
    cout << "Grid init " << endl;
}

Grid::~Grid() { cout << "Grid destroyed" << endl; }

void Grid::init() {
    if (initialized) {
        return;
    }

    grid_lines.setPrimitiveType(sf::PrimitiveType::Lines);
    for (unsigned x = 0; x <= num_cells; x++) {
        float x_pos = static_cast<float>(x * cell_size);
        grid_lines.append({{x_pos, 0}, grid_color});
        grid_lines.append({{x_pos, static_cast<float>(height)}, grid_color});
    }
    for (unsigned y = 0; y <= num_cells; y++) {
        float y_pos = static_cast<float>(y * cell_size);
        grid_lines.append({{0, y_pos}, grid_color});
        grid_lines.append({{static_cast<float>(width), y_pos}, grid_color});
    }

    cell_shapes.resize(num_cells, vector<sf::RectangleShape>(num_cells));
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            cell_shapes[y][x].setSize(
                sf::Vector2f(static_cast<float>(cell_size - 1), static_cast<float>(cell_size - 1)));
            cell_shapes[y][x].setPosition(
                sf::Vector2f(static_cast<float>(x * cell_size), static_cast<float>(y * cell_size)));
        }
    }

    initialized = true;
}

void Grid::drawGrid(sf::RenderWindow& window) { window.draw(grid_lines); }

void Grid::drawCells(sf::RenderWindow& window, const vector<vector<sf::Color>>& cell_colors) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            cell_shapes[y][x].setFillColor(cell_colors[y][x]);
            window.draw(cell_shapes[y][x]);
        }
    }
}