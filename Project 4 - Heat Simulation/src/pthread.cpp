#include <iomanip>
#include <iostream>
#include <pthread.h>

#include "common.h"

int num_threads;
pthread_barrier_t barrier;
pthread_barrier_t worker_barrier;

struct Args {
    int begin[2];
    int end[2];
};

void* worker(void* args) {
    auto [begin, end] = *static_cast<Args*>(args);
    // initialize fire area
    generate_fire_area(begin[1], end[1]);
    pthread_barrier_wait(&barrier);

    int count = 1;
    while (count++ <= MAX_ITERATIONS) {
        // update temperature
        update(begin[0], end[0]);
        pthread_barrier_wait(&worker_barrier);

        maintain_wall(begin[1], end[1]);
        pthread_barrier_wait(&barrier); // wait for one iteration to finish

        pthread_barrier_wait(&barrier); // wait until check_continue and std::swap finish
    }

    pthread_exit(nullptr);
    return nullptr;
}

void master() {
    pthread_barrier_init(&barrier, nullptr, num_threads + 1);
    pthread_barrier_init(&worker_barrier, nullptr, num_threads);

    initialize();

    // prepare args
    std::vector<Args> args(num_threads);
    // split data for update()
    std::vector<int> counts(num_threads);
    std::vector<int> displs(num_threads + 1);
    split_data(counts, displs, size - 2, num_threads);
    for (int& i : displs) {
        i++;
    }
    for (int i = 0; i < num_threads; ++i) {
        args[i].begin[0] = displs[i];
        args[i].end[0] = displs[i + 1];
    }
    // split data for maintain_wall()
    split_data(counts, displs, size, num_threads);
    for (int i = 0; i < num_threads; ++i) {
        args[i].begin[1] = displs[i];
        args[i].end[1] = displs[i + 1];
    }

    std::vector<pthread_t> threads(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], nullptr, worker, &args[i]);
    }

    int count = 1;
    double total_time = 0;

    std::cout << std::fixed << std::setprecision(6);
    pthread_barrier_wait(&barrier); // wait for initialization
    while (count <= MAX_ITERATIONS) {
        time_point t1 = std_clock::now();

        pthread_barrier_wait(&barrier); // wait for one iteration to finish
        
        std::swap(data, new_data);
        pthread_barrier_wait(&barrier); // threads wait for swap to finish

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

    // wait for all threads to end
    for (const pthread_t& thread : threads) {
        pthread_join(thread, nullptr);
    }

    pthread_barrier_destroy(&barrier);
    pthread_barrier_destroy(&worker_barrier);

    std::cout << "Converge after " << count - 1 << " iterations, elapsed time: " << total_time
              << ", average computation time: " << total_time / (count - 1) << std::endl;
}

int main(const int argc, char* argv[]) {
    const std::string name = "Pthreads";

    size = std::stoi(argv[1]);
    num_threads = std::stoi(argv[2]);

#ifdef GUI
    glut_init(argc, argv, name);
#endif

    master();

    print_info(name, num_threads);

    return 0;
}
