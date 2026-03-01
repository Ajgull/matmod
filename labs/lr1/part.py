# import matplotlib.pyplot as plt
# import numpy as np
# import pandas as pd

# # Загрузка данных
# data = pd.read_csv("../labs/lr1/trajectory_space_craft.csv")

# # Создание статичного графика
# plt.figure(figsize=(12, 10))

# # Цвета для разных тел
# colors = {"spacecraft": "red", "planet": "blue", "moon": "green"}

# # Построение траекторий
# plt.plot(
#     data["x_spacecraft"],
#     data["y_spacecraft"],
#     color=colors["spacecraft"],
#     linewidth=1.5,
#     label="Космический аппарат",
#     alpha=0.8,
# )

# plt.plot(
#     data["x_planet"],
#     data["y_planet"],
#     color=colors["planet"],
#     linewidth=2.5,
#     label="Планета",
#     alpha=0.7,
# )

# plt.plot(
#     data["x_moon"],
#     data["y_moon"],
#     color=colors["moon"],
#     linewidth=2,
#     label="Луна",
#     alpha=0.7,
# )

# # Отмечаем начальные точки
# plt.plot(
#     data["x_spacecraft"].iloc[0],
#     data["y_spacecraft"].iloc[0],
#     "o",
#     color=colors["spacecraft"],
#     markersize=10,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Старт КА",
# )

# plt.plot(
#     data["x_planet"].iloc[0],
#     data["y_planet"].iloc[0],
#     "o",
#     color=colors["planet"],
#     markersize=12,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Старт планеты",
# )

# plt.plot(
#     data["x_moon"].iloc[0],
#     data["y_moon"].iloc[0],
#     "o",
#     color=colors["moon"],
#     markersize=8,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Старт луны",
# )

# # Отмечаем конечные точки
# plt.plot(
#     data["x_spacecraft"].iloc[-1],
#     data["y_spacecraft"].iloc[-1],
#     "s",
#     color=colors["spacecraft"],
#     markersize=10,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Финиш КА",
# )

# plt.plot(
#     data["x_planet"].iloc[-1],
#     data["y_planet"].iloc[-1],
#     "s",
#     color=colors["planet"],
#     markersize=12,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Финиш планеты",
# )

# plt.plot(
#     data["x_moon"].iloc[-1],
#     data["y_moon"].iloc[-1],
#     "s",
#     color=colors["moon"],
#     markersize=8,
#     markeredgecolor="black",
#     markeredgewidth=1.5,
#     label="Финиш луны",
# )

# # Добавляем информацию о времени в нескольких точках
# num_time_points = 5
# time_indices = np.linspace(0, len(data) - 1, num_time_points, dtype=int)

# for idx in time_indices[1:-1]:  # пропускаем первую и последнюю (уже отмечены)
#     time_val = data.iloc[idx]["time"]
#     # Для КА
#     plt.annotate(
#         f"t={time_val:.1f}с",
#         xy=(data.iloc[idx]["x_spacecraft"], data.iloc[idx]["y_spacecraft"]),
#         xytext=(5, 5),
#         textcoords="offset points",
#         fontsize=8,
#         color=colors["spacecraft"],
#         bbox=dict(
#             boxstyle="round,pad=0.3",
#             facecolor="white",
#             alpha=0.7,
#             edgecolor=colors["spacecraft"],
#         ),
#     )

# # Настройка графика
# plt.xlabel("X координата (м)", fontsize=12)
# plt.ylabel("Y координата (м)", fontsize=12)
# plt.title(
#     "Траектории движения космического аппарата, планеты и луны",
#     fontsize=14,
#     fontweight="bold",
# )
# plt.grid(True, alpha=0.3, linestyle="--")
# plt.axis("equal")

# # Добавляем легенду
# plt.legend(loc="upper right", fontsize=10, framealpha=0.9)

# # Добавляем информацию о масштабе
# x_range = data["x_spacecraft"].max() - data["x_spacecraft"].min()
# y_range = data["y_spacecraft"].max() - data["y_spacecraft"].min()
# scale_text = f"Масштаб: 1:{x_range / 10:.0e} м"
# plt.text(
#     0.02,
#     0.02,
#     scale_text,
#     transform=plt.gca().transAxes,
#     fontsize=9,
#     bbox=dict(boxstyle="round", facecolor="wheat", alpha=0.5),
# )

# # Сохраняем график
# plt.tight_layout()
# plt.savefig("trajectories_static.png", dpi=300, bbox_inches="tight")

# print("Статичный график сохранен как trajectories_static.png")

# # Создаем дополнительный график с увеличением области столкновения/сближения
# # Находим момент минимального расстояния до луны
# data["dist_to_moon"] = np.sqrt(
#     (data["x_spacecraft"] - data["x_moon"]) ** 2
#     + (data["y_spacecraft"] - data["y_moon"]) ** 2
# )
# min_dist_idx = data["dist_to_moon"].idxmin()
# min_dist_time = data.iloc[min_dist_idx]["time"]
# min_dist_value = data.iloc[min_dist_idx]["dist_to_moon"]

# # График с увеличением
# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 7))

# # Общий вид
# ax1.plot(data["x_spacecraft"], data["y_spacecraft"], "r-", linewidth=1, label="КА")
# ax1.plot(data["x_planet"], data["y_planet"], "b-", linewidth=2, label="Планета")
# ax1.plot(data["x_moon"], data["y_moon"], "g-", linewidth=1.5, label="Луна")
# ax1.set_xlabel("X (м)")
# ax1.set_ylabel("Y (м)")
# ax1.set_title("Общий вид траекторий")
# ax1.grid(True, alpha=0.3)
# ax1.legend()
# ax1.axis("equal")

# # Увеличенная область
# zoom_margin = 2e7  # 20,000 км вокруг точки сближения
# x_center = data.iloc[min_dist_idx]["x_spacecraft"]
# y_center = data.iloc[min_dist_idx]["y_spacecraft"]

# ax2.plot(data["x_spacecraft"], data["y_spacecraft"], "r-", linewidth=1.5, label="КА")
# ax2.plot(data["x_planet"], data["y_planet"], "b-", linewidth=2, label="Планета")
# ax2.plot(data["x_moon"], data["y_moon"], "g-", linewidth=1.5, label="Луна")

# # Отмечаем точку минимального расстояния
# ax2.plot(
#     data.iloc[min_dist_idx]["x_spacecraft"],
#     data.iloc[min_dist_idx]["y_spacecraft"],
#     "ro",
#     markersize=8,
#     markeredgecolor="black",
#     markeredgewidth=1,
# )
# ax2.plot(
#     data.iloc[min_dist_idx]["x_moon"],
#     data.iloc[min_dist_idx]["y_moon"],
#     "go",
#     markersize=8,
#     markeredgecolor="black",
#     markeredgewidth=1,
# )

# ax2.set_xlim(x_center - zoom_margin, x_center + zoom_margin)
# ax2.set_ylim(y_center - zoom_margin, y_center + zoom_margin)
# ax2.set_xlabel("X (м)")
# ax2.set_ylabel("Y (м)")
# ax2.set_title(
#     f"Область сближения с луной\nМинимальное расстояние: {min_dist_value:.2e} м"
# )
# ax2.grid(True, alpha=0.3)
# ax2.legend()
# ax2.set_aspect("equal")

# plt.tight_layout()
# plt.savefig("trajectories_zoom.png", dpi=300, bbox_inches="tight")
# print("График с увеличением сохранен как trajectories_zoom.png")

# plt.show()


import os
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def load_trajectory(filepath):

    df = pd.read_csv(filepath, skipinitialspace=True)
    df.columns = df.columns.str.strip()

    return df


def estimate_planet_radius(df):
    rocket_dist = np.sqrt(df["x_spacecraft"] ** 2 + df["y_spacecraft"] ** 2)
    min_dist = rocket_dist.min()
    return min_dist * 0.85 if min_dist > 1e-10 else 1.0


def estimate_sputnik_radius(df):
    sputnik_dist = np.sqrt(df["x_sputnik"] ** 2 + df["y_sputnik"] ** 2)
    return (
        sputnik_dist.mean() * 0.015
        if len(sputnik_dist) > 0 and sputnik_dist.mean() > 0
        else 0.1
    )


def plot_trajectories(df, output_path=None):
    # Оценки размеров тел из данных
    planet_r = estimate_planet_radius(df)
    sputnik_r = estimate_sputnik_radius(df)

    fig, ax = plt.subplots(figsize=(10, 10))
    ax.set_aspect("equal")
    ax.grid(True, alpha=0.3, linestyle="--", linewidth=0.5)
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title("Rocket and sputnik trajectories", fontsize=14, pad=20)

    planet_circle = plt.Circle(
        (0, 0), planet_r, color="#8B7355", label="Планета", zorder=5
    )
    ax.add_patch(planet_circle)

    ax.plot(
        df["x_sputnik"],
        df["y_sputnik"],
        color="#4169E1",
        linewidth=1.5,
        label="Спутник",
        zorder=3,
        alpha=0.9,
    )

    ax.plot(
        df["x_sputnik"].iloc[0],
        df["y_sputnik"].iloc[0],
        "o",
        color="#4169E1",
        markersize=7,
        zorder=4,
        label="Старт спутника",
    )

    ax.plot(
        df["x_spacecraft"],
        df["y_spacecraft"],
        color="#DC143C",
        linewidth=2,
        label="Ракета",
        zorder=2,
    )
    ax.plot(
        df["x_spacecraft"].iloc[0],
        df["y_spacecraft"].iloc[0],
        "s",
        color="#DC143C",
        markersize=8,
        label="Старт ракеты",
        zorder=4,
    )

    last = df.iloc[-1]
    final_dist = np.sqrt(
        (last["x_spacecraft"] - last["x_sputnik"]) ** 2
        + (last["y_spacecraft"] - last["y_sputnik"]) ** 2
    )

    if final_dist <= sputnik_r * 2.5:
        ax.plot(
            last["x_spacecraft"],
            last["y_spacecraft"],
            "*",
            color="#FFD700",
            markersize=30,
            label="💥 Столкновение!",
            zorder=7,
            markeredgecolor="black",
            markeredgewidth=1.5,
        )
        print(
            f"✅ Обнаружено столкновение! Расстояние до центра спутника: {final_dist:.2e} м"
        )
    else:
        ax.plot(
            df["x_spacecraft"].iloc[-1],
            df["y_spacecraft"].iloc[-1],
            "o",
            color="#DC143C",
            markersize=6,
            alpha=0.7,
            label="Финиш ракеты",
            zorder=4,
        )
        print(f"ℹ️  Столкновения нет. Расстояние до центра спутника: {final_dist:.2e} м")
        print(f"   Радиус спутника (оценка): {sputnik_r:.2e} м")

    ax.legend(loc="upper right", fontsize=9, framealpha=0.95, edgecolor="gray")

    all_x = pd.concat([df["x_spacecraft"], df["x_sputnik"], pd.Series([0])])
    all_y = pd.concat([df["y_spacecraft"], df["y_sputnik"], pd.Series([0])])
    margin = 1.15
    range_x = all_x.max() - all_x.min()
    range_y = all_y.max() - all_y.min()
    limit = max(range_x, range_y) * 0.5 * margin
    ax.set_xlim(-limit, limit)
    ax.set_ylim(-limit, limit)

    plt.tight_layout()

    if output_path:
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        plt.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.show()


def main():
    filepath = (
        sys.argv[1] if len(sys.argv) > 1 else "../labs/lr1/trajectory_space_craft.csv"
    )
    df = load_trajectory(filepath)
    plot_trajectories(df)


if __name__ == "__main__":
    main()
