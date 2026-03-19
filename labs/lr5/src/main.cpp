#include <memory>

#include "live.h"

using namespace std;

void task1() {
    auto game = make_unique<GameLive>();  // default parameters from consts LiveGameConsts
    game->run();
}

int main() {
    task1();
    return 0;
}