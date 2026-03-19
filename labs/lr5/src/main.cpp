#include <iostream>
#include <memory>

#include "live.h"
#include "neural.h"
#include "organism.h"

using namespace std;

void task1() {
    auto live = make_unique<GameLive>();  // default parameters from consts LiveGameConsts
    live->run();
}

void task2() {
    auto neural_network =
        make_unique<NeuralNetwork>();  // default parameters from consts LiveGameConsts
    neural_network->run();
}

void task3() {
    cout << "task 3" << endl;
    auto organism = make_unique<Organism>();  // default parameters from consts LiveGameConsts
    organism->run();
}

int main() {
    unsigned task;
    bool running = true;

    while (running) {
        cout << "Choose:" << endl;
        cout << "1 - Game of Life" << endl;
        cout << "2 - Neural Network" << endl;
        cout << "3 - Organism" << endl;
        cout << "0 - Exit" << endl;
        cout << "Write number ";
        cin >> task;

        switch (task) {
            case 1:
                task1();
                break;

            case 2:
                task2();
                break;

            case 3:
                task3();
                break;

            case 0:
                running = false;
                break;

            default:
                break;
        }
    }

    return 0;
}