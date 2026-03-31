#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "consts.h"
#include "grid.h"

using namespace std;

class BaseCellAutomaton {
   protected:
    unsigned num_cells;
    unsigned cell_size;
    bool is_running;
    float update_interval;
    chrono::steady_clock::time_point last_update;

    sf::Color background_color;
    sf::Color grid_color;
    unique_ptr<Grid> grid;
    vector<vector<sf::Color>> cell_colors;

    bool recording;
    bool recording_requested;
    unsigned int frame_counter;
    unsigned int recording_start_frame;
    string recording_filename;
    vector<string> recorded_frames;

    virtual void initGrid() = 0;
    virtual void updateGrid(bool force_update) = 0;
    virtual void updateCellColors() = 0;
    virtual void handleKeyPress(const sf::Event::KeyPressed& key_event);

    void drawGrid(sf::RenderWindow& window);
    void drawCells(sf::RenderWindow& window);

    void startRecording(const string& filename = "animation");
    void stopRecording();
    void saveFrame(sf::RenderWindow& window);
    void convertToGIF();
    void cleanupTempFiles();

   public:
    BaseCellAutomaton(unsigned num_cells, unsigned cell_size,
                      float default_update_interval = Consts::DEFAULT_UPDATE_INTERVAL);
    virtual ~BaseCellAutomaton();

    virtual void run();
    void increaseSpeed();
    void decreaseSpeed();
    void setRunning(bool running) { is_running = running; }
    bool isRunning() const { return is_running; }
    void setGridColor(const sf::Color& color);
    void setBackgroundColor(const sf::Color& color);
    sf::Color getGridColor() const { return grid_color; }
    sf::Color getBackgroundColor() const { return background_color; }
    virtual void reset() = 0;

    void toggleRecording();
    bool isRecording() const { return recording; }
};