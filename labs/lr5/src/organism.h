#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "base.h"
#include "consts.h"

using namespace std;

struct OrganismData {
    int age;
    float energy;
    bool is_alive;
    int x, y;

    OrganismData() : age(0), energy(0), is_alive(false), x(0), y(0) {}
    OrganismData(float e, int a, bool alive, int pos_x, int pos_y)
        : energy(e), age(a), is_alive(alive), x(pos_x), y(pos_y) {}
};

class Organism : public BaseCellAutomaton {
   private:
    vector<vector<bool>> grid;
    vector<vector<bool>> next_grid;
    vector<vector<float>> nutrients;
    vector<vector<float>> next_nutrients;
    vector<vector<OrganismData>> organisms;
    vector<vector<OrganismData>> next_organisms;

    sf::Color organism_color_high;
    sf::Color organism_color_medium;
    sf::Color organism_color_low;

    unsigned int tick;
    bool vision;

    mt19937 rng;
    uniform_int_distribution<unsigned> pos_dist;

    vector<pair<int, int>> getEmptyNeighbors(int x, int y, const vector<vector<bool>>& grid);
    pair<int, int> chooseBestMove(const vector<pair<int, int>>& neighbors, int current_x,
                                  int current_y, bool smart);

    void updateNutrients();
    void updateOrganisms();
    void updateSingleOrganism(int x, int y);

   public:
    Organism(unsigned num_cells = OrganismsConsts::DEFAULT_NUM_CELLS,
             unsigned cell_size = OrganismsConsts::DEFAULT_CELL_SIZE);
    virtual ~Organism();

    void initGrid() override;
    void updateGrid(bool force_update) override;
    void drawCells(sf::RenderWindow& window) override;
    void handleKeyPress(const sf::Event::KeyPressed& key_event) override;
    void reset() override;

    void toggleVision();
    unsigned int getPopulationCount() const;
};