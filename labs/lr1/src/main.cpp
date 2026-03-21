#include <chrono>
#include <iostream>
#include <memory>

#include "solver.h"
#include "spacecraft_solver.h"

using namespace std;

int main() {
    cout << "sun, planet, sputnik, part 1" << endl;

    auto start = chrono::steady_clock::now();
    bool add_spaace_apparat = false;
    int num_steps = 1000000;
    int num_years = 1;
    unique_ptr<Solver> task1 = make_unique<Solver>(num_steps, num_years);
    task1->run_computations();
    auto end = chrono::steady_clock::now();

    chrono::duration<double, milli> duration = end - start;
    cout << "time of 1 part computations = " << duration.count() << endl;

    task1->save_to_csv("../labs/lr1/src/trajectory_full.csv");
    cout << "haha" << endl;
    system("python ../labs/lr1/src/main.py");

    cout << "space apparat part 2" << endl;

    auto start_2 = chrono::steady_clock::now();
    int num_steps_burn = 1000000;
    int num_steps_coast = 1000000;

    auto task2 = make_unique<SpaceCraftSolver>(
        &task1->get_trajectory(), task1->get_time_to_around(), num_steps_burn, num_steps_coast);

    bool use_parallel = true;
    task2->run_computations(use_parallel);

    auto end_2 = chrono::steady_clock::now();
    chrono::duration<double> total_time = end_2 - start_2;

    cout << "time of 2 part computations = " << total_time.count() << " s" << endl;
    system("python ../labs/lr1/src/part.py");

    return 0;
}