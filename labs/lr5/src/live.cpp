#include "live.h"

GameLive::GameLive(GameRule rule, unsigned num_cells, unsigned cell_size, const string& pattern)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      current_pattern(pattern),
      current_rule(rule),
      alive_color(LiveGameConsts::CELL_COLOR),
      dead_color(Consts::BACKGROUND_COLOR) {
    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));

    pattern_manager = make_unique<Pattern>(num_cells);

    initGrid();
    updateCellColors();

    cout << "GameLive init with rule: " << (rule == GameRule::B3S23 ? "B3S23" : "B2S012") << endl;
    cout << "Available patterns: " << getAvailablePatterns().size() << endl;
}

GameLive::~GameLive() { cout << "GameLive destroyed" << endl; }

void GameLive::initGrid() { pattern_manager->applyPattern(current_pattern, grid, current_rule); }

void GameLive::clearGrid() {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            cell_colors[y][x] = dead_color;
        }
    }
    updateCellColors();
}

void GameLive::updateCellColors() {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x]) {
                cell_colors[y][x] = alive_color;
            } else {
                cell_colors[y][x] = dead_color;
            }
        }
    }
}

int GameLive::countNeighbors(unsigned x, unsigned y) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = static_cast<int>(x) + dx;
            int ny = static_cast<int>(y) + dy;

            if (nx >= 0 && nx < static_cast<int>(num_cells) && ny >= 0 &&
                ny < static_cast<int>(num_cells)) {
                if (grid[ny][nx]) {
                    count++;
                }
            }
        }
    }

    return count;
}

bool GameLive::applyConwayRules(bool is_alive, int neighbors) {
    if (is_alive) {
        return (neighbors == 2 || neighbors == 3);
    } else {
        return (neighbors == 3);
    }
}

bool GameLive::applyB2S012Rules(bool is_alive, int neighbors) {
    if (is_alive) {
        return (neighbors == 0 || neighbors == 1 || neighbors == 2);
    } else {
        return (neighbors == 2);
    }
}

void GameLive::updateGrid(bool force_update) {
    if (!is_running && !force_update) {
        return;
    }

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            int neighbors = countNeighbors(x, y);

            if (current_rule == GameRule::B3S23) {
                next_grid[y][x] = applyConwayRules(grid[y][x], neighbors);
            } else {
                next_grid[y][x] = applyB2S012Rules(grid[y][x], neighbors);
            }
        }
    }

    grid.swap(next_grid);
}

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
    pattern_manager->applyPattern(pattern_name, grid, current_rule);
}

void GameLive::reset() {
    clearGrid();
    is_running = false;
    updateCellColors();
}

void GameLive::setRule(GameRule rule) {
    if (current_rule != rule) {
        current_rule = rule;
        clearGrid();
        cout << "rule changed to " << (rule == GameRule::B3S23 ? "B3S23" : "B2S012") << endl;
        updateCellColors();
    }
}

vector<tuple<string, string, unsigned>> GameLive::getAvailablePatterns() const {
    if (current_rule == GameRule::B3S23) {
        return LiveGameConsts::CONWAY_PATTERNS;
    } else {
        return LiveGameConsts::B2S012_PATTERNS;
    }
}

void GameLive::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    static unsigned current_pattern_index = 0;
    auto available_patterns = getAvailablePatterns();

    switch (key_event.scancode) {
        case sf::Keyboard::Scan::R:
            randomizeGrid();
            is_running = false;
            current_pattern_index = 0;
            updateCellColors();
            cout << "grid randomized" << endl;
            break;

        case sf::Keyboard::Scan::C:
            clearGrid();
            is_running = false;
            updateCellColors();
            cout << "grid cleared" << endl;
            break;

        case sf::Keyboard::Scan::G:
            if (current_rule == GameRule::B3S23) {
                setRule(GameRule::B2S012);
            } else {
                setRule(GameRule::B3S23);
            }
            is_running = false;
            current_pattern_index = 0;
            loadPattern("random");
            updateCellColors();
            break;

        case sf::Keyboard::Scan::Num1:
        case sf::Keyboard::Scan::Num2:
        case sf::Keyboard::Scan::Num3:
        case sf::Keyboard::Scan::Num4:
        case sf::Keyboard::Scan::Num5:
        case sf::Keyboard::Scan::Num6:
        case sf::Keyboard::Scan::Num7:
        case sf::Keyboard::Scan::Num8:
        case sf::Keyboard::Scan::Num9: {
            int index =
                static_cast<int>(key_event.scancode) - static_cast<int>(sf::Keyboard::Scan::Num1);
            if (index >= 0 && index < static_cast<int>(available_patterns.size())) {
                current_pattern_index = index;
                loadPattern(get<0>(available_patterns[current_pattern_index]));
                is_running = false;
                updateCellColors();
                cout << "pattern loaded: " << get<1>(available_patterns[current_pattern_index])
                     << endl;
            }
            break;
        }

        default:
            BaseCellAutomaton::handleKeyPress(key_event);
            break;
    }
}
