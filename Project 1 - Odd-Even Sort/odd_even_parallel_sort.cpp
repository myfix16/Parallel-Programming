#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>


void print_arr(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void odd_even_sort(int* my_elements, const int my_rank, const int my_size, 
                   const int total_num_elements, const int world_size, MPI_Comm comm) {
    // the sort is guaranteed to finish in `total_num_elements` iterations
    for (int i = 0; i < total_num_elements; i++) {
        // do local odd even sort
        for (int j = i % 2; j < my_size - 1; j += 2) {
            if (my_elements[j] > my_elements[j + 1]) {
                std::swap(my_elements[j], my_elements[j + 1]);
            }
        }
        
        // do inter-process communication (and number swap) at odd passes
        if (i % 2 != 0) {
            int send_num, recv_num;

            // if not the first process, send the first element to the left
            if (my_rank != 0) {
                send_num = my_elements[0];
                MPI_Send(&send_num, 1, MPI_INT, my_rank - 1, 0, comm);
                MPI_Recv(&recv_num, 1, MPI_INT, my_rank - 1, 0, comm, MPI_STATUS_IGNORE);
                if (recv_num > my_elements[0]) {
                    my_elements[0] = recv_num;
                }
            }
            // if not the last process, send the last element to the right
            if (my_rank != world_size - 1) {
                send_num = my_elements[my_size - 1];
                MPI_Recv(&recv_num, 1, MPI_INT, my_rank + 1, 0, comm, MPI_STATUS_IGNORE);
                MPI_Send(&send_num, 1, MPI_INT, my_rank + 1, 0, comm);
                if (recv_num < my_elements[my_size - 1]) {
                    my_elements[my_size - 1] = recv_num;
                }
            }
        }
    }
}

void seq_odd_even_sort(int* sorted_elements, int num_elements) {
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        // alternate odd and even passes
        for (int offset = 0; offset < 2; ++offset) {
            // do the actual odd/even sort
            for (int j = offset; j < num_elements - 1; j += 2) {
                if (sorted_elements[j] > sorted_elements[j + 1]) {
                    std::swap(sorted_elements[j], sorted_elements[j + 1]);
                    sorted = false;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int num_elements; // number of elements to be sorted

    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int elements[num_elements]; // store elements
    int sorted_elements[num_elements]; // store sorted elements

    if (rank == 0) { // read inputs from file (master process)
        std::ifstream input(argv[2]);
        int element;
        int i = 0;
        while (input >> element) {
            elements[i++] = element;
        }
        std::cout << "mpi" << "\n";
        std::cout << "actual number of elements:" << i << std::endl;
    }

    std::chrono::high_resolution_clock::time_point t1, t2;
    std::chrono::duration<double> time_span;
    if (rank == 0) {
        // if array size < number of processors, do local odd-even sort in node 0
        if (num_elements < world_size) {
            std::copy(elements, elements + num_elements, sorted_elements);
            t1 = std::chrono::high_resolution_clock::now();
            seq_odd_even_sort(sorted_elements, num_elements);
        }
        else {
            t1 = std::chrono::high_resolution_clock::now(); // record time
        }
    }

    if (num_elements >= world_size) {
        // calculate the number of elements each process will sort
        // here, if num_elements is not divisible by world_size,
        // we distribute the remaining elements evenly to processes
        int quotient = num_elements / world_size;
        int remainder = num_elements % world_size;

        std::vector<int> send_counts(world_size);
        for (int i = 0; i < world_size; i++) {
            send_counts[i] = quotient;
            if (i < remainder) {
                send_counts[i]++;
            }
        }

        std::vector<int> displs(world_size);
        displs[0] = 0;
        for (int i = 1; i < world_size; i++) {
            displs[i] = displs[i - 1] + send_counts[i - 1];
        }


        int* my_elements = new int[quotient + 1];

        // distribute elements to each process
        MPI_Scatterv(
            elements, send_counts.data(), displs.data(), MPI_INT, 
            my_elements, send_counts[rank], MPI_INT, 0, MPI_COMM_WORLD
        );

        // do the odd-even sort
        odd_even_sort(my_elements, rank, send_counts[rank], num_elements, world_size, MPI_COMM_WORLD);


        // collect result from each process
        MPI_Gatherv(
            my_elements, send_counts[rank], MPI_INT, 
            sorted_elements, send_counts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD
        ); 
    }

    // clang-format off
    if (rank == 0){ // record time (only executed in master process)
        t2 = std::chrono::high_resolution_clock::now();  
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Student ID: " << "119020038" << std::endl;
        std::cout << "Name: " << "Xi Mao" << std::endl;
        std::cout << "Assignment 1" << std::endl;
        std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
        std::cout << "Input Size: " << num_elements << std::endl;
        std::cout << "Process Number: " << world_size << std::endl; 
        
        if (num_elements <= 20) {
            std::cout << "\n";
            std::cout << "Original Array: ";
            print_arr(elements, num_elements);
            std::cout << "\n";
            std::cout << "Sorted Array: ";
            print_arr(sorted_elements, num_elements);
        }
    }
    // clang-format on

    if (rank == 0) { // write result to file (only executed in master process)
        std::ofstream output(argv[2] + std::string(".parallel.out"), std::ios_base::out);
        for (int i = 0; i < num_elements; i++) {
            output << sorted_elements[i] << std::endl;
        }
    }

    MPI_Finalize();

    return 0;
}
