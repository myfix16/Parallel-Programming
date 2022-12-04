#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>


void print_arr(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    int num_elements;  // number of elements to be sorted
    num_elements = atoi(argv[1]);  // convert command line argument to num_elements

    int elements[num_elements];  // store elements
    int sorted_elements[num_elements];  // store sorted elements

    std::ifstream input(argv[2]);
    int element;
    int i = 0;
    while (input >> element) {
        elements[i] = element;
        i++;
    }
    std::cout << "seq" << "\n";
    std::cout << "actual number of elements:" << i << std::endl;

    std::copy(elements, elements + num_elements, sorted_elements);
    
    std::chrono::high_resolution_clock::time_point t1, t2;
    std::chrono::duration<double> time_span;
    t1 = std::chrono::high_resolution_clock::now();  // record time

    // Body of odd even sort
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

    // clang-format off
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Student ID: " << "119020038" << std::endl;
    std::cout << "Name: " << "Xi Mao" << std::endl;
    std::cout << "Assignment 1" << std::endl;
    std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
    std::cout << "Input Size: " << num_elements << std::endl;
    std::cout << "Process Number: " << 1 << std::endl;
    
    if (num_elements <= 20) {
        std::cout << "\n";
        std::cout << "Original Array: ";
        print_arr(elements, num_elements);
        std::cout << "\n";
        std::cout << "Sorted Array: ";
        print_arr(sorted_elements, num_elements);
    }
    // clang-format on

    std::ofstream output(argv[2] + std::string(".seq.out"), std::ios_base::out);
    for (int j = 0; j < num_elements; j++) {
        output << sorted_elements[j] << std::endl;
    }

    return 0;
}
