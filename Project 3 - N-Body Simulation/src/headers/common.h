#pragma once

#include <random>
#include <string>
#include <vector>

#include "physics.h"

#define TIME_IT(operations)                                                                        \
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); \
    operations;                                                                                    \
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now(); \
    std::chrono::duration<double> time_span = t2 - t1;                                             \
    std::cout << "Total time: " << time_span.count() << '\n';

// use extern keyword to prevent global variable redefinition
extern int n_body; // number of bodies
extern int n_iteration;

// mass vector
extern std::vector<double> m;
// position vector
extern std::vector<Point> pos;
// velocity vector
extern std::vector<Velocity> v;

void generate_data(std::vector<double>& m, std::vector<Point>& pos, std::vector<Velocity>& v,
                   int n);

void print_information(std::string prog_name, int size, int num_cores);

void split_data(int* counts, int* displs, int total_count, int num_partitions);

void split_data(std::vector<int>& counts, std::vector<int>& displs, int total_count,
                int num_partitions);

#ifdef GUI
void glut_init(int argc, char** argv, const std::string& prog_name);

void glut_update();

void glut_update(const Point* data, size_t n);

// encapsulate the glutMainLoop() function
void glut_main_loop();
#endif