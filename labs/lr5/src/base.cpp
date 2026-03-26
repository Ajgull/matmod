#include "base.h"

BaseCellAutomaton::BaseCellAutomaton(unsigned num_cells, unsigned cell_size,
                                     float default_update_interval)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      update_interval(default_update_interval),
      background_color(Consts::BACKGROUND_COLOR) {
    grid = make_unique<Grid>(num_cells, cell_size, Consts::GRID_COLOR);
    grid->init();
    cell_colors.resize(num_cells, vector<sf::Color>(num_cells, Consts::BACKGROUND_COLOR));

    cout << "BaseCellAutomaton init" << endl;
    last_update = chrono::steady_clock::now();
}

BaseCellAutomaton::~BaseCellAutomaton() { cout << "BaseCellAutomaton destroyed" << endl; }

void BaseCellAutomaton::drawGrid(sf::RenderWindow& window) {
    if (grid) {
        grid->drawGrid(window);
    }
}

void BaseCellAutomaton::drawCells(sf::RenderWindow& window) {
    if (grid) {
        grid->drawCells(window, cell_colors);
    }
}

void BaseCellAutomaton::increaseSpeed() {
    if (update_interval > Consts::MAX_SPEED) {
        update_interval -= Consts::SPEED_STEP;
        cout << "speed increased = " << update_interval << "s" << endl;
    }
}

void BaseCellAutomaton::decreaseSpeed() {
    if (update_interval < Consts::MIN_SPEED) {
        update_interval += Consts::SPEED_STEP;
        cout << "speed decreased = " << update_interval << "s" << endl;
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
            updateCellColors();
            cout << "step forward" << endl;
            break;
        default:
            break;
    }
}

void BaseCellAutomaton::run() {
    sf::RenderWindow window(sf::VideoMode({grid->getWidth(), grid->getHeight()}), "Cell Automaton");
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
            updateCellColors();
            last_update = now;
        }

        window.clear(background_color);
        drawGrid(window);
        drawCells(window);
        window.display();
    }
}