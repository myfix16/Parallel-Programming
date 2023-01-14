#include <cmath>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iomanip>
#include <iostream>

#include "common.h"

constexpr int BLOCK_SIZE = 512;

int num_cuda_threads;

__global__ void generate_fire_area_cuda(bool* device_fire_area, const int size, const int len) {
    const int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx >= len) return;
    const int i = idx / size, j = idx % size;

    constexpr int fire1_r2 = FIRE_SIZE * FIRE_SIZE;
    const int a = i - size / 2;
    const int b = j - size / 2;
    const int r2 = 0.5 * a * a + 0.8 * b * b - 0.5 * a * b;
    if (r2 < fire1_r2) device_fire_area[i * size + j] = true;

    constexpr int fire2_r2 = (FIRE_SIZE / 2) * (FIRE_SIZE / 2);
    const int c = i - 1 * size / 3;
    const int d = j - 1 * size / 3;
    const int r22 = c * c + d * d;
    if (r22 < fire2_r2) device_fire_area[i * size + j] = true;
}

__global__ void update_cuda(const float* device_data, float* device_new_data, 
                            const bool* device_fire_area, const int size, const int len) {
    const int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx - size < 0 || idx + size >= len) return; // to avoid invalid array access

    if (device_fire_area[idx]) device_new_data[idx] = FIRE_TEMP;
    else {
        const float up = device_data[idx - size];
        const float down = device_data[idx + size];
        const float left = device_data[idx - 1];
        const float right = device_data[idx + 1];
        const float new_val = (up + down + left + right) / 4;
        device_new_data[idx] = new_val;
    }
}

__global__ void maintain_wall_cuda(float* device_new_data, const int size, const int len) {
    const int t_idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (t_idx >= size) return;

    device_new_data[t_idx] = WALL_TEMP;
    device_new_data[t_idx * size] = WALL_TEMP;
    device_new_data[t_idx * size + size - 1] = WALL_TEMP;
    device_new_data[len - size + t_idx] = WALL_TEMP;
}

void master() {
    float* device_data;
    float* device_new_data;
    bool* device_fire_area;
    bool* continue_arr;
    //! to copy a variable from device to host via cudaMemcpy, you MUST allocate the variable via
    //! cudaMalloc!
    bool* device_cont;

    initialize();

    // calculate number of CUDA threads to use
    const int num_blocks = len % BLOCK_SIZE ? len / BLOCK_SIZE + 1 : len / BLOCK_SIZE;
    num_cuda_threads = num_blocks * BLOCK_SIZE;

    cudaMalloc(&device_data, len * sizeof(float));
    cudaMalloc(&device_new_data, len * sizeof(float));
    cudaMalloc(&device_fire_area, len * sizeof(bool));
    cudaMalloc(&continue_arr, len * sizeof(bool));
    cudaMalloc(&device_cont, sizeof(bool));

    generate_fire_area_cuda<<<num_blocks, BLOCK_SIZE>>>(device_fire_area, size, len);

    // copy data from host to device
    cudaMemcpy(device_data, data.data(), len * sizeof(float), cudaMemcpyHostToDevice);
    
    int count = 1;
    double total_time = 0;

    std::cout << std::fixed << std::setprecision(6);
    while (count <= MAX_ITERATIONS) {
        time_point t1 = std_clock::now();

        update_cuda<<<num_blocks, BLOCK_SIZE>>>(device_data, device_new_data, device_fire_area, size, len);
        maintain_wall_cuda<<<num_blocks, BLOCK_SIZE>>>(device_new_data, size, len);

        // copy data from device to host and swap data and new_data
#ifdef GUI
        cudaMemcpy(data.data(), device_new_data, len * sizeof(float), cudaMemcpyDeviceToHost);
#endif
        // flip data and new_data after one iteration to avoid copying data
        std::swap(device_data, device_new_data);

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

    cudaFree(device_data);
    cudaFree(device_new_data);
    cudaFree(device_fire_area);
    cudaFree(continue_arr);
    cudaFree(&device_cont);
}

int main(const int argc, char* argv[]) {
    const std::string name = "CUDA";

    size = std::stoi(argv[1]);

#ifdef GUI
    glut_init(argc, argv, name);
#endif

    master();

    print_info(name, num_cuda_threads);

    return 0;
}
