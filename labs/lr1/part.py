import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def load_trajectory(filepath):
    df = pd.read_csv(filepath, skipinitialspace=True)
    df.columns = df.columns.str.strip()
    return df


def load_planet_trajectory(filepath):
    df = pd.read_csv(filepath, skipinitialspace=True)
    df.columns = df.columns.str.strip()
    return df


def convert_to_planet_frame(df_craft, df_planet):
    df = df_craft.copy()

    df["x_planet"] = np.interp(df["time"], df_planet["time"], df_planet["x_planet"])
    df["y_planet"] = np.interp(df["time"], df_planet["time"], df_planet["y_planet"])

    df["x_spacecraft_rel"] = df["x_spacecraft"] - df["x_planet"]
    df["y_spacecraft_rel"] = df["y_spacecraft"] - df["y_planet"]
    df["x_sputnik_rel"] = df["x_sputnik"] - df["x_planet"]
    df["y_sputnik_rel"] = df["y_sputnik"] - df["y_planet"]

    return df


def plot_trajectories(df_craft, df_planet):
    df = convert_to_planet_frame(df_craft, df_planet)
    # plt.figure(figsize=(10, 8))
    plt.grid(True, alpha=0.3, linestyle="--", linewidth=0.5)
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.plot(0, 0, "o", color="#8B7355", markersize=12, label="Планета")

    plt.plot(
        df["x_sputnik_rel"],
        df["y_sputnik_rel"],
        color="#4169E1",
        linewidth=1,
        label="Спутник",
    )

    plt.plot(
        df["x_sputnik_rel"].iloc[0],
        df["y_sputnik_rel"].iloc[0],
        "o",
        color="#4169E1",
    )

    craft_x = df["x_spacecraft_rel"]
    craft_y = df["y_spacecraft_rel"]

    plt.plot(craft_x, craft_y, color="#DC143C", linewidth=1.5, label="Space craft")

    plt.plot(
        df["x_spacecraft_rel"].iloc[0],
        df["y_spacecraft_rel"].iloc[0],
        "s",
        color="red",
        markersize=8,
        label="Старт ракеты",
    )

    T_BURN = 5100.0
    burn_end_idx = (df["time"] <= T_BURN).sum() - 1
    if burn_end_idx > 0 and burn_end_idx < len(df):
        plt.plot(
            df["x_spacecraft_rel"].iloc[burn_end_idx],
            df["y_spacecraft_rel"].iloc[burn_end_idx],
            "*",
            color="#FFD700",
            markersize=10,
            label="Отделение груза",
        )

    last = df.iloc[-1]

    plt.plot(
        last["x_spacecraft_rel"],
        last["y_spacecraft_rel"],
        "*",
        color="#FFD700",
        markersize=25,
        label="Boom!",
        zorder=7,
        markeredgecolor="black",
        markeredgewidth=1.5,
    )

    plt.legend(loc="upper right")

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    craft_path = "../labs/lr1/trajectory_space_craft.csv"
    planet_path = "../labs/lr1/trajectory_full.csv"

    df_craft = load_trajectory(craft_path)
    df_planet = load_planet_trajectory(planet_path)
    plot_trajectories(df_craft, df_planet)
