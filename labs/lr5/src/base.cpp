#include "base.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <thread>

BaseCellAutomaton::BaseCellAutomaton(unsigned num_cells, unsigned cell_size,
                                     float default_update_interval)
    : num_cells(num_cells),
      cell_size(cell_size),
      is_running(false),
      update_interval(default_update_interval),
      background_color(Consts::BACKGROUND_COLOR),
      grid_color(Consts::GRID_COLOR),
      recording(false),
      recording_requested(false),
      frame_counter(0),
      recording_start_frame(0) {
    grid = make_unique<Grid>(num_cells, cell_size, grid_color);
    grid->init();
    cell_colors.resize(num_cells, vector<sf::Color>(num_cells, Consts::BACKGROUND_COLOR));

    cout << "BaseCellAutomaton init" << endl;
    last_update = chrono::steady_clock::now();

    if (!filesystem::exists("temp_frames")) {
        filesystem::create_directory("temp_frames");
    }
}

BaseCellAutomaton::~BaseCellAutomaton() {
    cout << "BaseCellAutomaton destroyed" << endl;
    if (recording) {
        stopRecording();
    }
    cleanupTempFiles();
}

void BaseCellAutomaton::startRecording(const string& filename) {
    if (recording) {
        cout << "Already recording!" << endl;
        return;
    }

    if (!is_running) {
        recording_requested = true;
        return;
    }

    recording_filename = filename;
    recording = true;
    recording_requested = false;
    frame_counter = 0;
    recording_start_frame = 0;
    recorded_frames.clear();
    cout << "recording started" << endl;
}

void BaseCellAutomaton::stopRecording() {
    if (!recording) {
        if (recording_requested) {
            recording_requested = false;
        }
        return;
    }

    recording = false;
    cout << "recording stopped. " << recorded_frames.size() << " frames captured." << endl;
    cout << "converting to gif" << endl;
    convertToGIF();
}

void BaseCellAutomaton::saveFrame(sf::RenderWindow& window) {
    if (!recording) {
        return;
    }
    if (!is_running) {
        return;
    }

    sf::Texture texture;
    if (!texture.resize({window.getSize().x, window.getSize().y})) {
        return;
    }

    texture.update(window);

    stringstream ss;
    ss << "temp_frames/frame_" << setw(6) << setfill('0') << frame_counter << ".png";
    string filename = ss.str();

    sf::Image screenshot = texture.copyToImage();
    if (!screenshot.saveToFile(filename)) {
        return;
    }

    recorded_frames.push_back(filename);
    frame_counter++;

    if (frame_counter % 30 == 0) {
        cout << "recorded " << frame_counter << " frames" << endl;
    }
}

void BaseCellAutomaton::convertToGIF() {
    if (recorded_frames.empty()) {
        cout << "No frames recorded!" << endl;
        return;
    }

    string command = "magick -delay 5 -loop 0 ";

    for (const auto& frame : recorded_frames) {
        command += frame + " ";
    }

    command += recording_filename + ".gif";

    // cout << "running: " << command << endl;
    int result = system(command.c_str());

    if (result == 0) {
        cout << "gif saved " << recording_filename << ".gif" << endl;
        cout << "total frames: " << recorded_frames.size() << endl;

        cleanupTempFiles();
    }
}

void BaseCellAutomaton::cleanupTempFiles() {
    for (const auto& frame : recorded_frames) {
        filesystem::remove(frame);
    }
    recorded_frames.clear();

    filesystem::remove("temp_frames");
}

void BaseCellAutomaton::toggleRecording() {
    if (recording) {
        stopRecording();
    } else {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << "animation_" << time_t;
        startRecording(ss.str());
    }
}

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

void BaseCellAutomaton::setGridColor(const sf::Color& color) {
    grid_color = color;
    grid = make_unique<Grid>(num_cells, cell_size, grid_color);
    grid->init();
    cout << "grid color changed " << endl;
}

void BaseCellAutomaton::setBackgroundColor(const sf::Color& color) {
    background_color = color;
    cout << "background color changed " << endl;
}

void BaseCellAutomaton::handleKeyPress(const sf::Event::KeyPressed& key_event) {
    switch (key_event.scancode) {
        case sf::Keyboard::Scan::Escape:
            break;
        case sf::Keyboard::Scan::Space:
            is_running = !is_running;
            if (is_running && recording_requested) {
                startRecording(recording_filename.empty() ? "animation" : recording_filename);
            }
            cout << (is_running ? "run" : "pause") << endl;
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
        case sf::Keyboard::Scan::S:  // start/stop recording
            toggleRecording();
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

        if (recording && is_running) {
            saveFrame(window);
        }
    }
}