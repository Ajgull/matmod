#include "organism.h"

Organism::Organism(unsigned num_cells, unsigned cell_size)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      update_interval(Consts::DEFAULT_UPDATE_INTERVAL),
      grid_color(Consts::GRID_COLOR),
      cell_color(OrganismsConsts::CELL_COLOR),
      background_color(Consts::BACKGROUND_COLOR) {
    width = cell_size * num_cells;
    height = cell_size * num_cells;

    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));

    initRendering();

    last_update = chrono::steady_clock::now();
}

Organism::~Organism() { cout << "Organism Network destroyed" << endl; }

void Organism::initRendering() {
    if (rendering_initialized) {
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

    rendering_initialized = true;
}

void Organism::increaseSpeed() {
    if (update_interval > Consts::MAX_SPEED) {
        update_interval -= Consts::SPEED_STEP;
        cout << "speed increased = " << update_interval << "s" << endl;
    }
}

void Organism::decreaseSpeed() {
    if (update_interval < Consts::MIN_SPEED) {
        update_interval += Consts::SPEED_STEP;
        cout << "speed decreased: " << update_interval << "s" << endl;
    }
}

void Organism::updateGrid(bool force_update) {
    if (!is_running && !force_update) {
        return;
    }
}

void Organism::drawGrid(sf::RenderWindow& window) { window.draw(grid_lines); }

void Organism::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            cell_shapes[y][x].setFillColor(cell_color);
            window.draw(cell_shapes[y][x]);
        }
    }
}

void Organism::run() {
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
        }

        window.clear(background_color);
        drawGrid(window);
        drawCells(window);
        window.display();
    }
}