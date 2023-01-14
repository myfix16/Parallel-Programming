#pragma once

#ifdef GUI
// glut for windows related problems
// see https://stackoverflow.com/questions/24186250/include-windows-h-causes-a-lot-of-syntax-errors
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cstdlib>

#include "Windows.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef NDEBUG
constexpr auto DEBUG = false;
#else
constexpr auto DEBUG = true;
#endif

#include <chrono>
#include <string>
#include <vector>

constexpr auto THRESHOLD = 0.001f;
constexpr auto FIRE_TEMP = 90.0f;
constexpr auto WALL_TEMP = 0.0f;
constexpr auto FIRE_SIZE = 80;
constexpr auto MAX_ITERATIONS = 2000;

extern int size; // problem size
extern int len; // size * size
extern std::vector<float> data;
extern std::vector<float> new_data;
extern std::vector<bool> fire_area;
#ifdef GUI
extern std::vector<GLubyte> pixels;
#endif

using std_clock = std::chrono::high_resolution_clock;
using time_point = std_clock::time_point;

void initialize();

void generate_fire_area(int begin, int end);

void update(int begin, int end);

// maintain the temperature of wall
void maintain_wall(int begin, int end);

void split_data(std::vector<int>& counts, std::vector<int>& displs, int total_count,
                int num_partitions);

void print_info(const std::string& prog_name, int num_cores);

#ifdef GUI
void glut_init(int argc, char** argv, const std::string& prog_name);

// visualize temperature distribution
void plot(const std::vector<float>& data);

void data2pixels(const std::vector<float>& data, std::vector<GLubyte>& pixels);

void plot(const std::vector<GLubyte>& pixels);
#endif
