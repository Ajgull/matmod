import numpy as np
from consts import *


class Task1:
    def __init__(self) -> None:
        self.sun_initial = np.array([0.0, 0.0])
        self.palnet_initial = np.array([R_SUN_PLANET, 0.0])
        self.sputnik_initial = np.array([R_SUN_PLANET + R_PLANET_SPUTNIK, 0.0])

        self.v_planet = np.array([0, V_PLANET])
        self.v_sputnik = np.array([0, V_SPUTNIK])

        self.time_end = 2 * np.pi * np.sqrt(R_SUN_PLANET**3 / (G * M_SUN))

    def equations(self):
        print(self.time_end)

    def run(self):
        pass


if __name__ == "__main__":
    print("star, planet, sputnik part 1")

    task = Task1()
    task.equations()
