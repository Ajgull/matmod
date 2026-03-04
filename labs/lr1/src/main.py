import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("../labs/lr1/src/trajectory_full.csv", delimiter=",")

x_planet = data["x_planet"]
y_planet = data["y_planet"]
x_sputnik = data["x_sputnik"]
y_sputnik = data["y_sputnik"]

plt.figure(figsize=(10, 10))

plt.plot(x_planet, y_planet, "b-", linewidth=1.5, label="Planet", alpha=0.8)
plt.plot(x_sputnik, y_sputnik, "r-", linewidth=1.0, label="Sputnik", alpha=0.7)

plt.grid(True)
plt.xlabel("X")
plt.ylabel("Y")
plt.legend(loc="best", fontsize=10, framealpha=0.9)
plt.show()
