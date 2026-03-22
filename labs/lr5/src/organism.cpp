#include "organism.h"

Organism::Organism(unsigned num_cells, unsigned cell_size)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      organism_color_high(OrganismsConsts::ORGANISM_COLOR_HIGH),
      organism_color_medium(OrganismsConsts::ORGANISM_COLOR_MEDIUM),
      organism_color_low(OrganismsConsts::ORGANISM_COLOR_LOW),
      tick(0),
      vision(false),
      rng(random_device{}()),
      pos_dist(0, num_cells - 1) {
    grid.resize(num_cells, vector<bool>(num_cells, false));
    next_grid.resize(num_cells, vector<bool>(num_cells, false));
    nutrients.resize(num_cells, vector<float>(num_cells, OrganismsConsts::P_MAX));
    next_nutrients.resize(num_cells, vector<float>(num_cells, OrganismsConsts::P_MAX));
    organisms.resize(num_cells, vector<OrganismData>(num_cells));
    next_organisms.resize(num_cells, vector<OrganismData>(num_cells));

    cout << "Organisms init" << endl;
    initRendering();
    initGrid();
}

Organism::~Organism() { cout << "Organism destroyed" << endl; }

void Organism::initGrid() {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            grid[y][x] = false;
            nutrients[y][x] = OrganismsConsts::P_MAX;
            organisms[y][x] = OrganismData(0, 0, false, x, y);
        }
    }

    unsigned total_cells = num_cells * num_cells;
    unsigned max_organisms = (total_cells * OrganismsConsts::A) / 100;
    unsigned placed = 0;

    // cout << "Initializing " << max_organisms << " organisms out of " << total_cells << " cells"
    //      << endl;

    while (placed < max_organisms) {
        int x = pos_dist(rng);
        int y = pos_dist(rng);

        if (!grid[y][x]) {
            grid[y][x] = true;
            int initial_age = uniform_int_distribution<int>(0, OrganismsConsts::T)(rng);
            float initial_energy = uniform_real_distribution<float>(OrganismsConsts::DELTA_E * 5,
                                                                    OrganismsConsts::P1)(rng);

            organisms[y][x] = OrganismData(initial_energy, initial_age, true, x, y);
            placed++;
        }
    }

    tick = 0;
}

void Organism::updateGrid(bool force_update) {
    if (!is_running && !force_update) {
        return;
    }

    updateNutrients();
    updateOrganisms();

    grid.swap(next_grid);
    nutrients.swap(next_nutrients);
    organisms.swap(next_organisms);

    tick++;

    if (tick % 10 == 0) {
        cout << "tick: " << tick << ", population: " << getPopulationCount() << endl;
    }
}

void Organism::updateNutrients() {
    // rule 4
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (nutrients[y][x] < OrganismsConsts::P_MAX) {
                nutrients[y][x] = min(OrganismsConsts::P_MAX, nutrients[y][x] + OrganismsConsts::R);
            }
        }
    }
}

void Organism::updateOrganisms() {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            next_grid[y][x] = false;
            next_nutrients[y][x] = nutrients[y][x];
            next_organisms[y][x] = OrganismData(0, 0, false, x, y);
        }
    }

    vector<pair<int, int>> organisms_positions;
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x] && organisms[y][x].is_alive) {
                organisms_positions.push_back({x, y});
            }
        }
    }

    vector<bool> processed(organisms_positions.size(), false);
    int remaining = organisms_positions.size();

    while (remaining > 0) {
        int idx = uniform_int_distribution<int>(0, organisms_positions.size() - 1)(rng);
        if (!processed[idx]) {
            processed[idx] = true;
            remaining--;
            updateSingleOrganism(organisms_positions[idx].first, organisms_positions[idx].second);
        }
    }
}

void Organism::updateSingleOrganism(int x, int y) {
    if (!grid[y][x] || !organisms[y][x].is_alive) return;

    OrganismData org = organisms[y][x];

    float current_nutrient = nutrients[y][x];

    // rule 7 - 8
    float can_take = min(OrganismsConsts::DELTA_P, OrganismsConsts::P1 - org.energy);
    float actual_take = min(current_nutrient, can_take);

    if (actual_take > 0) {
        next_nutrients[y][x] = max(0.0f, next_nutrients[y][x] - actual_take);
        org.energy += actual_take;
    }

    if (org.energy > OrganismsConsts::P1) {
        org.energy = OrganismsConsts::P1;
    }

    // rule 9
    org.energy -= OrganismsConsts::DELTA_E;

    // rule 11
    org.age++;

    // rule 12
    if (org.age > OrganismsConsts::L || org.energy <= 0) {
        return;
    }

    vector<pair<int, int>> empty_neighbors = getEmptyNeighbors(x, y, next_grid);

    // rule 13
    if (org.age >= OrganismsConsts::T && org.energy > OrganismsConsts::DELTA_R &&
        !empty_neighbors.empty()) {
        pair<int, int> target = chooseBestMove(empty_neighbors, x, y, vision);

        if (target.first != -1) {
            org.energy -= OrganismsConsts::DELTA_R;

            if (org.energy < 0) org.energy = 0;

            float child_energy = org.energy / 2.0f;
            org.energy -= child_energy;

            OrganismData child(max(0.0f, child_energy), 0, true, x, y);

            org.x = target.first;
            org.y = target.second;

            next_grid[target.second][target.first] = true;
            next_organisms[target.second][target.first] = org;
            next_grid[y][x] = true;
            next_organisms[y][x] = child;

            return;
        }
    }

    // rule 10
    if (!empty_neighbors.empty()) {
        pair<int, int> target = chooseBestMove(empty_neighbors, x, y, vision);

        if (target.first != -1) {
            org.x = target.first;
            org.y = target.second;
            next_grid[target.second][target.first] = true;
            next_organisms[target.second][target.first] = org;
            return;
        }
    }

    if (!next_grid[y][x]) {
        next_grid[y][x] = true;
        next_organisms[y][x] = org;
    }
}

vector<pair<int, int>> Organism::getEmptyNeighbors(int x, int y, const vector<vector<bool>>& grid) {
    vector<pair<int, int>> empty;
    empty.reserve(8);

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < static_cast<int>(num_cells) && ny >= 0 &&
                ny < static_cast<int>(num_cells) && !grid[ny][nx]) {
                empty.push_back({nx, ny});
            }
        }
    }

    return empty;
}

pair<int, int> Organism::chooseBestMove(const vector<pair<int, int>>& neighbors, int current_x,
                                        int current_y, bool smart) {
    if (neighbors.empty()) {
        return {-1, -1};
    }

    if (!smart) {
        // task
        int idx = uniform_int_distribution<int>(0, neighbors.size() - 1)(rng);
        return neighbors[idx];
    }

    // task 4
    float max_nutrient = -1;
    vector<pair<int, int>> best_cells;

    for (const auto& pos : neighbors) {
        float nutrient_level = nutrients[pos.second][pos.first];
        if (nutrient_level > max_nutrient) {
            max_nutrient = nutrient_level;
            best_cells.clear();
            best_cells.push_back(pos);
        } else if (abs(nutrient_level - max_nutrient) < 1e-6) {
            best_cells.push_back(pos);
        }
    }

    // Check if moving is beneficial
    float current_nutrient = nutrients[current_y][current_x];
    if (max_nutrient <= current_nutrient) {
        return {-1, -1};
    }

    // Randomly choose among best cells
    int idx = uniform_int_distribution<int>(0, best_cells.size() - 1)(rng);
    return best_cells[idx];
}

void Organism::drawCells(sf::RenderWindow& window) {
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x] && organisms[y][x].is_alive) {
                float energy_ratio = organisms[y][x].energy / OrganismsConsts::P1;

                sf::Color org_color;
                if (energy_ratio > 0.7f) {
                    org_color = organism_color_high;
                } else if (energy_ratio > 0.3f) {
                    float t = (energy_ratio - 0.3f) / 0.4f;
                    uint8_t r = organism_color_medium.r +
                                t * (organism_color_high.r - organism_color_medium.r);
                    uint8_t g = organism_color_medium.g +
                                t * (organism_color_high.g - organism_color_medium.g);
                    uint8_t b = organism_color_medium.b +
                                t * (organism_color_high.b - organism_color_medium.b);
                    org_color = sf::Color(r, g, b);
                } else {
                    float t = energy_ratio / 0.3f;
                    uint8_t r =
                        organism_color_low.r + t * (organism_color_medium.r - organism_color_low.r);
                    uint8_t g =
                        organism_color_low.g + t * (organism_color_medium.g - organism_color_low.g);
                    uint8_t b =
                        organism_color_low.b + t * (organism_color_medium.b - organism_color_low.b);
                    org_color = sf::Color(r, g, b);
                }

                cell_shapes[y][x].setFillColor(org_color);
                window.draw(cell_shapes[y][x]);
            } else {
                float nutrient_ratio = nutrients[y][x] / OrganismsConsts::P_MAX;
                uint8_t intensity = static_cast<uint8_t>(50 + nutrient_ratio * 205);
                cell_shapes[y][x].setFillColor(sf::Color(0, intensity, 0));
                window.draw(cell_shapes[y][x]);
            }
        }
    }
}

void Organism::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    BaseCellAutomaton::handleKeyPress(key_event);

    switch (key_event.scancode) {
        case sf::Keyboard::Scan::R:
            reset();
            break;
        case sf::Keyboard::Scan::V:
            toggleVision();
            break;
        default:
            break;
    }
}

void Organism::reset() {
    initGrid();
    tick = 0;
    is_running = false;
    cout << "Organism Simulation reset" << endl;
}

void Organism::toggleVision() {
    vision = !vision;
    cout << "vision " << vision << endl;
}

unsigned int Organism::getPopulationCount() const {
    unsigned count = 0;
    for (unsigned y = 0; y < num_cells; y++) {
        for (unsigned x = 0; x < num_cells; x++) {
            if (grid[y][x] && organisms[y][x].is_alive) {
                count++;
            }
        }
    }
    return count;
}