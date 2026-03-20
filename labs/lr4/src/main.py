import matplotlib

matplotlib.use("Agg")

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

current_dir = Path(__file__).parent.absolute()
project_root = current_dir.parent.parent.parent
RESULTS_DIR = project_root / "labs/lr4/out"
RESULTS_DIR.mkdir(exist_ok=True)

print(f"Results will be saved to: {RESULTS_DIR}")

all_results = {}


def plot_simulation_results(data):
    M = data["M"]
    N = data["N"]
    x_coords = np.array(data["x_coords"])
    y_coords = np.array(data["y_coords"])
    trajectories_x = data["trajectories_x"]
    trajectories_y = data["trajectories_y"]
    mean_x = data["mean_x"]
    mean_y = data["mean_y"]
    var_x = data["var_x"]
    var_y = data["var_y"]
    var_R = data["var_R"]
    mean_R = data["mean_R"]

    print(f"\n  Plotting results for M={M}, N={N}")

    if M not in all_results:
        all_results[M] = {}
    all_results[M][N] = {
        "x_coords": x_coords,
        "y_coords": y_coords,
        "trajectories_x": trajectories_x,
        "trajectories_y": trajectories_y,
        "mean_x": mean_x,
        "mean_y": mean_y,
        "var_x": var_x,
        "var_y": var_y,
        "var_R": var_R,
        "mean_R": mean_R,
    }

    # Сохраняем траекторию первой частицы
    if len(trajectories_x) > 0:
        plot_trajectory(trajectories_x[0], trajectories_y[0], M, N)
        plot_mnk_for_each_M()


def plot_mnk_for_each_M():
    """Построение <ΔR²> vs N с линией МНК для каждого M отдельно"""
    for M, results in all_results.items():
        N_values = sorted(results.keys())
        var_R_values = [results[N]["var_R"] for N in N_values]

        # МНК в лог-лог масштабе: ln(⟨ΔR²⟩) = ln(A) + ν·ln(N)
        log_N = np.log(N_values)
        log_var = np.log(var_R_values)
        coeffs = np.polyfit(log_N, log_var, 1)  # [ν, ln(A)]
        nu, ln_A = coeffs[0], coeffs[1]
        A = np.exp(ln_A)

        N_fit = np.linspace(min(N_values), max(N_values), 100)
        var_R_fit = A * N_fit**nu

        fig, ax = plt.subplots(figsize=(8, 6))
        ax.loglog(
            N_values,
            var_R_values,
            "o-",
            label="⟨ΔR²⟩",
            markersize=6,
            linewidth=2,
        )
        ax.loglog(
            N_fit, var_R_fit, "r--", label=f"МНК: ⟨ΔR²⟩={A:.3f}·N^{nu:.3f}", linewidth=2
        )

        ax.set_xlabel("N", fontsize=11)
        ax.set_ylabel("⟨ΔR²⟩", fontsize=11)
        ax.set_title(f"Зависимость ⟨ΔR²⟩ от N (M={M})", fontsize=12)
        ax.legend(fontsize=10)

        plt.tight_layout()
        filename = RESULTS_DIR / f"mnk_M{M}.png"
        plt.savefig(filename, dpi=150, bbox_inches="tight")
        print(f"Saved MNK plot: {filename}")
        plt.close(fig)

        # Вывод параметров в консоль
        print(f"\n📊 M = {M}:")
        print(f"   ν = {nu:.6f}  (теория: ν=1 для нормальной диффузии)")
        print(f"   A = {A:.6f}")
        if abs(nu - 1.0) < 0.1:
            print("   → Нормальная диффузия")
        elif nu > 1.1:
            print("   → Супердиффузия")
        elif nu < 0.9:
            print("   → Субдиффузия")


def plot_trajectory(traj_x, traj_y, M, N):
    fig, ax = plt.subplots(figsize=(9, 8))

    ax.plot(traj_x, traj_y, "b-", linewidth=1.5, alpha=0.8)
    ax.plot(traj_x[0], traj_y[0], "go", markersize=6, label="Старт")
    ax.plot(traj_x[-1], traj_y[-1], "ro", markersize=6, label="Финиш")

    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_title(f"Траектория первой частицы (M={M}, N={N})")
    ax.grid(True, alpha=0.3)
    ax.axis("equal")
    ax.legend(loc="upper right")

    plt.tight_layout()

    filename = RESULTS_DIR / f"trajectory_M{M}_N{N}.png"
    plt.savefig(filename, dpi=150)
    print(f"    Saved trajectory to {filename}")
    plt.close(fig)


def plot_summary_plots():
    # 1. Зависимость <ΔR²> от N для разных M
    fig, ax = plt.subplots(figsize=(10, 6))
    for M, results in all_results.items():
        N_values = sorted(results.keys())
        var_R_values = [results[N]["var_R"] for N in N_values]
        ax.loglog(N_values, var_R_values, "o-", label=f"M={M}", markersize=6)

        if len(N_values) > 1:
            log_N = np.log(N_values)
            log_var = np.log(var_R_values)
            coeffs = np.polyfit(log_N, log_var, 1)
            nu = coeffs[0]
            A = np.exp(coeffs[1])
            theory = A * np.array(N_values) ** nu
            ax.loglog(N_values, theory, "--", alpha=0.5, label=f"M={M} (ν={nu:.3f})")

            print(f"\nM = {M}:")
            print(f"  ν = {nu:.6f}")
            print(f"  A = {A:.6f}")
            if abs(nu - 1.0) < 0.1:
                print("  → Нормальная диффузия (ν ≈ 1)")
            elif nu > 1.1:
                print("  → Супердиффузия (ν > 1)")
            elif nu < 0.9:
                print("  → Субдиффузия (ν < 1)")

    ax.set_xlabel("N (число прыжков)")
    ax.set_ylabel("<ΔR²>")
    ax.set_title("Зависимость среднего квадрата смещения от числа прыжков")
    ax.grid(True, alpha=0.3, which="both")
    ax.legend(fontsize=10)

    filename = RESULTS_DIR / "var_R_vs_N.png"
    plt.savefig(filename, dpi=150, bbox_inches="tight")
    print(f"\nSaved {filename}")
    plt.close(fig)

    # 2. Для каждого M строим отдельные графики статистик
    for M, results in all_results.items():
        fig, axes = plt.subplots(1, 2, figsize=(12, 5))

        N_values = sorted(results.keys())

        # Правильные значения для графиков
        mean_R_values = [results[N]["mean_R"] for N in N_values]
        var_R_values = [results[N]["var_R"] for N in N_values]

        # График среднего радиуса
        axes[0].plot(
            N_values, mean_R_values, "o-", label="<R>", markersize=4, color="blue"
        )
        axes[0].set_xlabel("N")
        axes[0].set_ylabel("<R>")
        axes[0].set_title(f"Среднее смещение (M={M})")
        axes[0].grid(True, alpha=0.3)
        axes[0].legend()

        # График дисперсии
        axes[1].plot(
            N_values, var_R_values, "s-", label="σ²(R)", markersize=4, color="red"
        )
        axes[1].set_xlabel("N")
        axes[1].set_ylabel("Дисперсия")
        axes[1].set_title(f"Дисперсия (M={M})")
        axes[1].grid(True, alpha=0.3)
        axes[1].legend()

        plt.tight_layout()
        filename = RESULTS_DIR / f"statistics_M{M}.png"
        plt.savefig(filename, dpi=150, bbox_inches="tight")
        print(f"Saved {filename}")
        plt.close(fig)

    plot_histograms_for_each_M()


def plot_histograms_for_each_M():
    for M, results in all_results.items():
        fig, axes = plt.subplots(1, 2, figsize=(14, 6))
        fig.suptitle(f"Распределения координат M={M}", fontsize=14)

        all_x_coords = []
        all_y_coords = []

        for N, stats in sorted(results.items()):
            all_x_coords.extend(stats["x_coords"])
            all_y_coords.extend(stats["y_coords"])

        all_x_coords = np.array(all_x_coords)
        all_y_coords = np.array(all_y_coords)

        total_mean_x = np.mean(all_x_coords)
        total_std_x = np.std(all_x_coords)
        total_mean_y = np.mean(all_y_coords)
        total_std_y = np.std(all_y_coords)

        ax1 = axes[0]
        n_x, bins_x, patches_x = ax1.hist(
            all_x_coords,
            bins=50,
            alpha=0.7,
            color="blue",
            edgecolor="black",
            linewidth=1,
        )
        ax1.set_xlabel("x")
        ax1.set_ylabel("Частота")
        ax1.set_title("Распределение по x")
        ax1.grid(True, alpha=0.3)

        ax2 = axes[1]
        n_y, bins_y, patches_y = ax2.hist(
            all_y_coords,
            bins=50,
            alpha=0.7,
            color="green",
            edgecolor="black",
            linewidth=1,
        )
        ax2.set_xlabel("y")
        ax2.set_ylabel("Частота")
        ax2.set_title("Распределение по y")
        ax2.grid(True, alpha=0.3)

        plt.tight_layout()
        filename = RESULTS_DIR / f"histograms_M{M}.png"
        plt.savefig(filename, dpi=150, bbox_inches="tight")
        print(f"  Saved histograms to {filename}")
        print(
            f"    M={M}: mean_x={total_mean_x:.4f}, std_x={total_std_x:.4f}, mean_y={total_mean_y:.4f}, std_y={total_std_y:.4f}"
        )
        plt.close(fig)
