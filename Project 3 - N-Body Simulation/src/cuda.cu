#include <chrono>
#include <cmath>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "common.h"

constexpr size_t BLOCK_SIZE = 512;

unsigned int total_threads;

thrust::host_vector<double> host_m;
thrust::host_vector<Point> host_pos;

void generate_data_cuda(thrust::host_vector<double>& host_m, thrust::host_vector<Point>& host_pos,
                        const int n) {
    // initialize a random distribution
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> uniform(0, RAND_MAX);
    std::uniform_int_distribution<std::mt19937::result_type> u_mass(1, MAX_MASS);

    for (int i = 0; i < n; i++) {
        host_m[i] = u_mass(rng);
        host_pos[i].x = 2000.0 + uniform(rng) % (BOUND_X / 4);
        host_pos[i].y = 2000.0 + uniform(rng) % (BOUND_Y / 4);
    }
}

__device__ Force get_force_cuda(const double m1, const double m2, const Point& pos1,
                                const Point& pos2) {
    const double dx = pos2.x - pos1.x, dy = pos2.y - pos1.y;
    const double r_sqr = dx * dx + dy * dy;
    const double r = sqrt(r_sqr);
    const double f = GRAVITY_CONST * m1 * m2 / (r_sqr + FLOAT_OP_ERROR);
    return Force{f * dx / r, f * dy / r};
}

__device__ void handle_wall_collision_cuda(Point& pos, Velocity& v) {
    if (pos.x < 0) {
        pos.x = 0;
        v.x = -v.x;
    }
    else if (pos.x > BOUND_X) {
        pos.x = BOUND_X;
        v.x = -v.x;
    }
    if (pos.y < 0) {
        pos.y = 0;
        v.y = -v.y;
    }
    else if (pos.y > BOUND_Y) {
        pos.y = BOUND_Y;
        v.y = -v.y;
    }
}

__global__ void update_positions(const double* m, Point* pos, Velocity* v, const int n) {
    const unsigned int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        // X = X + V * dt
        pos[idx] += v[idx] * DT;
        // handle wall collision
        handle_wall_collision_cuda(pos[idx], v[idx]);
        // handle collision with other bodies and bounce
        for (int j = 0; j < n; ++j) {
            if (idx == j) continue;
            if (pos[idx].sqr_dist(pos[j]) < COLLISION_DIST2) {
                // do bounce
                Velocity& v1 = v[idx];
                Point& pos1 = pos[idx];
                // simple workaround: invert the speed
                v1.x = -v1.x;
                v1.y = -v1.y;
                // move the body away
                pos1 += v1 * DT;
            }
        }
    }
}

__global__ void update_velocities(const double* m, const Point* pos, Velocity* v, const int n) {
    const unsigned int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < n) {
        Force f{};
        // sum up all the forces
        for (int j = 0; j < n; ++j) {
            if (idx == j) continue;
            f += get_force_cuda(m[idx], m[j], pos[idx], pos[j]);
        }
        // v += at
        v[idx] += (f / m[idx]) * DT;
    }
}

void master() {
    using namespace std::chrono;

    host_m.resize(n_body);
    host_pos.resize(n_body);

    generate_data_cuda(host_m, host_pos, n_body);

    // copy data from host to device
    thrust::device_vector<double> device_m = host_m;
    thrust::device_vector<Point> device_pos = host_pos;
    thrust::device_vector<Velocity> device_v(n_body);

    const unsigned int n_block =
        n_body % BLOCK_SIZE == 0 ? n_body / BLOCK_SIZE : n_body / BLOCK_SIZE + 1;

    // split the data to arrange jobs
    total_threads = n_block * BLOCK_SIZE;

    for (int i = 0; i < n_iteration; i++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        update_velocities<<<n_block, BLOCK_SIZE>>>(raw_pointer_cast(device_m.data()),
                                                   raw_pointer_cast(device_pos.data()),
                                                   raw_pointer_cast(device_v.data()), n_body);

        update_positions<<<n_block, BLOCK_SIZE>>>(raw_pointer_cast(device_m.data()),
                                                  raw_pointer_cast(device_pos.data()),
                                                  raw_pointer_cast(device_v.data()), n_body);

        // copy updated position back to host
        host_pos = device_pos;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = t2 - t1;
        std::cout << "Iteration " << i << ", elapsed time: " << time_span.count() << '\n';

#ifdef GUI
        glut_update(host_pos.data(), host_pos.size());
#endif
    }
}

int main(int argc, char* argv[]) {
    const std::string prog_name = "CUDA";

    n_body = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);

#ifdef GUI
    glut_init(argc, argv, prog_name);
#endif

    TIME_IT(master();)

    print_information(prog_name, n_body, total_threads);

    return 0;
}
