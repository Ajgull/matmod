import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

filename = "../labs/lr2/src/strange_attractor.csv"

with open(filename, "r") as f:
    first_line = f.readline().strip()

params_info = ""
if first_line.startswith("#"):
    params_info = first_line[1:].strip()
    parts = params_info.split(",")
    a_val = float(parts[0].split("=")[1].strip())
    b_val = float(parts[1].split("=")[1].strip())


df = pd.read_csv(filename, comment="#")

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection="3d")

ax.plot(
    df["x"],
    df["y"],
    df["z"],
    label="trajectory",
    color="blue",
    linewidth=1,
)


# ax.scatter(
#     0,
#     0,
#     0,
#     color="green",
#     s=100,
#     label="O1",
# )

if b_val > -1:
    val = np.sqrt(8.0 / 3.0 * (b_val + 1))
    # ax.scatter(
    #     val,
    #     val,
    #     b_val + 1,
    #     color="purple",
    #     s=10,
    #     label="O2",
    # )
    ax.scatter(
        -val,
        -val,
        b_val + 1,
        color="orange",
        s=100,
        label="O3",
    )

ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")


text_str = f"a = {a_val}\nb = {b_val}\n"

ax.text2D(
    0.02,
    0.98,
    text_str,
    transform=ax.transAxes,
    verticalalignment="top",
    fontsize=10,
    bbox=dict(boxstyle="round", facecolor="wheat", alpha=0.8, edgecolor="black"),
)

ax.legend(loc="upper left", bbox_to_anchor=(1.05, 1), fontsize=9)
ax.view_init(elev=20, azim=-125)


plt.tight_layout()

output_filename = f"lr2_screen/plot_a{a_val}_b{b_val}.png"
plt.savefig(output_filename, dpi=300, bbox_inches="tight")
plt.show()
