#include <memory>

#include "consts.h"
#include "live.h"

using namespace std;

void task1() {
    auto game =
        make_unique<GameLive>(LiveGameConsts::DEFAULT_NUM_CELLS, LiveGameConsts::DEFAULT_CELL_SIZE,
                              get<0>(LiveGameConsts::PATTERNS[0]));  // random
    game->run();
}

int main() {
    task1();
    return 0;
}