import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.animation import FuncAnimation

df = pd.read_csv("../labs/lr3/src/velocity_verlet.csv")

steps = df["step"].values
time = df["time"].values
q_data = df[[col for col in df.columns if col.startswith("q")]].values
v_data = df[[col for col in df.columns if col.startswith("v")]].values

n_particles = q_data.shape[1]

skip_frames = max(1, len(steps) // 200)
indices = np.arange(0, len(steps), skip_frames)
steps, time, q_data, v_data = (
    steps[indices],
    time[indices],
    q_data[indices],
    v_data[indices],
)
n_frames = len(indices)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 8))

(line_q,) = ax1.plot([], [], "b-", linewidth=1.5)
(line_v,) = ax2.plot([], [], "r-", linewidth=1.5)
time_text = fig.text(
    0.02, 0.98, "", fontsize=14, bbox=dict(boxstyle="round", facecolor="lightblue")
)

# x_range = [0, n_particles - 1]
x_range = [450, 550]

ax1.set(
    xlim=x_range,
    ylim=[np.min(q_data) - 0.1, np.max(q_data) + 0.1],
    ylabel="displacement",
)
ax2.set(
    xlim=x_range,
    ylim=[np.min(v_data) - 0.1, np.max(v_data) + 0.1],
    ylabel="velocity",
    xlabel="particle index",
)


def animate(frame):
    x = np.arange(n_particles)
    line_q.set_data(x, q_data[frame])
    line_v.set_data(x, v_data[frame])
    time_text.set_text(f"t = {time[frame]:.2f}")
    return line_q, line_v, time_text


anim = FuncAnimation(fig, animate, frames=n_frames, interval=100, blit=True)
anim.save("../labs/lr3/src/fpu_animation_haha.gif", writer="pillow", dpi=100)


print("!!анимация сохранена!!")
