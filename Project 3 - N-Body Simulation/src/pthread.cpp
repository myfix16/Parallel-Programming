#include <chrono>
#include <iostream>
#include <pthread.h>

#include "common.h"

struct Args {
    int begin;
    int end;
    int n_iterations;
};

int n_thd; // number of threads

pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t inner_barrier;
pthread_barrier_t barrier;


void update_positions(const std::vector<double>& m, std::vector<Point>& pos,
                      std::vector<Velocity>& v, const size_t n, const size_t begin,
                      const size_t end) {
    for (size_t i = begin; i < end; ++i) {
        // X = X + V * dt
        pos[i] += v[i] * DT;
        // handle wall collision
        handle_wall_collision(pos[i], v[i]);
        // handle collision with other bodies and bounce
        for (size_t j = 0; j < n; ++j) {
            if (i == j) continue;
            if (pos[i].sqr_dist(pos[j]) < COLLISION_DIST2) {
                pthread_mutex_lock(&data_mutex);
                do_bounce(m[i], m[j], pos[i], pos[j], v[i], v[j]);
                pthread_mutex_unlock(&data_mutex);
            }
        }
    }
}

void update_velocities(const std::vector<double>& m, const std::vector<Point>& pos,
                       std::vector<Velocity>& v, const size_t n, const size_t begin,
                       const size_t end) {
    for (size_t i = begin; i < end; ++i) {
        update_velocity(m, pos, v, n, i);
    }
}

void* worker(void* args) {
    const auto [begin, end, n_iterations] = *static_cast<Args*>(args);
    // waiting for a start signal
    for (int i = 0; i < n_iterations; ++i) {
        update_velocities(m, pos, v, n_body, begin, end);
        pthread_barrier_wait(&inner_barrier);
        update_positions(m, pos, v, n_body, begin, end);
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(nullptr);
    return nullptr;
}

void master() {
    using namespace std::chrono;

    generate_data(m, pos, v, n_body);

    pthread_barrier_init(&inner_barrier, nullptr, n_thd);
    pthread_barrier_init(&barrier, nullptr, n_thd + 1);

    std::vector<int> counts(n_thd);
    std::vector<int> displs(n_thd + 1);
    split_data(counts, displs, n_body, n_thd);

    // initialize thread args
    std::vector<Args> args(n_thd);
    for (size_t i = 0; i < n_thd; ++i) {
        args[i].begin = displs[i];
        args[i].end = displs[i + 1];
        args[i].n_iterations = n_iteration;
    }

    // create threads
    std::vector<pthread_t> threads(n_thd);
    for (int i = 0; i < n_thd; ++i) {
        pthread_create(&threads[i], nullptr, worker, &args[i]);
    }

    for (int i = 0; i < n_iteration; i++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        // a new iteration begins when every threads reach the barrier
        pthread_barrier_wait(&barrier);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = t2 - t1;

        std::cout << "Iteration " << i << ", elapsed time: " << time_span.count() << '\n';

#ifdef GUI
        glut_update();
#endif
    }

    // wait for all threads to exit
    for (const pthread_t& thread : threads) {
        pthread_join(thread, nullptr);
    }

    pthread_mutex_destroy(&data_mutex);
    pthread_barrier_destroy(&inner_barrier);
    pthread_barrier_destroy(&barrier);
}


int main(int argc, char* argv[]) {
    const std::string prog_name = "Pthreads";

    n_body = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);
    n_thd = std::stoi(argv[3]);

#ifdef GUI
    glut_init(argc, argv, prog_name);
#endif

    TIME_IT(master();)

    print_information(prog_name, n_body, n_thd);

#ifdef GUI
    glut_main_loop();
#endif

    return 0;
}
