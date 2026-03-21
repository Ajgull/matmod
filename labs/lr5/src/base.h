#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "consts.h"

using namespace std;

class BaseCellAutomaton {
   protected:
    unsigned width, height, cell_size, num_cells;
    bool is_running;
    float update_interval;
    chrono::steady_clock::time_point last_update;

    sf::Color grid_color;
    sf::Color background_color;

    bool rendering_initialized = false;
    sf::VertexArray grid_lines;
    vector<vector<sf::RectangleShape>> cell_shapes;

    virtual void initGrid() = 0;
    virtual void updateGrid(bool force_update) = 0;
    virtual void drawCells(sf::RenderWindow& window) = 0;

    void initRendering();
    void drawGrid(sf::RenderWindow& window);
    virtual void handleKeyPress(const sf::Event::KeyPressed& key_event);

   public:
    BaseCellAutomaton(unsigned num_cells, unsigned cell_size,
                      float default_update_interval = Consts::DEFAULT_UPDATE_INTERVAL);
    virtual ~BaseCellAutomaton();

    virtual void run();
    void increaseSpeed();
    void decreaseSpeed();
    void setRunning(bool running) { is_running = running; }
    bool isRunning() const { return is_running; }

    virtual void reset() = 0;
};