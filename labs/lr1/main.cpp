#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

// Гравитационная постоянная
const double G = 6.67e-11;  // м^3/(кг·с^2)

// Параметры системы (вариант 8)
const double M_SUN = 2.0e30;      // масса Солнца, кг
const double M_PLANET = 1.9e27;   // масса планеты, кг
const double M_SPUTNIK = 4.8e22;  // масса спутника, кг

// Расстояния (в метрах)
const double R_SUN_PLANET = 780.0e9;      // 780 млн км = 7.8e11 м
const double R_PLANET_SPUTNIK = 167.0e6;  // 167 тыс. км = 1.67e8 м

// Скорости (в м/с)
const double V_PLANET = 13.0e3;   // 13 км/с
const double V_SPUTNIK = 13.7e3;  // 13.7 км/с

// Минимальное расстояние для предотвращения деления на ноль
const double MIN_DISTANCE = 1.0;  // 1 метр

// Структура для хранения состояния системы
struct State {
    double x_planet;    // координата X планеты
    double y_planet;    // координата Y планеты
    double x_sputnik;   // координата X спутника
    double y_sputnik;   // координата Y спутника
    double vx_planet;   // скорость планеты по X
    double vy_planet;   // скорость планеты по Y
    double vx_sputnik;  // скорость спутника по X
    double vy_sputnik;  // скорость спутника по Y
    double t;           // время

    // Конструктор с начальными условиями
    State() {
        // Все тела на оси X
        x_planet = R_SUN_PLANET;
        y_planet = 0.0;
        x_sputnik = R_SUN_PLANET + R_PLANET_SPUTNIK;
        y_sputnik = 0.0;

        // Скорости направлены вдоль оси Y
        vx_planet = 0.0;
        vy_planet = V_PLANET;
        vx_sputnik = 0.0;
        vy_sputnik = V_SPUTNIK;

        t = 0.0;
    }

    // Конструктор копирования
    State(const State& other) {
        x_planet = other.x_planet;
        y_planet = other.y_planet;
        x_sputnik = other.x_sputnik;
        y_sputnik = other.y_sputnik;
        vx_planet = other.vx_planet;
        vy_planet = other.vy_planet;
        vx_sputnik = other.vx_sputnik;
        vy_sputnik = other.vy_sputnik;
        t = other.t;
    }
};

// Класс для решения задачи
class ThreeBodyProblem {
   private:
    std::vector<State> trajectory;  // вектор для хранения траектории

    // Безопасное вычисление расстояния
    double safe_distance(double dx, double dy) const {
        double r = std::sqrt(dx * dx + dy * dy);
        return std::max(r, MIN_DISTANCE);
    }

    // Вычисление правых частей системы ОДУ
    void computeDerivatives(const State& s, double dydt[8]) const {
        // Расстояния с защитой от деления на ноль
        double r_sun_planet = safe_distance(s.x_planet, s.y_planet);
        double r_sun_sputnik = safe_distance(s.x_sputnik, s.y_sputnik);

        double dx = s.x_sputnik - s.x_planet;
        double dy = s.y_sputnik - s.y_planet;
        double r_planet_sputnik = safe_distance(dx, dy);

        // Ускорение планеты
        // От Солнца
        double ax_planet_sun =
            -G * M_SUN * s.x_planet / (r_sun_planet * r_sun_planet * r_sun_planet);
        double ay_planet_sun =
            -G * M_SUN * s.y_planet / (r_sun_planet * r_sun_planet * r_sun_planet);

        // От спутника (сила, действующая на планету со стороны спутника)
        double ax_planet_sputnik =
            -G * M_SPUTNIK * (-dx) / (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);
        double ay_planet_sputnik =
            -G * M_SPUTNIK * (-dy) / (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);

        // Ускорение спутника
        // От Солнца
        double ax_sputnik_sun =
            -G * M_SUN * s.x_sputnik / (r_sun_sputnik * r_sun_sputnik * r_sun_sputnik);
        double ay_sputnik_sun =
            -G * M_SUN * s.y_sputnik / (r_sun_sputnik * r_sun_sputnik * r_sun_sputnik);

        // От планеты
        double ax_sputnik_planet =
            -G * M_PLANET * dx / (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);
        double ay_sputnik_planet =
            -G * M_PLANET * dy / (r_planet_sputnik * r_planet_sputnik * r_planet_sputnik);

        // Заполняем производные
        dydt[0] = s.vx_planet;                         // dx_planet/dt
        dydt[1] = s.vy_planet;                         // dy_planet/dt
        dydt[2] = s.vx_sputnik;                        // dx_sputnik/dt
        dydt[3] = s.vy_sputnik;                        // dy_sputnik/dt
        dydt[4] = ax_planet_sun + ax_planet_sputnik;   // dvx_planet/dt
        dydt[5] = ay_planet_sun + ay_planet_sputnik;   // dvy_planet/dt
        dydt[6] = ax_sputnik_sun + ax_sputnik_planet;  // dvx_sputnik/dt
        dydt[7] = ay_sputnik_sun + ay_sputnik_planet;  // dvy_sputnik/dt
    }

   public:
    // Метод Рунге-Кутты 4-го порядка
    bool stepRK4(State& s, double dt) {
        double k1[8], k2[8], k3[8], k4[8];
        State temp(*&s);  // копируем текущее состояние

        // Проверка на некорректные значения
        if (std::isnan(s.x_planet) || std::isinf(s.x_planet)) {
            return false;
        }

        try {
            // k1
            computeDerivatives(s, k1);

            // k2
            temp = s;
            temp.x_planet += k1[0] * dt / 2;
            temp.y_planet += k1[1] * dt / 2;
            temp.x_sputnik += k1[2] * dt / 2;
            temp.y_sputnik += k1[3] * dt / 2;
            temp.vx_planet += k1[4] * dt / 2;
            temp.vy_planet += k1[5] * dt / 2;
            temp.vx_sputnik += k1[6] * dt / 2;
            temp.vy_sputnik += k1[7] * dt / 2;
            temp.t = s.t + dt / 2;
            computeDerivatives(temp, k2);

            // k3
            temp = s;
            temp.x_planet += k2[0] * dt / 2;
            temp.y_planet += k2[1] * dt / 2;
            temp.x_sputnik += k2[2] * dt / 2;
            temp.y_sputnik += k2[3] * dt / 2;
            temp.vx_planet += k2[4] * dt / 2;
            temp.vy_planet += k2[5] * dt / 2;
            temp.vx_sputnik += k2[6] * dt / 2;
            temp.vy_sputnik += k2[7] * dt / 2;
            temp.t = s.t + dt / 2;
            computeDerivatives(temp, k3);

            // k4
            temp = s;
            temp.x_planet += k3[0] * dt;
            temp.y_planet += k3[1] * dt;
            temp.x_sputnik += k3[2] * dt;
            temp.y_sputnik += k3[3] * dt;
            temp.vx_planet += k3[4] * dt;
            temp.vy_planet += k3[5] * dt;
            temp.vx_sputnik += k3[6] * dt;
            temp.vy_sputnik += k3[7] * dt;
            temp.t = s.t + dt;
            computeDerivatives(temp, k4);

            // Финальное значение
            s.x_planet += (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0]) * dt / 6;
            s.y_planet += (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1]) * dt / 6;
            s.x_sputnik += (k1[2] + 2 * k2[2] + 2 * k3[2] + k4[2]) * dt / 6;
            s.y_sputnik += (k1[3] + 2 * k2[3] + 2 * k3[3] + k4[3]) * dt / 6;
            s.vx_planet += (k1[4] + 2 * k2[4] + 2 * k3[4] + k4[4]) * dt / 6;
            s.vy_planet += (k1[5] + 2 * k2[5] + 2 * k3[5] + k4[5]) * dt / 6;
            s.vx_sputnik += (k1[6] + 2 * k2[6] + 2 * k3[6] + k4[6]) * dt / 6;
            s.vy_sputnik += (k1[7] + 2 * k2[7] + 2 * k3[7] + k4[7]) * dt / 6;
            s.t += dt;

            return true;

        } catch (...) {
            return false;
        }
    }

    // Вычисление периода обращения планеты
    double getPlanetPeriod() const {
        return 2 * M_PI * std::sqrt(R_SUN_PLANET * R_SUN_PLANET * R_SUN_PLANET / (G * M_SUN));
    }

    // Вычисление полной энергии системы
    double computeTotalEnergy(const State& s) const {
        // Кинетическая энергия
        double E_kin_planet =
            0.5 * M_PLANET * (s.vx_planet * s.vx_planet + s.vy_planet * s.vy_planet);
        double E_kin_sputnik =
            0.5 * M_SPUTNIK * (s.vx_sputnik * s.vx_sputnik + s.vy_sputnik * s.vy_sputnik);

        // Потенциальная энергия
        double r_sun_planet = safe_distance(s.x_planet, s.y_planet);
        double r_sun_sputnik = safe_distance(s.x_sputnik, s.y_sputnik);
        double dx = s.x_sputnik - s.x_planet;
        double dy = s.y_sputnik - s.y_planet;
        double r_planet_sputnik = safe_distance(dx, dy);

        double E_pot = -G * M_SUN * M_PLANET / r_sun_planet -
                       G * M_SUN * M_SPUTNIK / r_sun_sputnik -
                       G * M_PLANET * M_SPUTNIK / r_planet_sputnik;

        return E_kin_planet + E_kin_sputnik + E_pot;
    }

    // Интегрирование на заданном интервале времени
    std::vector<State> integrate(double t_max, double dt, bool save_all = true) {
        trajectory.clear();
        State current;

        // Сохраняем начальное состояние
        if (save_all) {
            trajectory.push_back(current);
        }

        int step = 0;
        int save_step =
            std::max(1, static_cast<int>(1.0 / dt));  // сохраняем примерно 1 точку в секунду
        int progress_step = static_cast<int>(t_max / dt / 100);  // 1% прогресса

        std::cout << "Интегрирование: 0%";
        std::cout.flush();

        while (current.t < t_max) {
            // Выполняем шаг
            bool success = stepRK4(current, dt);

            if (!success) {
                std::cout << "\nОшибка интегрирования на шаге " << step << std::endl;
                break;
            }

            // Сохраняем с определенной частотой
            if (save_all && step % save_step == 0) {
                trajectory.push_back(current);
            }

            // Показываем прогресс
            if (step % progress_step == 0) {
                int percent = static_cast<int>(100.0 * current.t / t_max);
                std::cout << "\rИнтегрирование: " << percent << "%";
                std::cout.flush();
            }

            step++;

            // Защита от бесконечного цикла
            if (step > 1e8) {
                std::cout << "\nСлишком много шагов! Прерывание.\n";
                break;
            }
        }

        std::cout << "\rИнтегрирование: 100% завершено\n";

        // Добавляем конечную точку
        if (save_all && (trajectory.empty() || trajectory.back().t < current.t)) {
            trajectory.push_back(current);
        }

        return trajectory;
    }

    // Сохранение траектории в файл
    void saveTrajectory(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return;
        }

        file << std::scientific << std::setprecision(15);

        // Заголовок
        file << "t,x_planet,y_planet,x_sputnik,y_sputnik,"
             << "vx_planet,vy_planet,vx_sputnik,vy_sputnik,energy\n";

        for (const auto& s : trajectory) {
            double energy = computeTotalEnergy(s);
            file << s.t << "," << s.x_planet << "," << s.y_planet << "," << s.x_sputnik << ","
                 << s.y_sputnik << "," << s.vx_planet << "," << s.vy_planet << "," << s.vx_sputnik
                 << "," << s.vy_sputnik << "," << energy << "\n";
        }

        file.close();
        std::cout << "Траектория сохранена в файл: " << filename << std::endl;
    }

    // Сохранение относительной траектории спутника
    void saveRelativeTrajectory(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return;
        }

        file << std::scientific << std::setprecision(10);

        file << "x_rel,y_rel\n";

        for (const auto& s : trajectory) {
            double x_rel = s.x_sputnik - s.x_planet;
            double y_rel = s.y_sputnik - s.y_planet;
            file << x_rel << "," << y_rel << "\n";
        }

        file.close();
        std::cout << "Относительная траектория сохранена в файл: " << filename << std::endl;
    }

    // Вывод информации о системе
    void printSystemInfo() const {
        std::cout << "\n=== ПАРАМЕТРЫ СИСТЕМЫ ===\n";
        std::cout << "Масса Солнца: " << M_SUN << " кг\n";
        std::cout << "Масса планеты: " << M_PLANET << " кг\n";
        std::cout << "Масса спутника: " << M_SPUTNIK << " кг\n\n";

        std::cout << "Расстояние Солнце-Планета: " << R_SUN_PLANET / 1e9 << " млн км\n";
        std::cout << "Расстояние Планета-Спутник: " << R_PLANET_SPUTNIK / 1e6 << " тыс. км\n\n";

        std::cout << "Начальная скорость планеты: " << V_PLANET / 1e3 << " км/с\n";
        std::cout << "Начальная скорость спутника: " << V_SPUTNIK / 1e3 << " км/с\n\n";

        double T_planet = getPlanetPeriod();
        std::cout << "Период обращения планеты: " << T_planet << " с\n";
        std::cout << "(" << T_planet / 86400 << " дней, " << T_planet / (365.25 * 86400)
                  << " лет)\n";

        // Период обращения спутника вокруг планеты (приближенно)
        double T_sputnik =
            2 * M_PI *
            std::sqrt(R_PLANET_SPUTNIK * R_PLANET_SPUTNIK * R_PLANET_SPUTNIK / (G * M_PLANET));
        std::cout << "Период обращения спутника вокруг планеты: " << T_sputnik << " с\n";
        std::cout << "(" << T_sputnik / 86400 << " дней)\n";
    }
};

// Функция для создания Python скрипта для визуализации (альтернатива gnuplot)
void createPythonScript() {
    std::ofstream script("plot_trajectories.py");
    script << "import pandas as pd\n";
    script << "import matplotlib.pyplot as plt\n\n";

    script << "# Чтение данных\n";
    script << "df = pd.read_csv('trajectory.csv')\n";
    script << "df_rel = pd.read_csv('trajectory_relative.csv')\n\n";

    script << "# График траекторий\n";
    script << "plt.figure(figsize=(12, 12))\n";
    script << "plt.plot(df['x_planet'], df['y_planet'], 'b-', linewidth=1, label='Планета')\n";
    script << "plt.plot(df['x_sputnik'], df['y_sputnik'], 'r-', linewidth=1, label='Спутник')\n";
    script << "plt.plot(0, 0, 'yo', markersize=10, label='Солнце')\n";
    script << "plt.grid(True, alpha=0.3)\n";
    script << "plt.xlabel('X (м)')\n";
    script << "plt.ylabel('Y (м)')\n";
    script << "plt.title('Траектории движения тел')\n";
    script << "plt.legend()\n";
    script << "plt.axis('equal')\n";
    script << "plt.savefig('trajectories.png', dpi=300, bbox_inches='tight')\n";
    script << "plt.show()\n\n";

    script << "# Относительная траектория\n";
    script << "plt.figure(figsize=(8, 8))\n";
    script << "plt.plot(df_rel['x_rel'], df_rel['y_rel'], 'g-', linewidth=1)\n";
    script << "plt.plot(0, 0, 'bo', markersize=8, label='Планета')\n";
    script << "plt.grid(True, alpha=0.3)\n";
    script << "plt.xlabel('X относительно планеты (м)')\n";
    script << "plt.ylabel('Y относительно планеты (м)')\n";
    script << "plt.title('Орбита спутника относительно планеты')\n";
    script << "plt.axis('equal')\n";
    script << "plt.legend()\n";
    script << "plt.savefig('relative_orbit.png', dpi=300, bbox_inches='tight')\n";
    script << "plt.show()\n";

    script.close();
    std::cout << "\nPython скрипт для визуализации создан: plot_trajectories.py\n";
    std::cout << "Для построения графиков выполните:\n";
    std::cout << "  python plot_trajectories.py\n";
}

int main() {
    try {
        // Создаем объект задачи
        ThreeBodyProblem problem;

        // Выводим информацию о системе
        problem.printSystemInfo();

        // Параметры интегрирования
        double T_planet = problem.getPlanetPeriod();
        double t_max = T_planet / 10;  // Для начала проинтегрируем 1/10 оборота
        double dt = T_planet / 10000;  // 10000 шагов за период

        std::cout << "\n=== ПАРАМЕТРЫ ИНТЕГРИРОВАНИЯ ===\n";
        std::cout << "Время интегрирования: " << t_max << " с\n";
        std::cout << "(" << t_max / 86400 << " дней)\n";
        std::cout << "Шаг интегрирования: " << dt << " с\n";
        std::cout << "Количество шагов: " << static_cast<int>(t_max / dt) << "\n";

        // Интегрируем
        std::cout << "\nНачинаем интегрирование...\n";
        auto trajectory = problem.integrate(t_max, dt);
        std::cout << "Интегрирование завершено. Получено " << trajectory.size() << " точек.\n";

        if (trajectory.empty()) {
            std::cerr << "Ошибка: не получено точек траектории!\n";
            return 1;
        }

        // Сохраняем результаты
        problem.saveTrajectory("trajectory.csv");
        problem.saveRelativeTrajectory("trajectory_relative.csv");

        // Создаем скрипт для визуализации
        createPythonScript();

        // Проверка сохранения энергии
        std::cout << "\n=== ПРОВЕРКА СОХРАНЕНИЯ ЭНЕРГИИ ===\n";
        double E0 = problem.computeTotalEnergy(trajectory.front());
        double E1 = problem.computeTotalEnergy(trajectory.back());
        double dE = std::abs((E1 - E0) / E0) * 100;

        std::cout << "Начальная энергия: " << E0 << " Дж\n";
        std::cout << "Конечная энергия: " << E1 << " Дж\n";
        std::cout << "Относительное изменение: " << dE << " %\n";

        if (dE < 0.1) {
            std::cout << "✓ Энергия сохраняется хорошо (изменение < 0.1%)\n";
        } else if (dE < 1.0) {
            std::cout << "⚠ Энергия сохраняется удовлетворительно (изменение < 1%)\n";
        } else {
            std::cout << "✗ Энергия сохраняется плохо. Уменьшите шаг интегрирования.\n";
        }

        // Вывод конечных положений
        const auto& last = trajectory.back();
        std::cout << "\n=== КОНЕЧНЫЕ ПОЛОЖЕНИЯ ===\n";
        std::cout << "Планета: (" << last.x_planet << ", " << last.y_planet << ") м\n";
        std::cout << "Спутник: (" << last.x_sputnik << ", " << last.y_sputnik << ") м\n";

        std::cout << "\nПрограмма завершена успешно!\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная ошибка!" << std::endl;
        return 1;
    }

    return 0;
}