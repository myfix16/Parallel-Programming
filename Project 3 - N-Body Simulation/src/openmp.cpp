#include <chrono>
#include <iostream>
#include <omp.h>

#include "common.h"

int n_omp_threads;


void update_position(const std::vector<double>& m, std::vector<Point>& pos,
                     std::vector<Velocity>& v, const size_t n) {
#pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        // X = X + V * dt
        pos[i] += v[i] * DT;
        // handle wall collision
        handle_wall_collision(pos[i], v[i]);
        // handle collision with other bodies and bounce
        for (size_t j = 0; j < n; ++j) {
            if (i == j) continue;
            if (pos[i].sqr_dist(pos[j]) < COLLISION_DIST2) {
                #pragma omp critical
                { do_bounce(m[i], m[j], pos[i], pos[j], v[i], v[j]); }
            }
        }
    }
}

void update_velocity(const std::vector<double>& m, const std::vector<Point>& pos,
                     std::vector<Velocity>& v, const size_t n) {
#pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        update_velocity(m, pos, v, n, i);
    }
}


void master() {
    using namespace std::chrono;

    generate_data(m, pos, v, n_body);

    omp_set_num_threads(n_omp_threads);
    for (int i = 0; i < n_iteration; i++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        // TODO: choose better threads configuration
        update_velocity(m, pos, v, n_body);
        update_position(m, pos, v, n_body);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = t2 - t1;

        std::cout << "Iteration " << i << ", elapsed time: " << time_span.count() << '\n';

#ifdef GUI
        glut_update();
#endif
    }
}


int main(int argc, char* argv[]) {
    const std::string prog_name = "OpenMP";

    n_body = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);
    n_omp_threads = std::stoi(argv[3]);

#ifdef GUI
    glut_init(argc, argv, prog_name);
#endif

    TIME_IT(master();)

    print_information(prog_name, n_body, n_omp_threads);

#ifdef GUI
    glut_main_loop();
#endif

    return 0;
}
