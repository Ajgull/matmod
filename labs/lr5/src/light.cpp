#include "light.h"

Light::Light(unsigned num_cells, unsigned cell_size)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      tick(0),
      accumulated_exposure(LightConsts::ACCUMULATED_EXPOSURE),
      pixel_mass_figure(LightConsts::PIXEL_MASS_FIGURE),
      pixel_mass_out_figure(LightConsts::PIXEL_MASS_OUT_FIGURE),
      glass_colors({50, 60, 70}),
      color_shift({0.02f, 0.0f, -0.04f}) {
    wave_height.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    wave_velocity.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    accumulated_light.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    pixel_mass.resize(num_cells, vector<float>(num_cells, 1.0f));

    setGridColor(Consts::BACKGROUND_COLOR);
    initGrid();

    cout << "Light init" << endl;
}

Light::~Light() { cout << "Light destroyed" << endl; }

void Light::addPointSource(int x, int y, float amplitude) {
    if (x >= 0 && x < num_cells && y >= 0 && y < num_cells) {
        float value = amplitude;
        for (int k = 0; k < 3; k++) {
            wave_height[x][y][k] = value;
        }
    }
}

void Light::addPulsingSource(int x, int y, float frequency, float amplitude) {
    if (tick % 60 < 30) {
        float value = std::sin(tick * frequency) * amplitude;
        for (int k = 0; k < 3; k++) {
            wave_height[x][y][k] = value;
        }
    }
}

void Light::initGrid() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_height[x][y][k] = 0.0f;
                wave_velocity[x][y][k] = 0.0f;
                accumulated_light[x][y][k] = 0.0f;
            }
            pixel_mass[x][y] = pixel_mass_out_figure;
        }
    }

    initCircle();

    // initDiagonalBoundary(10, 145, num_cells - 15);
}

void Light::initCircle() {
    float center_x = num_cells / 2.0f;
    float center_y = num_cells / 2.0f;
    float radius = 100.0f / cell_size;

    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            float dx = x - center_x;
            float dy = y - center_y;
            if (sqrt(dx * dx + dy * dy) < radius) {
                pixel_mass[x][y] = pixel_mass_figure;
            }
        }
    }
}

void Light::initDiagonalBoundary(unsigned boundary_width, int angle_degrees, int start_y) {
    float theta = angle_degrees * M_PI / 180.0f;
    int start_x = 0;

    float sin_t = sin(theta);
    float cos_t = cos(theta);

    float half_width = static_cast<float>(boundary_width) / 2.0f;

    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            float dist = abs(sin_t * (static_cast<float>(x) - start_x) -
                             cos_t * (static_cast<float>(y) - start_y));

            if (dist <= half_width) {
                pixel_mass[x][y] = pixel_mass_figure;
            }
        }
    }
}

void Light::generateWaves() {
    int source_x = num_cells / 4;
    int start_y = num_cells / 2 - 50 / cell_size;
    int end_y = num_cells / 2;

    for (int y = start_y; y < end_y; y++) {
        if (y >= 0 && y < num_cells && source_x >= 0 && source_x < num_cells) {
            float value = sin(static_cast<float>(tick) * LightConsts::WAVE_FREQUENCY) *
                          LightConsts::WAVE_AMPLITUDE;

            for (int k = 0; k < 3; k++) {
                wave_height[source_x][y][k] = value;
            }
        }
    }
}

void Light::updatePhysics() {
    for (unsigned x = 1; x < num_cells - 1; x++) {
        for (unsigned y = 1; y < num_cells - 1; y++) {
            for (int k = 0; k < 3; k++) {
                float speed = pixel_mass[x][y] - color_shift[k];

                float force = wave_height[x - 1][y][k] + wave_height[x + 1][y][k] +
                              wave_height[x][y - 1][k] + wave_height[x][y + 1][k];

                wave_velocity[x][y][k] += (force / 4.0f - wave_height[x][y][k]) * speed;
            }
        }
    }
}

void Light::updateAccumulation() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_height[x][y][k] += wave_velocity[x][y][k];

                accumulated_light[x][y][k] += abs(wave_height[x][y][k]) * accumulated_exposure;
                if (accumulated_light[x][y][k] > 1.0f) {
                    accumulated_light[x][y][k] = 1.0f;
                }
            }
        }
    }
}

void Light::updateCellColors() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            sf::Color color;
            bool is_glass = (pixel_mass[x][y] < 1.0f);

            for (int k = 0; k < 3; k++) {
                float accumulated = min(accumulated_light[x][y][k], 1.0f);
                float color_value = pow(accumulated, 2.0f) * 255.0f;

                if (is_glass) {
                    color_value = min(color_value + static_cast<float>(glass_colors[k]), 255.0f);
                }

                uint8_t channel_value = static_cast<uint8_t>(min(max(color_value, 0.0f), 255.0f));

                switch (k) {
                    case 0:
                        color.r = channel_value;
                        break;
                    case 1:
                        color.g = channel_value;
                        break;
                    case 2:
                        color.b = channel_value;
                        break;
                }
            }
            cell_colors[y][x] = color;
        }
    }
}

void Light::updateGrid(bool force_update) {
    if (force_update || is_running) {
        generateWaves();

        // addPointSource(num_cells / 4, num_cells / 2, 15.0f);

        // addPulsingSource(num_cells / 3 + 10, num_cells / 2, LightConsts::WAVE_FREQUENCY,
        //                  LightConsts::WAVE_AMPLITUDE);

        updatePhysics();
        updateAccumulation();

        tick++;
    }
}

void Light::reset() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_height[x][y][k] = 0.0f;
                wave_velocity[x][y][k] = 0.0f;
                accumulated_light[x][y][k] = 0.0f;
            }
            pixel_mass[x][y] = pixel_mass_out_figure;
        }
    }

    tick = 0;
    is_running = false;

    initGrid();
    updateCellColors();
}

void Light::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    BaseCellAutomaton::handleKeyPress(key_event);

    switch (key_event.scancode) {
        case sf::Keyboard::Scan::R:
            reset();
            break;
        case sf::Keyboard::Scan::G: {
            static bool grid_visible = false;
            if (grid_visible) {
                setGridColor(Consts::BACKGROUND_COLOR);
            } else {
                setGridColor(Consts::GRID_COLOR);
            }
            grid_visible = !grid_visible;
        } break;
        default:
            break;
    }
}
