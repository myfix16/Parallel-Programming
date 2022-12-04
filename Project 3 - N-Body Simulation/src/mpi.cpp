#include <chrono>
#include <iostream>
#include <mpi.h>

#include "common.h"

int rank;
int world_size;

std::vector<int> counts;
std::vector<int> displs;

MPI_Datatype MPI_VECTOR; // MPI data type for struct Vector

void update_positions(const std::vector<double>& m, std::vector<Point>& pos,
                      std::vector<Velocity>& v, const size_t n, const size_t begin,
                      const size_t end) {
    for (size_t i = begin; i < end; ++i) {
        update_position(m, pos, v, n, i);
    }
}

void update_velocities(const std::vector<double>& m, const std::vector<Point>& pos,
                       std::vector<Velocity>& v, const size_t n, const size_t begin,
                       const size_t end) {
    for (size_t i = begin; i < end; ++i) {
        update_velocity(m, pos, v, n, i);
    }
}

void do_one_iteration() {
    const int begin = displs[rank], end = displs[rank + 1];

    // update velocities
    update_velocities(m, pos, v, n_body, begin, end);
    // broadcast updated velocities
    for (int i = 0; i < world_size; ++i) {
        MPI_Bcast(&v[displs[i]], counts[i], MPI_VECTOR, i, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // update positions
    update_positions(m, pos, v, n_body, begin, end);
    // broadcast updated positions and velocities
    for (int i = 0; i < world_size; ++i) {
        MPI_Bcast(&pos[displs[i]], counts[i], MPI_VECTOR, i, MPI_COMM_WORLD);
        MPI_Bcast(&v[displs[i]], counts[i], MPI_VECTOR, i, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

void master() {
    using namespace std::chrono;

    generate_data(m, pos, v, n_body);

    // copy data to slaves
    MPI_Bcast(m.data(), n_body, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(pos.data(), n_body, MPI_VECTOR, 0, MPI_COMM_WORLD);
    MPI_Bcast(v.data(), n_body, MPI_VECTOR, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n_iteration; i++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        do_one_iteration();

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = t2 - t1;

        std::cout << "Iteration " << i << ", elapsed time: " << time_span.count() << std::endl;

#ifdef GUI
        glut_update();
#endif
    }
}

void slave() {
    // initialize global variables in slave processes
    m.resize(n_body);
    pos.resize(n_body, Point{});
    v.resize(n_body, Velocity{});
    // receive data from master
    MPI_Bcast(m.data(), n_body, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(pos.data(), n_body, MPI_VECTOR, 0, MPI_COMM_WORLD);
    MPI_Bcast(v.data(), n_body, MPI_VECTOR, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n_iteration; ++i) {
        do_one_iteration();
    }
}

int main(int argc, char* argv[]) {
    const std::string prog_name = "MPI";

    n_body = std::stoi(argv[1]);
    n_iteration = std::stoi(argv[2]);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // create a MPI data type for Vector
    MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_VECTOR);
    MPI_Type_commit(&MPI_VECTOR);

    // initialize vectors for data splitting
    counts.resize(world_size);
    displs.resize(world_size + 1);
    split_data(counts, displs, n_body, world_size);

    if (rank == 0) {
#ifdef GUI
        glut_init(argc, argv, prog_name);
#endif

        TIME_IT(master();)
    }
    else {
        slave();
    }

    if (rank == 0) {
        print_information(prog_name, n_body, world_size);

#ifdef GUI
        glut_main_loop();
#endif
    }

    MPI_Finalize();

    return 0;
}
