import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("../labs/lr1/trajectory_space_craft.csv", comment="#")

plt.figure(figsize=(10, 10))
plt.plot(df["x_ship"], df["y_ship"], "r", label="Rocket", linewidth=5.0)
plt.plot(df["x_sputnik"], df["y_sputnik"], "b", label="Sputnik", linewidth=5.0)
# plt.plot(df["x_planet"], df["y_planet"], "g", label="Planet", linewidth=5.0)
plt.xlabel("X")
plt.ylabel("Y")
plt.title("Trajectory in XY plane")
plt.legend()
plt.grid(True)
plt.show()
