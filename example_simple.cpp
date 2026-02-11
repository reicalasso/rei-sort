/**
 * Rei Sort - Minimal Standalone Example
 * Compile: g++ -std=c++20 -O3 -I reicore -o example example_simple.cpp
 */

#include "reicore/rei_sort.hpp"
#include <iostream>
#include <vector>

int main() {
    // Basic usage
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3};

    std::cout << "Before: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    rei::rei_sort(numbers);

    std::cout << "After:  ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
