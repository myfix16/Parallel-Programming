#include "asg2.h"
#include <cstdio>
#include <vector>
#include <pthread.h>

int n_thd; // number of threads

typedef struct {
    Point *begin, *end;
} Args;


void *worker(void *args) {
    Args *arg = static_cast<Args *>(args);
    compute_block(arg->begin, arg->end);
    return nullptr;
}

int main(int argc, char *argv[]) {

    if (argc == 5) {
        X_RESN = atoi(argv[1]);
        Y_RESN = atoi(argv[2]);
        max_iteration = atoi(argv[3]);
        n_thd = atoi(argv[4]);
    } else {
        X_RESN = 800;
        Y_RESN = 800;
        max_iteration = 100;
        n_thd = 4;
    }

#ifdef GUI
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Pthread");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, X_RESN, 0, Y_RESN);
    glutDisplayFunc(plot);
#endif

    /* computation part begin */
    t1 = std::chrono::high_resolution_clock::now();

    init_data();

    std::vector<pthread_t> thds(n_thd);  // thread poll
    std::vector<Args> args(n_thd);  // arguments for all threads

    // split the data
    const int quotient = total_size / n_thd;
    const int remainder = total_size % n_thd;
    std::vector<int> send_counts(n_thd);
    std::vector<int> displs(n_thd + 1);

    for (int i = 0; i < n_thd; i++) {
        send_counts[i] = quotient;
        if (i < remainder) send_counts[i]++;
    }

    displs[0] = 0;
    for (int i = 1; i <= n_thd; i++) {
        displs[i] = displs[i - 1] + send_counts[i - 1];
    }

    // create threads
    for (int thd = 0; thd < n_thd; thd++) {
        args[thd].begin = data + displs[thd];
        args[thd].end = data + displs[thd + 1];
    }
    for (int thd = 0; thd < n_thd; thd++)
        pthread_create(&thds[thd], nullptr, worker, &args[thd]);
    for (int thd = 0; thd < n_thd; thd++)
        pthread_join(thds[thd], nullptr);

    t2 = std::chrono::high_resolution_clock::now();
    time_span = t2 - t1;
    /* computation part end */

    printf("Student ID: 119020038\n");
    printf("Name: Xi Mao\n");
    printf("Assignment 2: Pthread\n");
    printf("Run Time: %f seconds\n", time_span.count());
    printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
    printf("Processing Speed: %f pixels/s\n", total_size / time_span.count());
    printf("Thread Number: %d\n", n_thd);

#ifdef GUI
    glutMainLoop();
#endif

    return 0;
}

