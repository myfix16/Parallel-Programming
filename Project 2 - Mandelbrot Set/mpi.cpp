#include "asg2.h"
#include <cstddef>
#include <vector>
#include <mpi.h>


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (argc == 4) {
        X_RESN = atoi(argv[1]);
        Y_RESN = atoi(argv[2]);
        max_iteration = atoi(argv[3]);
    } else {
        X_RESN = 800;
        Y_RESN = 800;
        max_iteration = 100;
    }

    total_size = X_RESN * Y_RESN;

    /* create a MPI data type for struct Point */
    MPI_Datatype MPI_POINT;
    int blocklengths[] = {1, 1, 1};
    MPI_Aint displacements[] = {offsetof(Point, x), offsetof(Point, y), offsetof(Point, color)};
    MPI_Datatype types[] = {MPI_INT, MPI_INT, MPI_FLOAT};
    MPI_Type_create_struct(3, blocklengths, displacements, types, &MPI_POINT);
    MPI_Type_commit(&MPI_POINT);

    if (rank == 0) {
#ifdef GUI
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
        glutInitWindowSize(500, 500);
        glutInitWindowPosition(0, 0);
        glutCreateWindow("MPI");
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(0, X_RESN, 0, Y_RESN);
        glutDisplayFunc(plot);
#endif

        t1 = std::chrono::high_resolution_clock::now();
        init_data();
    }

    // partition the data
    const int quotient = total_size / world_size;
    const int remainder = total_size % world_size;
    std::vector<int> send_counts(world_size);
    std::vector<int> displs(world_size);

    for (int i = 0; i < world_size; i++) {
        send_counts[i] = quotient;
        if (i < remainder) send_counts[i]++;
    }

    displs[0] = 0;
    for (int i = 1; i < world_size; i++)
        displs[i] = displs[i - 1] + send_counts[i - 1];

    auto *sub_arr = new Point[quotient + 1];

    // distribute elements to each process
    MPI_Scatterv(
        data, send_counts.data(), displs.data(), MPI_POINT,
        sub_arr, send_counts[rank], MPI_POINT, 0, MPI_COMM_WORLD
    );

    // compute a block of points
    compute_block(sub_arr, sub_arr + send_counts[rank]);

    // collect result from each process
    MPI_Gatherv(
        sub_arr, send_counts[rank], MPI_POINT,
        data, send_counts.data(), displs.data(), MPI_POINT, 0, MPI_COMM_WORLD
    );

    // clean up
    delete[] sub_arr;

    if (rank == 0) {
        t2 = std::chrono::high_resolution_clock::now();
        time_span = t2 - t1;

        printf("Student ID: 119020038\n");
        printf("Name: Xi Mao\n");
        printf("Assignment 2: MPI\n");
        printf("Run Time: %f seconds\n", time_span.count());
        printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
        printf("Processing Speed: %f pixels/s\n", total_size / time_span.count());
        printf("Process Number: %d\n", world_size);

#ifdef GUI
        glutMainLoop();
#endif
    }

    MPI_Finalize();

    return 0;
}

