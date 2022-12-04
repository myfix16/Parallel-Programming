#include "asg2.h"
#include <cstdio>


void sequentialCompute() {
    /* compute for all points one by one */
    Point *p = data;
    for (int index = 0; index < total_size; index++) {
        compute(p);
        p++;
    }
}

int main(int argc, char *argv[]) {
    /* pass in metadata for computation */
    if (argc == 4) {
        X_RESN = atoi(argv[1]);
        Y_RESN = atoi(argv[2]);
        max_iteration = atoi(argv[3]);
    } else {
        X_RESN = 800;
        Y_RESN = 800;
        max_iteration = 100;
    }

#ifdef GUI
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Sequential");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, X_RESN, 0, Y_RESN);
    glutDisplayFunc(plot);
#endif

    /* computation part begin */
    t1 = std::chrono::high_resolution_clock::now();
    init_data();
    sequentialCompute();
    t2 = std::chrono::high_resolution_clock::now();
    time_span = t2 - t1;
    /* computation part end */

    printf("Student ID: 119020038\n");
    printf("Name: Xi Mao\n");
    printf("Assignment 2: Sequential\n");
    printf("Run Time: %f seconds\n", time_span.count());
    printf("Problem Size: %d * %d, %d\n", X_RESN, Y_RESN, max_iteration);
    printf("Processing Speed: %f pixels/s\n", total_size / time_span.count());
    printf("Process Number: %d\n", 1);

#ifdef GUI
    glutMainLoop();
#endif

    return 0;
}

