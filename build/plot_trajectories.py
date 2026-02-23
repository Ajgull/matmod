import pandas as pd
import matplotlib.pyplot as plt

# Чтение данных
df = pd.read_csv('trajectory.csv')
df_rel = pd.read_csv('trajectory_relative.csv')

# График траекторий
plt.figure(figsize=(12, 12))
plt.plot(df['x_planet'], df['y_planet'], 'b-', linewidth=1, label='Планета')
plt.plot(df['x_sputnik'], df['y_sputnik'], 'r-', linewidth=1, label='Спутник')
plt.plot(0, 0, 'yo', markersize=10, label='Солнце')
plt.grid(True, alpha=0.3)
plt.xlabel('X (м)')
plt.ylabel('Y (м)')
plt.title('Траектории движения тел')
plt.legend()
plt.axis('equal')
plt.savefig('trajectories.png', dpi=300, bbox_inches='tight')
plt.show()

# Относительная траектория
plt.figure(figsize=(8, 8))
plt.plot(df_rel['x_rel'], df_rel['y_rel'], 'g-', linewidth=1)
plt.plot(0, 0, 'bo', markersize=8, label='Планета')
plt.grid(True, alpha=0.3)
plt.xlabel('X относительно планеты (м)')
plt.ylabel('Y относительно планеты (м)')
plt.title('Орбита спутника относительно планеты')
plt.axis('equal')
plt.legend()
plt.savefig('relative_orbit.png', dpi=300, bbox_inches='tight')
plt.show()
