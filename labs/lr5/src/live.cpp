#include "live.h"

GameLive::GameLive(unsigned num_cells, unsigned cell_size, const string& pattern)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      update_interval(Consts::DEFAULT_UPDATE_INTERVAL),
      current_pattern(pattern),
      grid_color(Consts::GRID_COLOR),
      cell_color(LiveGameConsts::CELL_COLOR),
      background_color(Consts::BACKGROUND_COLOR) {
    width = cell_size * num_cells;
    height = cell_size * num_cells;

    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));

    pattern_manager = make_unique<Pattern>(num_cells);
    last_update = chrono::steady_clock::now();

    loadPattern(pattern);
    cout << "initial pattern " << get<1>(LiveGameConsts::PATTERNS[0]) << endl;
}

GameLive::~GameLive() { cout << "game destroyed" << endl; }

void GameLive::drawGrid(sf::RenderWindow& window) {
    sf::VertexArray grid_lines(sf::PrimitiveType::Lines);

    for (unsigned x = 0; x <= num_cells; x++) {
        float x_pos = x * cell_size;
        sf::Vertex v1;
        v1.position = sf::Vector2f(x_pos, 0);
        v1.color = grid_color;

        sf::Vertex v2;
        v2.position = sf::Vector2f(x_pos, height);
        v2.color = grid_color;

        grid_lines.append(v1);
        grid_lines.append(v2);
    }

    for (unsigned y = 0; y <= num_cells; y++) {
        float y_pos = y * cell_size;
        sf::Vertex v1;
        v1.position = sf::Vector2f(0, y_pos);
        v1.color = grid_color;

        sf::Vertex v2;
        v2.position = sf::Vector2f(width, y_pos);
        v2.color = grid_color;

        grid_lines.append(v1);
        grid_lines.append(v2);
    }

    window.draw(grid_lines);
}

void GameLive::drawCells(sf::RenderWindow& window) {
    sf::RectangleShape cell_shape(sf::Vector2f(cell_size - 1, cell_size - 1));
    cell_shape.setFillColor(cell_color);

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x]) {
                cell_shape.setPosition(sf::Vector2f(x * cell_size, y * cell_size));
                window.draw(cell_shape);
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

void GameLive::updateGrid(bool force_update = false) {
    if (!is_running && !force_update) {
        return;
    }

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            int neighbors = countNeighbors(x, y);

            if (grid[y][x]) {
                if (neighbors == 2 || neighbors == 3) {
                    next_grid[y][x] = true;
                } else {
                    next_grid[y][x] = false;
                }
            } else {
                if (neighbors == 3) {
                    next_grid[y][x] = true;
                } else {
                    next_grid[y][x] = false;
                }
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

void GameLive::increaseSpeed() {
    if (update_interval > Consts::MAX_SPEED) {
        update_interval -= Consts::SPEED_STEP;
        cout << "speed increased = " << update_interval << "s" << endl;
    }
}

void GameLive::decreaseSpeed() {
    if (update_interval < Consts::MIN_SPEED) {
        update_interval += Consts::SPEED_STEP;
        cout << "speed decreased: " << update_interval << "s" << endl;
    }
}

void GameLive::run() {
    bool mouse_pressed = false;
    bool single_step = false;

    unsigned current_pattern_index = 0;
    for (unsigned i = 0; i < LiveGameConsts::PATTERNS.size(); i++) {
        if (get<0>(LiveGameConsts::PATTERNS[i]) == current_pattern) {
            current_pattern_index = i;
            break;
        }
    }

    sf::RenderWindow window(sf::VideoMode({width, height}),
                            get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
    window.setFramerateLimit(Consts::FRAME_LIMIT);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto* key_event = event->getIf<sf::Event::KeyPressed>()) {
                switch (key_event->scancode) {
                    case sf::Keyboard::Scan::Space:  // run
                        is_running = !is_running;
                        break;

                    case sf::Keyboard::Scan::R:  // random
                        randomizeGrid();
                        is_running = false;
                        current_pattern_index = 0;
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        cout << "grid randomized" << endl;
                        break;

                    case sf::Keyboard::Scan::C:  // clear
                        clearGrid();
                        is_running = false;
                        cout << "grid cleared" << endl;
                        break;

                    case sf::Keyboard::Scan::Right:  // one next step
                        updateGrid(true);
                        cout << "step forward" << endl;
                        break;

                    case sf::Keyboard::Scan::Num1:  // Блок статичный
                        current_pattern_index = 1;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num2:  // Улей статичный
                        current_pattern_index = 2;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num3:  // Буханка статичный
                        current_pattern_index = 3;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num4:  // Мигалка цикличный
                        current_pattern_index = 4;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num5:  // Жаба
                        current_pattern_index = 5;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num6:  // Планер цикличный
                        current_pattern_index = 6;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num7:  // Пульсар цикличный не сразу
                        current_pattern_index = 7;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Num8:  // Пушка Госпера
                        current_pattern_index = 8;
                        loadPattern(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        window.setTitle(get<0>(LiveGameConsts::PATTERNS[current_pattern_index]));
                        is_running = false;
                        cout << "pattern "
                             << get<1>(LiveGameConsts::PATTERNS[current_pattern_index]) << endl;
                        break;

                    case sf::Keyboard::Scan::Equal:  // increase speed
                        increaseSpeed();
                        break;

                    case sf::Keyboard::Scan::Hyphen:  // decrease speed
                        decreaseSpeed();
                        break;

                    case sf::Keyboard::Scan::Escape:  // exit
                        window.close();
                        break;

                    default:
                        break;
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
            updateGrid();
            last_update = now;
        }

        window.clear(background_color);
        drawGrid(window);
        drawCells(window);
        window.display();
    }
}