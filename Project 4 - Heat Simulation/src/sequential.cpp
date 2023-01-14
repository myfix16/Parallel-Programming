#include <iomanip>
#include <iostream>

#include "common.h"


void master() {
    initialize();
    generate_fire_area(0, size);
    
    int count = 1;
    double total_time = 0;

    std::cout << std::fixed << std::setprecision(6);
    while (count <= MAX_ITERATIONS) {
        time_point t1 = std_clock::now();

        update(1, size - 1);
        // maintain_fire(0, len);
        maintain_wall(0, size);

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
    const std::string name = "Sequential";

    size = std::stoi(argv[1]);

#ifdef GUI
    glut_init(argc, argv, name);
#endif

    master();

    print_info(name, 1);

    return 0;
}
