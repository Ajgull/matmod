#include "neural.h"

NeuralNetwork::NeuralNetwork(unsigned num_cells, unsigned cell_size)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      current_tick(0),
      update_interval(Consts::DEFAULT_UPDATE_INTERVAL),
      grid_color(Consts::GRID_COLOR),
      cell_color_active(NeuralNetworkConsts::CELL_COLOR_ACTIVE),
      cell_color_rest(NeuralNetworkConsts::CELL_COLOR_REST),
      cell_color_recovery(NeuralNetworkConsts::CELL_COLOR_RECOVERY),
      background_color(Consts::BACKGROUND_COLOR) {
    width = cell_size * num_cells;
    height = cell_size * num_cells;

    grid.resize(num_cells, vector<CellState>(num_cells, CellState::REST));
    next_grid.resize(num_cells, vector<CellState>(num_cells, CellState::REST));

    activator_level.resize(num_cells, vector<float>(num_cells, 0.0f));
    state_timer.resize(num_cells, vector<int>(num_cells, 0));

    initFlatFront();
    initRendering();
    // initGenerator();
    last_update = chrono::steady_clock::now();
}

NeuralNetwork::~NeuralNetwork() { cout << "Neural Network destroyed" << endl; }

void NeuralNetwork::initRendering() {
    if (rendering_initialized) return;

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

    rendering_initialized = true;
}

void NeuralNetwork::initFlatFront() {
    unsigned front_x = 450;

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned dx = 0; dx < 3; dx++) {
            unsigned x = front_x + dx;
            if (x < num_cells) {
                grid[y][x] = CellState::ACTIVE;
                activator_level[y][x] = 1.0f;
                state_timer[y][x] = NeuralNetworkConsts::T;
            }
        }
    }
    cout << "Initialized flat front at x = " << front_x << endl;
}

void NeuralNetwork::initGenerator() {
    int center_y = num_cells / 2;
    int center_x = num_cells / 2;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int y = center_y + dy;
            int x = center_x + dx;

            if (y >= 0 && y < static_cast<int>(num_cells) && x >= 0 &&
                x < static_cast<int>(num_cells)) {
                if (grid[y][x] == CellState::REST) {
                    grid[y][x] = CellState::ACTIVE;
                    activator_level[y][x] = 1.0f;
                    state_timer[y][x] = NeuralNetworkConsts::T;
                }
            }
        }
    }
}

void NeuralNetwork::increaseSpeed() {
    if (update_interval > Consts::MAX_SPEED) {
        update_interval -= Consts::SPEED_STEP;
        cout << "speed increased = " << update_interval << "s" << endl;
    }
}

void NeuralNetwork::decreaseSpeed() {
    if (update_interval < Consts::MIN_SPEED) {
        update_interval += Consts::SPEED_STEP;
        cout << "speed decreased: " << update_interval << "s" << endl;
    }
}

void NeuralNetwork::updateGrid(bool force_update) {
    if (!is_running && !force_update) return;

    current_tick++;

    vector<vector<CellState>> new_grid =
        vector<vector<CellState>>(num_cells, vector<CellState>(num_cells, CellState::REST));
    vector<vector<float>> new_activator =
        vector<vector<float>>(num_cells, vector<float>(num_cells, 0.0f));
    vector<vector<int>> new_timer = vector<vector<int>>(num_cells, vector<int>(num_cells, 0));

    const float decay = 1.0f - NeuralNetworkConsts::A;
    const int max_idx = static_cast<int>(num_cells);

    for (int y = 0; y < max_idx; ++y) {
        for (int x = 0; x < max_idx; ++x) {
            int y1 = max(0, y - 1);
            int y2 = min(max_idx, y + 2);
            int x1 = max(0, x - 1);
            int x2 = min(max_idx, x + 2);

            float sum = 0.0f;
            for (int ny = y1; ny < y2; ++ny) {
                for (int nx = x1; nx < x2; ++nx) {
                    sum += activator_level[ny][nx];
                }
            }

            CellState state = grid[y][x];
            int timer = state_timer[y][x];
            float activator = activator_level[y][x] * decay;

            switch (state) {
                case CellState::REST:
                    if (sum >= NeuralNetworkConsts::P) {
                        new_grid[y][x] = CellState::ACTIVE;
                        new_timer[y][x] = NeuralNetworkConsts::T;
                        new_activator[y][x] = 1.0f;
                    } else {
                        new_grid[y][x] = CellState::REST;
                        new_activator[y][x] = activator;
                    }
                    break;

                case CellState::ACTIVE:
                    new_activator[y][x] = 1.0f;
                    if (timer > 1) {
                        new_grid[y][x] = CellState::ACTIVE;
                        new_timer[y][x] = timer - 1;
                    } else {
                        new_grid[y][x] = CellState::RECOVERY;
                        new_timer[y][x] = NeuralNetworkConsts::B;
                    }
                    break;

                case CellState::RECOVERY:
                    new_activator[y][x] = activator;
                    if (timer > 1) {
                        new_grid[y][x] = CellState::RECOVERY;
                        new_timer[y][x] = timer - 1;
                    } else {
                        new_grid[y][x] = CellState::REST;
                    }
                    break;
            }
        }
    }

    grid = move(new_grid);
    activator_level = move(new_activator);
    state_timer = move(new_timer);

    if (current_tick % NeuralNetworkConsts::SOURCE_PERIOD == 0) {
        initGenerator();
    }

    next_grid = grid;
}

void NeuralNetwork::drawGrid(sf::RenderWindow& window) { window.draw(grid_lines); }

void NeuralNetwork::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            switch (grid[y][x]) {
                case CellState::ACTIVE:
                    cell_shapes[y][x].setFillColor(cell_color_active);
                    window.draw(cell_shapes[y][x]);
                    break;
                case CellState::RECOVERY:
                    cell_shapes[y][x].setFillColor(cell_color_recovery);
                    window.draw(cell_shapes[y][x]);
                    break;
                case CellState::REST:
                    cell_shapes[y][x].setFillColor(cell_color_rest);
                    window.draw(cell_shapes[y][x]);
                    break;
            }
        }
    }
}

void NeuralNetwork::run() {
    sf::RenderWindow window(sf::VideoMode({width, height}), "Neural Network");
    window.setFramerateLimit(Consts::FRAME_LIMIT);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto* key_event = event->getIf<sf::Event::KeyPressed>()) {
                switch (key_event->scancode) {
                    case sf::Keyboard::Scan::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Scan::Space:
                        is_running = !is_running;
                        break;
                    case sf::Keyboard::Scan::Equal:  // increase speed
                        increaseSpeed();
                        break;

                    case sf::Keyboard::Scan::Hyphen:  // decrease speed
                        decreaseSpeed();
                        break;

                    case sf::Keyboard::Scan::Right:  // one next step
                        updateGrid(true);
                        cout << "step forward" << endl;
                        break;

                    default:
                        break;
                }
            }
        }

        auto now = chrono::steady_clock::now();
        chrono::duration<float> elapsed = now - last_update;

        if (is_running && elapsed.count() >= update_interval) {
            updateGrid(false);
            last_update = now;

            if (current_tick % 100 == 0) {
                int active = 0, recovery = 0, rest = 0;
                for (unsigned y = 0; y < num_cells; y++) {
                    for (unsigned x = 0; x < num_cells; x++) {
                        if (grid[y][x] == CellState::ACTIVE)
                            active++;
                        else if (grid[y][x] == CellState::RECOVERY)
                            recovery++;
                        else
                            rest++;
                    }
                }
                cout << "Tick " << current_tick << ": Active=" << active
                     << ", Recovery=" << recovery << ", Rest=" << rest << endl;
            }
        }

        window.clear(background_color);
        drawGrid(window);
        drawCells(window);
        window.display();
    }
}