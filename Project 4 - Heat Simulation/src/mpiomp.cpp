#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <omp.h>

#include "common.h"

int rank, world_size;
int n_omp_threads;
std::vector<int> counts;
std::vector<int> displs;

void update_omp(const int begin, const int end) {
    #pragma omp parallel for
    for (int i = begin; i < end; i++) {
        for (int j = 1; j < size - 1; j++) {
            const int idx = i * size + j;
            if (fire_area[idx]) new_data[idx] = FIRE_TEMP;
            else {
                const float up = data[idx - size];
                const float down = data[idx + size];
                const float left = data[idx - 1];
                const float right = data[idx + 1];
                const float new_val = (up + down + left + right) / 4;
                new_data[idx] = new_val;
            }
        }
    }
}

// maintain the temperature of fire
void maintain_fire_omp() {
    #pragma omp parallel for
    for (int i = 0; i < len; i++) {
        if (fire_area[i]) new_data[i] = FIRE_TEMP;
    }
}

// maintain the temperature of wall
void maintain_wall_omp() {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        new_data[i] = WALL_TEMP;
        new_data[i * size] = WALL_TEMP;
        new_data[i * size + size - 1] = WALL_TEMP;
        new_data[len - size + i] = WALL_TEMP;
    }
}

void do_one_iteration() {
    update_omp(displs[rank], displs[rank + 1]);

    // send the last row to next rank
    if (rank != world_size - 1) {
        float* last_neighbor_row = &new_data[displs[rank + 1] * size];
        float* last_row = last_neighbor_row - size;
        MPI_Send(last_row, size, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Recv(last_neighbor_row, size, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    // send the previous row the previous rank
    if (rank != 0) {
        float* first_row = &new_data[displs[rank] * size];
        float* first_neighbor_row = first_row - size;
        MPI_Recv(first_neighbor_row, size, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(first_row, size, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD);
    }
    
    maintain_wall_omp();

    std::swap(data, new_data);
}

void slave() {
    int count = 1;
    while (count++ <= MAX_ITERATIONS) {
        do_one_iteration();
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void master() {
    int count = 1;
    double total_time = 0;

    std::cout << std::fixed << std::setprecision(6);
    while (count <= MAX_ITERATIONS) {
        time_point t1 = std_clock::now();

        do_one_iteration();
        MPI_Barrier(MPI_COMM_WORLD);

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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const std::string name = "MPI-OpenMP";

    size = std::stoi(argv[1]);
    n_omp_threads = std::stoi(argv[2]);

    omp_set_num_threads(n_omp_threads);

    // initialize the data
    initialize();
    generate_fire_area(0, size);

    // split the data
    counts.resize(world_size);
    displs.resize(world_size + 1);
    split_data(counts, displs, size - 2, world_size);
    for (int& i : displs) {
        i++;
    }

    if (rank == 0) {
#ifdef GUI
        glut_init(argc, argv, name);
#endif
        master();

        print_info(name, world_size);
    }
    else
        slave();

    MPI_Finalize();

    return 0;
}
