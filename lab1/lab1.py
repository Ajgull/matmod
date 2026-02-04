import numpy as np
import pygame as pg
from pygame.locals import QUIT

num_cel = 32
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)


class LiveGame:
    def __init__(
        self,
        num_cel: int = 32,
        speed: int = 10,
        cell_size: int = 20,
        pattern: str = "rand",
    ) -> None:
        self.cell_size = cell_size
        self.speed = speed
        self.num_cel = num_cel
        self.pattern = pattern
        self.width = self.num_cel * self.cell_size
        self.screen_size = (
            self.width,
            self.width,
        )
        self.list_of_cells = None
        self.screen = None
        self.iter = None
        self._load_pattern()

    def _load_pattern(self) -> None:
        patterns = {
            "block": [(0, 0), (0, 1), (1, 0), (1, 1)],
            "beehive": [(0, 1), (0, 2), (1, 0), (1, 3), (2, 1), (2, 2)],
            "loaf": [(0, 1), (0, 2), (1, 0), (1, 3), (2, 1), (2, 3), (3, 2)],
            "blinker": [(0, 1), (1, 1), (2, 1)],
            "toad": [(1, 0), (2, 0), (3, 0), (0, 1), (1, 1), (2, 1)],
            "pulsar": [
                (2, 4),
                (2, 5),
                (2, 6),
                (2, 10),
                (2, 11),
                (2, 12),
                (4, 2),
                (4, 7),
                (4, 9),
                (4, 14),
                (5, 2),
                (5, 7),
                (5, 9),
                (5, 14),
                (6, 2),
                (6, 7),
                (6, 9),
                (6, 14),
                (7, 4),
                (7, 5),
                (7, 6),
                (7, 10),
                (7, 11),
                (7, 12),
                (9, 4),
                (9, 5),
                (9, 6),
                (9, 10),
                (9, 11),
                (9, 12),
                (10, 2),
                (10, 7),
                (10, 9),
                (10, 14),
                (11, 2),
                (11, 7),
                (11, 9),
                (11, 14),
                (12, 2),
                (12, 7),
                (12, 9),
                (12, 14),
                (14, 4),
                (14, 5),
                (14, 6),
                (14, 10),
                (14, 11),
                (14, 12),
            ],
        }

        if self.pattern not in patterns:
            self.list_of_cells = np.random.randint(
                0, 2, size=(self.num_cel, self.num_cel)
            )
            return

        self.list_of_cells = np.zeros((self.num_cel, self.num_cel), dtype=int)

        coords = patterns[self.pattern]
        max_x = max(x for x, y in coords)
        max_y = max(y for x, y in coords)
        center_x = (self.num_cel - max_x - 1) // 2
        center_y = (self.num_cel - max_y - 1) // 2

        for x, y in coords:
            self.list_of_cells[center_x + x][center_y + y] = 1

    def _draw_grid(self) -> None:
        for x in range(0, self.width, self.cell_size):
            pg.draw.line(self.screen, BLACK, (x, 0), (x, self.width))

        for y in range(0, self.width, self.cell_size):
            pg.draw.line(self.screen, BLACK, (0, y), (self.width, y))

    def _draw_cell_list(self) -> None:
        for i in range(self.num_cel):
            for j in range(self.num_cel):
                color = BLACK if self.list_of_cells[i][j] == 1 else WHITE
                pg.draw.rect(
                    self.screen,
                    color,
                    pg.Rect(
                        i * self.cell_size,
                        j * self.cell_size,
                        self.cell_size,
                        self.cell_size,
                    ),
                )

    def _get_neighbours(self, x: int, y: int) -> int:
        count = 0
        for i in [-1, 0, 1]:
            for j in [-1, 0, 1]:
                if i == 0 and j == 0:
                    continue
                if 0 <= x + i < self.num_cel and 0 <= y + j < self.num_cel:
                    count += self.list_of_cells[x + i][y + j]
        return count

    def _update_cell_list(self) -> None:
        new_cell_list = np.zeros_like(self.list_of_cells)

        for i in range(self.num_cel):
            for j in range(self.num_cel):
                num = self._get_neighbours(i, j)
                if self.list_of_cells[i][j] == 1:
                    if num in (2, 3):
                        new_cell_list[i][j] = 1
                elif num == 3:
                    new_cell_list[i][j] = 1
        self.list_of_cells = new_cell_list

    def run(self) -> None:
        pg.init()
        self.screen = pg.display.set_mode(self.screen_size)
        clock = pg.time.Clock()
        self.iter = 0
        self.screen.fill(WHITE)
        running = True
        # iter_list = [10, 11, 50]

        while running:
            for event in pg.event.get():
                if event.type == QUIT:
                    running = False
            self.screen.fill(WHITE)
            self._draw_grid()
            self._draw_cell_list()
            pg.display.flip()
            # if self.iter in iter_list:
            #     from PIL import Image
            #     arr = pg.surfarray.array3d(self.screen)
            #     arr = np.transpose(arr, axes=(1, 0, 2))
            #     img = Image.fromarray(arr, "RGB")
            #     img.save(f"live_game_{self.iter:04d}.png")
            print(self.iter)
            clock.tick(self.speed)
            self._update_cell_list()
            self.iter += 1

        pg.quit()


patterns = [
    "rand",  # случайно
    "block",  # блок статично
    "loaf",  # каравай статично
    "beehive",  # улей статично
    "blinker",  # семафор цикл
    "toad",  # жаба цикл
    "pulsar",  # нечто большое и цикличное
]

if __name__ == "__main__":
    game = LiveGame(pattern="pulsar", speed=7)
    game.run()
