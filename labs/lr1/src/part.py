import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

craft_path = "../labs/lr1/src/trajectory_space_craft.csv"
planet_path = "../labs/lr1/src/trajectory_full.csv"

df_craft = pd.read_csv(craft_path)
df_planet = pd.read_csv(planet_path)

df_craft["x_planet"] = np.interp(
    df_craft["time"], df_planet["time"], df_planet["x_planet"]
)
df_craft["y_planet"] = np.interp(
    df_craft["time"], df_planet["time"], df_planet["y_planet"]
)

df_craft["x_spacecraft_rel"] = df_craft["x_spacecraft"] - df_craft["x_planet"]
df_craft["y_spacecraft_rel"] = df_craft["y_spacecraft"] - df_craft["y_planet"]
df_craft["x_sputnik_rel"] = df_craft["x_sputnik"] - df_craft["x_planet"]
df_craft["y_sputnik_rel"] = df_craft["y_sputnik"] - df_craft["y_planet"]


plt.figure(figsize=(10, 8))
plt.grid(True, alpha=0.3, linestyle="--", linewidth=0.5)
plt.xlabel("X")
plt.ylabel("Y")
plt.plot(0, 0, "o", color="tan", markersize=12, label="planet")

plt.plot(
    df_craft["x_sputnik_rel"],
    df_craft["y_sputnik_rel"],
    color="blue",
    linewidth=1,
    label="sputnik",
)

plt.plot(
    df_craft["x_sputnik_rel"].iloc[0],
    df_craft["y_sputnik_rel"].iloc[0],
    "o",
    color="blue",
    label="start sputnik",
)

plt.plot(
    df_craft["x_spacecraft_rel"],
    df_craft["y_spacecraft_rel"],
    color="red",
    linewidth=1.5,
    label="space craft",
)

plt.plot(
    df_craft["x_spacecraft_rel"].iloc[0],
    df_craft["y_spacecraft_rel"].iloc[0],
    "p",
    color="red",
    markersize=10,
    label="start craft",
)

T_BURN = 5100.0
burn_end_idx = (df_craft["time"] <= T_BURN).sum() - 1
if burn_end_idx > 0 and burn_end_idx < len(df_craft):
    plt.plot(
        df_craft["x_spacecraft_rel"].iloc[burn_end_idx],
        df_craft["y_spacecraft_rel"].iloc[burn_end_idx],
        "o",
        color="red",
        markersize=10,
        label="end fuel",
    )

plt.plot(
    df_craft["x_spacecraft_rel"].iloc[-1],
    df_craft["y_spacecraft_rel"].iloc[-1],
    "*",
    color="yellow",
    markersize=20,
    label="boom",
    markeredgecolor="black",
)

plt.legend(loc="upper left")

plt.tight_layout()
plt.show()
