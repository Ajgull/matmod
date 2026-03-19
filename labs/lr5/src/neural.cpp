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
    cout << "haha" << endl;

    width = cell_size * num_cells;
    height = cell_size * num_cells;

    grid.resize(num_cells, vector<CellState>(num_cells, CellState::REST));
    next_grid.resize(num_cells, vector<CellState>(num_cells, CellState::REST));

    activation_timer.resize(num_cells, vector<int>(num_cells, 0));
    recovery_timer.resize(num_cells, vector<int>(num_cells, 0));

    initFlatFront();
    last_update = chrono::steady_clock::now();
    initRendering();
}

NeuralNetwork::~NeuralNetwork() { cout << "neural weetwork destroyed" << endl; }

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
    unsigned center_x = num_cells / 2;

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned dx = 0; dx < 3; dx++) {
            unsigned x = center_x - 1 + dx;
            if (x < num_cells) {
                grid[y][x] = CellState::ACTIVE;
                activation_timer[y][x] = NeuralNetworkConsts::T;
            }
        }
    }
    cout << "Initialized flat front at x = " << center_x - 1 << " to " << center_x + 1 << endl;
}

void NeuralNetwork::updateSource() {
    current_tick++;

    if (current_tick % NeuralNetworkConsts::SOURCE_PERIOD == 0) {
        unsigned source_x = 5;
        unsigned source_y = 5;

        for (unsigned dy = 0; dy < 3; dy++) {
            for (unsigned dx = 0; dx < 3; dx++) {
                unsigned x = source_x + dx;
                unsigned y = source_y + dy;

                if (x < num_cells && y < num_cells) {
                    if (grid[y][x] == CellState::REST) {
                        next_grid[y][x] = CellState::ACTIVE;
                        activation_timer[y][x] = NeuralNetworkConsts::T;
                    }
                }
            }
        }

        cout << "Periodic source activated at tick " << current_tick << endl;
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

float NeuralNetwork::calculateActivatorLevel(unsigned x, unsigned y) {
    float activator = 0.0f;

    if (grid[y][x] == CellState::ACTIVE) {
        activator += 1.0f;
    }

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < static_cast<int>(num_cells) && ny >= 0 &&
                ny < static_cast<int>(num_cells)) {
                if (grid[ny][nx] == CellState::ACTIVE) {
                    activator += 1.0f;
                } else if (grid[ny][nx] == CellState::RECOVERY) {
                    float decay = 1.0f - (static_cast<float>(recovery_timer[ny][nx]) /
                                          NeuralNetworkConsts::B) *
                                             NeuralNetworkConsts::A;
                    activator += max(0.0f, decay);
                }
            }
        }
    }

    return activator;
}

int NeuralNetwork::countNeighbors(unsigned x, unsigned y, CellState state) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < static_cast<int>(num_cells) && ny >= 0 &&
                ny < static_cast<int>(num_cells)) {
                if (grid[ny][nx] == state) {
                    count++;
                }
            }
        }
    }
    return count;
}

void NeuralNetwork::drawGrid(sf::RenderWindow& window) { window.draw(grid_lines); }

void NeuralNetwork::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            switch (grid[y][x]) {
                case CellState::ACTIVE:
                    cell_shapes[y][x].setFillColor(cell_color_active);
                    break;
                case CellState::RECOVERY:
                    cell_shapes[y][x].setFillColor(cell_color_recovery);
                    break;
                case CellState::REST:
                    cell_shapes[y][x].setFillColor(cell_color_rest);
                    break;
            }
            window.draw(cell_shapes[y][x]);
        }
    }
}

void NeuralNetwork::updateGrid(bool force_update = false) {
    if (!is_running && !force_update) {
        return;
    }

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            next_grid[y][x] = grid[y][x];
        }
    }

    updateSource();

    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            switch (grid[y][x]) {
                case CellState::ACTIVE:
                    activation_timer[y][x]--;

                    if (activation_timer[y][x] <= 0) {
                        next_grid[y][x] = CellState::RECOVERY;
                        recovery_timer[y][x] = NeuralNetworkConsts::B;
                    }
                    break;

                case CellState::RECOVERY:
                    recovery_timer[y][x]--;

                    if (recovery_timer[y][x] <= 0) {
                        next_grid[y][x] = CellState::REST;
                    }
                    break;

                case CellState::REST:
                    float activator = calculateActivatorLevel(x, y);

                    if (activator >= NeuralNetworkConsts::P) {
                        next_grid[y][x] = CellState::ACTIVE;
                        activation_timer[y][x] = NeuralNetworkConsts::T;
                    }
                    break;
            }
        }
    }

    grid.swap(next_grid);
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
            updateGrid();
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