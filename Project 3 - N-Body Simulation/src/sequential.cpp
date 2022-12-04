#include <chrono>
#include <iostream>

#include "common.h"

void update_positions(const std::vector<double>& m, std::vector<Point>& pos,
                      std::vector<Velocity>& v, const size_t n) {
    for (size_t i = 0; i < n; i++) {
        update_position(m, pos, v, n, i);
    }
}

void update_velocities(const std::vector<double>& m, const std::vector<Point>& pos,
                       std::vector<Velocity>& v, const size_t n) {
    for (size_t i = 0; i < n; i++) {
        update_velocity(m, pos, v, n, i);
    }
}

void master() {
    using namespace std::chrono;

    generate_data(m, pos, v, n_body);

    for (int i = 0; i < n_iteration; i++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        update_velocities(m, pos, v, n_body);
        update_positions(m, pos, v, n_body);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = t2 - t1;

        std::cout << "Iteration " << i << ", elapsed time: " << time_span.count() << '\n';

#ifdef GUI
        glut_update();
#endif
    }
}

int main(int argc, char* argv[]) {
    const std::string prog_name = "Sequential";
    
    n_body = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);

#ifdef GUI
    glut_init(argc, argv, prog_name);
#endif

    TIME_IT(master();)

    print_information(prog_name, n_body, 1);

#ifdef GUI
    glut_main_loop();
#endif

    return 0;
}
