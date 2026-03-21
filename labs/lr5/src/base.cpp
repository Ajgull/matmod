#include "base.h"

BaseCellAutomaton::BaseCellAutomaton(unsigned num_cells, unsigned cell_size,
                                     float default_update_interval)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      update_interval(default_update_interval),
      grid_color(Consts::GRID_COLOR),
      background_color(Consts::BACKGROUND_COLOR) {
    width = cell_size * num_cells;
    height = cell_size * num_cells;

    cout << "BaseCellAutomaton init" << endl;
    last_update = chrono::steady_clock::now();
}

BaseCellAutomaton::~BaseCellAutomaton() { cout << "BaseCellAutomaton destroyed" << endl; }

void BaseCellAutomaton::initRendering() {
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

void BaseCellAutomaton::drawGrid(sf::RenderWindow& window) { window.draw(grid_lines); }

void BaseCellAutomaton::increaseSpeed() {
    if (update_interval > Consts::MAX_SPEED) {
        update_interval -= Consts::SPEED_STEP;
        cout << "speed increased = " << update_interval << "s" << endl;
    }
}

void BaseCellAutomaton::decreaseSpeed() {
    if (update_interval < Consts::MIN_SPEED) {
        update_interval += Consts::SPEED_STEP;
        cout << "speed decreased: " << update_interval << "s" << endl;
    }
}

void BaseCellAutomaton::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    switch (key_event.scancode) {
        case sf::Keyboard::Scan::Escape:
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

void BaseCellAutomaton::run() {
    sf::RenderWindow window(sf::VideoMode({width, height}), "Cell Automaton");
    window.setFramerateLimit(Consts::FRAME_LIMIT);

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