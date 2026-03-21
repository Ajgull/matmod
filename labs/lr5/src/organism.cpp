#include "organism.h"

Organism::Organism(unsigned num_cells, unsigned cell_size)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      cell_color(OrganismsConsts::CELL_COLOR) {
    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));

    initRendering();
    initGrid();
}

Organism::~Organism() { cout << "Organism destroyed" << endl; }

void Organism::initGrid() {
    // Базовая инициализация - все клетки мертвы
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            grid[y][x] = false;
        }
    }
}

void Organism::updateGrid(bool force_update) {
    if (!is_running && !force_update) {
        return;
    }

    // TODO: Реализовать логику обновления для организма
    // Пока просто копируем состояние
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            next_grid[y][x] = grid[y][x];
        }
    }
    grid.swap(next_grid);
}

void Organism::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x]) {
                cell_shapes[y][x].setFillColor(cell_color);
                window.draw(cell_shapes[y][x]);
            }
        }
    }
}

void Organism::reset() {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            grid[y][x] = false;
        }
    }
    is_running = false;
}