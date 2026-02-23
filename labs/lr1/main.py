import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("../labs/lr1/trajectory_full.csv", delimiter=",", skiprows=1)

time = data[:, 0]
x_planet = data[:, 1]
y_planet = data[:, 2]
x_sputnik = data[:, 3]
y_sputnik = data[:, 4]

plt.figure(figsize=(10, 10))

plt.plot(x_planet, y_planet, "b-", linewidth=1.5, label="Планета", alpha=0.8)
plt.plot(x_sputnik, y_sputnik, "r-", linewidth=1.0, label="Спутник", alpha=0.7)

plt.grid(True)
plt.xlabel("X")
plt.ylabel("Y")
plt.legend(loc="best", fontsize=10, framealpha=0.9)
plt.show()
