#include <iomanip>
#include <iostream>
#include <mpi.h>

#include "common.h"

int rank, world_size;
std::vector<int> counts;
std::vector<int> displs;

void do_one_iteration() {
    update(displs[rank], displs[rank + 1]);
    
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
    
    maintain_wall(0, size);
 
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

    const std::string name = "MPI";

    size = std::stoi(argv[1]);

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
