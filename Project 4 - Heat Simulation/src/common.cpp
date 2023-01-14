#include "common.h"

#include <iostream>

constexpr int WINDOW_SIZE = 800;
constexpr int RESOLUTION = 800;
constexpr float PARTICLE_SIZE = static_cast<float>(WINDOW_SIZE) / RESOLUTION;

int size;
int len;
std::vector<float> data(0);
std::vector<float> new_data(0);
std::vector<bool> fire_area(0);
#ifdef GUI
std::vector<GLubyte> pixels(RESOLUTION* RESOLUTION * 3);
#endif


void initialize() {
    len = size * size;
    data.resize(len, WALL_TEMP);
    new_data.resize(len);
    fire_area.resize(len, false);
}

void generate_fire_area(const int begin, const int end) {
    constexpr int fire1_r2 = FIRE_SIZE * FIRE_SIZE;
    for (int i = begin; i < end; i++) {
        for (int j = 0; j < size; j++) {
            const int a = i - size / 2;
            const int b = j - size / 2;
            const int r2 = 0.5 * a * a + 0.8 * b * b - 0.5 * a * b;
            if (r2 < fire1_r2) fire_area[i * size + j] = true;
        }
    }

    constexpr int fire2_r2 = (FIRE_SIZE / 2) * (FIRE_SIZE / 2);
    for (int i = begin; i < end; i++) {
        for (int j = 0; j < size; j++) {
            const int a = i - 1 * size / 3;
            const int b = j - 1 * size / 3;
            const int r2 = a * a + b * b;
            if (r2 < fire2_r2) fire_area[i * size + j] = true;
        }
    }
}

void update(const int begin, const int end) {
    for (int i = begin; i < end; i++) {
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

void maintain_wall(const int begin, const int end) {
    for (int i = begin; i < end; i++) {
        new_data[i] = WALL_TEMP;
        new_data[i * size] = WALL_TEMP;
        new_data[i * size + size - 1] = WALL_TEMP;
        new_data[len - size + i] = WALL_TEMP;
    }
}

void split_data(std::vector<int>& counts, std::vector<int>& displs, const int total_count,
                const int num_partitions) {
    const int quotient = total_count / num_partitions;
    const int remainder = total_count % num_partitions;
    for (size_t i = 0; i < num_partitions; i++) {
        counts[i] = quotient;
        if (i < remainder) counts[i]++;
    }
    displs[0] = 0;
    for (size_t i = 1; i <= num_partitions; i++) {
        displs[i] = displs[i - 1] + counts[i - 1];
    }
}

void print_info(const std::string& prog_name, const int num_cores) {
    std::cout << "Student ID: 119020038\n";
    std::cout << "Name: Mao Xi\n";
    std::cout << "Assignment 4: Heat Distribution " << prog_name << " Implementation\n";
    std::cout << "Problem Size: " << size << "\n";
    std::cout << "Number of Cores: " << num_cores << std::endl;
}

// GUI functions
#ifdef GUI
void glut_init(int argc, char** argv, const std::string& prog_name) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    const std::string title =
        std::string("Heat Distribution Simulation ") + prog_name + " Implementation";
    glutCreateWindow(title.c_str());
    gluOrtho2D(0, RESOLUTION, 0, RESOLUTION);
}

// convert raw data (large, size^2) to pixels (small, resolution^2) for faster rendering speed
void data2pixels(const std::vector<float>& data, std::vector<GLubyte>& pixels) {
    const float factor_data_pixel = static_cast<float>(size) / RESOLUTION;
    const float factor_temp_color = static_cast<float>(255) / FIRE_TEMP;
    for (int x = 0; x < RESOLUTION; x++) {
        for (int y = 0; y < RESOLUTION; y++) {
            const int idx = x * RESOLUTION + y;
            const int idx_pixel = idx * 3;
            const int x_raw = x * factor_data_pixel;
            const int y_raw = y * factor_data_pixel;
            const int idx_raw = y_raw * size + x_raw;
            const float temp = data[idx_raw];
            const int color = (static_cast<int>(temp) / 5 * 5) * factor_temp_color;
            pixels[idx_pixel] = color;
            pixels[idx_pixel + 1] = 255 - color;
            pixels[idx_pixel + 2] = 255 - color;
        }
    }
}

void plot(const std::vector<GLubyte>& pixels) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(RESOLUTION, RESOLUTION, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glutSwapBuffers();
}
#endif