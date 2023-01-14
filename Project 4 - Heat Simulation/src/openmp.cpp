#include <iomanip>
#include <iostream>
#include <omp.h>

#include "common.h"

int n_omp_threads;


void update_omp() {
    // update_omp the temperature of each point, and store the result in `new_data` to avoid data racing
    #pragma omp parallel for
    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            const int idx = i * size + j;
            if (fire_area[idx]) new_data[idx] = FIRE_TEMP;
            else {
                const float up = data[idx - size];
                const float down = data[idx + size];
                const float left = data[idx - 1];
                const float right = data[idx + 1];
                const float new_val = (up + down + left + right) / 4;
                new_data[idx] = new_val;
            }
        }
    }
}

// maintain the temperature of wall
void maintain_wall_omp() {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        new_data[i] = WALL_TEMP;
        new_data[i * size] = WALL_TEMP;
        new_data[i * size + size - 1] = WALL_TEMP;
        new_data[len - size + i] = WALL_TEMP;
    }
}

void generate_fire_area_omp() {
    fire_area.resize(len, false);

    constexpr int fire1_r2 = FIRE_SIZE * FIRE_SIZE;
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            const int a = i - size / 2;
            const int b = j - size / 2;
            const int r2 = 0.5 * a * a + 0.8 * b * b - 0.5 * a * b;
            if (r2 < fire1_r2) fire_area[i * size + j] = true;
        }
    }

    constexpr int fire2_r2 = (FIRE_SIZE / 2) * (FIRE_SIZE / 2);
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            const int a = i - 1 * size / 3;
            const int b = j - 1 * size / 3;
            const int r2 = a * a + b * b;
            if (r2 < fire2_r2) fire_area[i * size + j] = true;
        }
    }
}

void master() {
    omp_set_num_threads(n_omp_threads);

    initialize();
    generate_fire_area_omp();

    int count = 1;
    double total_time = 0;

    std::cout << std::fixed << std::setprecision(6);
    while (count <= MAX_ITERATIONS) {
        time_point t1 = std_clock::now();
        
        update_omp();
        // maintain_fire_omp();
        maintain_wall_omp();

        std::swap(data, new_data);

        time_point t2 = std_clock::now();
        const double time = std::chrono::duration<double>(t2 - t1).count();
        if (DEBUG) std::cout << "Iteration " << count << " ,elapsed time: " << time << '\n';
        total_time += time;

#ifdef GUI
        // plot(data);
        data2pixels(data, pixels);
        plot(pixels);
#endif

        count++;
    }

    std::cout << "Converge after " << count - 1 << " iterations, elapsed time: " << total_time
              << ", average computation time: " << total_time / (count - 1) << std::endl;
}

int main(const int argc, char* argv[]) {
    const std::string name = "OpenMP";

    size = std::stoi(argv[1]);
    n_omp_threads = std::stoi(argv[2]);

#ifdef GUI
    glut_init(argc, argv, name);
#endif

    master();

    print_info(name, n_omp_threads);

    return 0;
}
