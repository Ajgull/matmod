#include "live.h"

GameLive::GameLive(unsigned num_cells, unsigned cell_size, const string& pattern)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      current_pattern(pattern),
      cell_color(LiveGameConsts::CELL_COLOR) {
    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));

    pattern_manager = make_unique<Pattern>(num_cells);

    initRendering();
    initGrid();

    cout << "GameLive init" << endl;

    cout << "initial pattern " << get<1>(LiveGameConsts::PATTERNS[0]) << endl;
}

GameLive::~GameLive() { cout << "GameLive destroyed" << endl; }

void GameLive::initGrid() { pattern_manager->applyPattern(current_pattern, grid); }

void GameLive::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x]) {
                cell_shapes[y][x].setFillColor(cell_color);
                window.draw(cell_shapes[y][x]);
            }
        }
    }
}

int GameLive::countNeighbors(unsigned x, unsigned y) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = (x + dx + num_cells) % num_cells;
            int ny = (y + dy + num_cells) % num_cells;

            if (grid[ny][nx]) {
                count++;
            }
        }
    }

    return count;
}

void GameLive::updateGrid(bool force_update) {
    if (!is_running && !force_update) {
        return;
    }

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            int neighbors = countNeighbors(x, y);

            if (grid[y][x]) {
                next_grid[y][x] = (neighbors == 2 || neighbors == 3);
            } else {
                next_grid[y][x] = (neighbors == 3);
            }
        }
    }

    grid.swap(next_grid);
}

void GameLive::clearGrid() { pattern_manager->clearGrid(grid); }

void GameLive::randomizeGrid() { pattern_manager->randomizeGrid(grid); }

void GameLive::setCell(unsigned x, unsigned y, bool state) {
    if (x < num_cells && y < num_cells) {
        grid[y][x] = state;
    }
}

void GameLive::toggleCell(unsigned x, unsigned y) {
    if (x < num_cells && y < num_cells) {
        grid[y][x] = !grid[y][x];
    }
}

void GameLive::loadPattern(const string& pattern_name) {
    current_pattern = pattern_name;
    pattern_manager->applyPattern(pattern_name, grid);
}

void GameLive::reset() {
    clearGrid();
    is_running = false;
}

void GameLive::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    static unsigned current_pattern_index = 0;

    switch (key_event.scancode) {
        case sf::Keyboard::Scan::R:  // random
            randomizeGrid();
            is_running = false;
            current_pattern_index = 0;
            cout << "grid randomized" << endl;
            break;

        case sf::Keyboard::Scan::C:  // clear
            clearGrid();
            is_running = false;
            cout << "grid cleared" << endl;
            break;

        case sf::Keyboard::Scan::Num1:
        case sf::Keyboard::Scan::Num2:
        case sf::Keyboard::Scan::Num3:
        case sf::Keyboard::Scan::Num4:
        case sf::Keyboard::Scan::Num5:
        case sf::Keyboard::Scan::Num6:
        case sf::Keyboard::Scan::Num7:
        case sf::Keyboard::Scan::Num8: {
            int index = static_cast<int>(key_event.scancode) -
                        static_cast<int>(sf::Keyboard::Scan::Num1) + 1;
            if (index >= 1 && index < static_cast<int>(LiveGameConsts::PATTERNS.size())) {
                current_pattern_index = index;
                loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                is_running = false;
                cout << "pattern " << get<1>(LiveGameConsts::PATTERNS[current_pattern_index])
                     << endl;
            }
            break;
        }

        default:
            BaseCellAutomaton::handleKeyPress(key_event);
            break;
    }
}

void GameLive::run() {
    sf::RenderWindow window(sf::VideoMode({width, height}), get<0>(LiveGameConsts::PATTERNS[0]));
    window.setFramerateLimit(Consts::FRAME_LIMIT);

    bool mouse_pressed = false;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto* key_event = event->getIf<sf::Event::KeyPressed>()) {
                if (key_event->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                } else {
                    handleKeyPress(*key_event);
                }
            } else if (auto* mouse_press = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse_press->button == sf::Mouse::Button::Left) {
                    mouse_pressed = true;
                    auto pos = mouse_press->position;
                    unsigned x = static_cast<unsigned>(pos.x) / cell_size;
                    unsigned y = static_cast<unsigned>(pos.y) / cell_size;
                    toggleCell(x, y);
                }
            } else if (auto* mouse_release = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse_release->button == sf::Mouse::Button::Left) {
                    mouse_pressed = false;
                }
            } else if (auto* mouse_move = event->getIf<sf::Event::MouseMoved>()) {
                if (mouse_pressed) {
                    auto pos = mouse_move->position;
                    unsigned x = static_cast<unsigned>(pos.x) / cell_size;
                    unsigned y = static_cast<unsigned>(pos.y) / cell_size;
                    toggleCell(x, y);
                }
            }
        }

        auto now = chrono::steady_clock::now();
        chrono::duration<float> elapsed = now - last_update;

        if (is_running && elapsed.count() >= update_interval) {
            updateGrid(false);
            last_update = now;
        }

        window.clear(background_color);
        drawGrid(window);
        drawCells(window);
        window.display();
    }
}