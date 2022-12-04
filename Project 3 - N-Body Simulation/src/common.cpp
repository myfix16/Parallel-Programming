// glut for windows related problems
// see https://stackoverflow.com/questions/24186250/include-windows-h-causes-a-lot-of-syntax-errors
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cstdlib>

#include "Windows.h"
#endif

#ifdef GUI
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <iostream>

#include "common.h"

int n_body;
int n_iteration;

std::vector<double> m;
std::vector<Point> pos;
std::vector<Velocity> v;

void generate_data(std::vector<double>& m, std::vector<Point>& pos, std::vector<Velocity>& v,
                   const int n) {
    m.resize(n_body, 0);
    pos.resize(n_body, Point{});
    v.resize(n_body, Velocity{});

    // initialize a random distribution
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> uniform(0, RAND_MAX);
    std::uniform_int_distribution<std::mt19937::result_type> u_mass(1, MAX_MASS);

    for (int i = 0; i < n; i++) {
        m[i] = u_mass(rng);
        pos[i].x = 2000.0 + uniform(rng) % (BOUND_X / 4);
        pos[i].y = 2000.0 + uniform(rng) % (BOUND_Y / 4);
    }
}

void print_information(const std::string prog_name, const int size, const int num_cores) {
    std::cout << "Student ID: 119020038\n";
    std::cout << "Name: Mao Xi\n";
    std::cout << "Assignment 2: N Body Simulation " << prog_name << " Implementation\n";
    std::cout << "Number of Bodies: " << size << "\n";
    std::cout << "Number of Cores: " << num_cores << std::endl;
}

// split the data
void split_data(std::vector<int>& counts, std::vector<int>& displs, const int total_count,
                const int num_partitions) {
    split_data(counts.data(), displs.data(), total_count, num_partitions);
}

void split_data(int* counts, int* displs, const int total_count,
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

#ifdef GUI
void glut_init(int argc, char** argv, const std::string& prog_name) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 800);
    glutCreateWindow((std::string("N Body Simulation ") + prog_name + " Implementation").c_str());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0, BOUND_X, 0, BOUND_Y);
    glutDisplayFunc(glut_update);
}

void glut_update() {
    glut_update(pos.data(), pos.size());
}

inline void glut_update(const Point* data, const size_t n) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < n; ++i) {
        const Point& point = data[i];
        glVertex2f(static_cast<float>(point.x), static_cast<float>(point.y));
    }
    glEnd();
    glFlush();
    glutSwapBuffers();
}

void glut_main_loop() {
    glutMainLoop();
}

#endif