#include "light.h"

Light::Light(unsigned num_cells, unsigned cell_size)
    : BaseCellAutomaton(num_cells, cell_size, Consts::DEFAULT_UPDATE_INTERVAL),
      current_frame(0),
      accumulated_exposure(LightConsts::ACCUMULATED_EXPOSURE),
      glass_colors({50, 60, 70}),
      color_shift({0.02f, 0.0f, -0.04f}) {
    wave_height.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    wave_velocity.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    accumulated_light.resize(num_cells, vector<vector<float>>(num_cells, vector<float>(3, 0.0f)));
    pixel_mass.resize(num_cells, vector<float>(num_cells, 1.0f));

    setGridColor(Consts::BACKGROUND_COLOR);

    cout << "Light init" << endl;
    cout << "Controls: SPACE - Start/Pause, R - Reset, G - Toggle grid" << endl;
    cout << "          1/2 - Change wave speed" << endl;
}

Light::~Light() { cout << "Light destroyed" << endl; }

void Light::initGrid() {
    // Очищаем все массивы
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_height[x][y][k] = 0.0f;
                wave_velocity[x][y][k] = 0.0f;
                accumulated_light[x][y][k] = 0.0f;
            }
            pixel_mass[x][y] = 1.0f;
        }
    }

    // Создаем стеклянную линзу в центре
    float center_x = num_cells / 2.0f;
    float center_y = num_cells / 2.0f;
    float radius = 100.0f / cell_size;  // 100 пикселей в клетках

    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            float dx = x - center_x;
            float dy = y - center_y;
            if (std::sqrt(dx * dx + dy * dy) < radius) {
                pixel_mass[x][y] = 3.0f / 4.0f;  // Стекло
            }
        }
    }

    cout << "Grid initialized with glass lens at center" << endl;
}

void Light::generateWaves() {
    if (current_frame < LightConsts::WAVE_GENERATION_DURATION) {
        int source_x = LightConsts::SOURCE_X;
        int start_y = (num_cells / 2) - (50 / cell_size);
        int end_y = num_cells / 2;

        for (int y = start_y; y < end_y; y++) {
            int y_idx = y - (50 / cell_size);
            if (y_idx >= 0 && y_idx < static_cast<int>(num_cells) && source_x >= 0 &&
                source_x < static_cast<int>(num_cells)) {
                float value =
                    std::sin(static_cast<float>(current_frame) * LightConsts::WAVE_FREQUENCY) *
                    LightConsts::WAVE_AMPLITUDE;

                for (int k = 0; k < 3; k++) {
                    wave_height[source_x][y_idx][k] = value;
                }
            }
        }
    }
}

void Light::updatePhysics() {
    for (unsigned x = 1; x < num_cells - 1; x++) {
        for (unsigned y = 1; y < num_cells - 1; y++) {
            for (int k = 0; k < 3; k++) {
                float speed = pixel_mass[x][y] - color_shift[k];

                // Сумма соседних клеток
                float force = wave_height[x - 1][y][k] + wave_height[x + 1][y][k] +
                              wave_height[x][y - 1][k] + wave_height[x][y + 1][k];

                // Обновление скорости и высоты волны
                wave_velocity[x][y][k] += (force / 4.0f - wave_height[x][y][k]) * speed;
                wave_height[x][y][k] += wave_velocity[x][y][k];
            }
        }
    }

    // Небольшое затухание
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_velocity[x][y][k] *= 0.995f;
            }
        }
    }
}

void Light::updateAccumulation() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                accumulated_light[x][y][k] += std::abs(wave_height[x][y][k]) * accumulated_exposure;
                if (accumulated_light[x][y][k] > 1.0f) {
                    accumulated_light[x][y][k] = 1.0f;
                }
            }
        }
    }
}

void Light::updateGrid(bool force_update) {
    if (force_update || is_running) {
        generateWaves();
        updatePhysics();
        updateAccumulation();

        current_frame++;
    }
}

void Light::updateCellColors() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            sf::Color color;
            bool is_glass = (pixel_mass[x][y] < 0.9f);

            for (int k = 0; k < 3; k++) {
                // Цветовое значение
                float color_value =
                    std::pow(std::min(accumulated_light[x][y][k], 1.0f), 2.0f) * 255.0f;

                if (is_glass) {
                    color_value = std::min(color_value + glass_colors[k], 255.0f);
                }

                switch (k) {
                    case 0:
                        color.r = static_cast<uint8_t>(color_value);
                        break;
                    case 1:
                        color.g = static_cast<uint8_t>(color_value);
                        break;
                    case 2:
                        color.b = static_cast<uint8_t>(color_value);
                        break;
                }
            }

            cell_colors[y][x] = color;
        }
    }
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

void Light::reset() {
    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            for (int k = 0; k < 3; k++) {
                wave_height[x][y][k] = 0.0f;
                wave_velocity[x][y][k] = 0.0f;
                accumulated_light[x][y][k] = 0.0f;
            }
            pixel_mass[x][y] = 1.0f;
        }
    }

    float center_x = num_cells / 2.0f;
    float center_y = num_cells / 2.0f;
    float radius = 100.0f / cell_size;

    for (unsigned x = 0; x < num_cells; x++) {
        for (unsigned y = 0; y < num_cells; y++) {
            float dx = x - center_x;
            float dy = y - center_y;
            if (std::sqrt(dx * dx + dy * dy) < radius) {
                pixel_mass[x][y] = 3.0f / 4.0f;
            }
        }
    }

    current_frame = 0;
}