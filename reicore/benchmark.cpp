/**
 * REI SORT - Benchmark Suite
 *
 * Compares rei_sort vs std::sort on various data patterns:
 * - Random
 * - Already sorted
 * - Reverse sorted
 * - Few unique (many duplicates)
 * - Nearly sorted
 */

#include "rei_sort.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <iomanip>

using namespace std::chrono;

// ============================================================================
// Benchmark Utilities
// ============================================================================

template <typename Func>
double measure_time_ms(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, std::milli>(end - start).count();
}

template <typename T>
std::vector<T> generate_random(size_t n, T min_val, T max_val, unsigned seed = 42) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<T> dist(min_val, max_val);

    std::vector<T> data;
    data.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        data.push_back(dist(rng));
    }
    return data;
}

template <typename T>
std::vector<T> generate_sorted(size_t n) {
    std::vector<T> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<T>(i);
    }
    return data;
}

template <typename T>
std::vector<T> generate_reverse(size_t n) {
    std::vector<T> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = static_cast<T>(n - i - 1);
    }
    return data;
}

template <typename T>
std::vector<T> generate_few_unique(size_t n, size_t unique_count, unsigned seed = 42) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<T> dist(0, unique_count - 1);

    std::vector<T> data;
    data.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        data.push_back(dist(rng));
    }
    return data;
}

template <typename T>
std::vector<T> generate_nearly_sorted(size_t n, size_t swaps = 10, unsigned seed = 42) {
    auto data = generate_sorted<T>(n);
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<size_t> dist(0, n - 1);

    for (size_t i = 0; i < swaps; ++i) {
        size_t a = dist(rng);
        size_t b = dist(rng);
        std::swap(data[a], data[b]);
    }
    return data;
}

// ============================================================================
// Verification
// ============================================================================

template <typename T>
bool is_sorted(const std::vector<T>& data) {
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] < data[i - 1]) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Benchmark Runner
// ============================================================================

template <typename T>
void run_benchmark(const std::string& name, std::vector<T> data, bool verify = true) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << name << " (n = " << data.size() << ")\n";
    std::cout << std::string(60, '=') << "\n";

    // rei_sort (with detection)
    {
        auto data_copy = data;
        double time = measure_time_ms([&]() {
            rei::rei_sort(data_copy.begin(), data_copy.end());
        });
        std::cout << "rei_sort:        " << std::fixed << std::setprecision(3)
                  << std::setw(10) << time << " ms";
        if (verify && !is_sorted(data_copy)) {
            std::cout << " [FAILED]";
        }
        std::cout << "\n";
    }

    // rei_sort (no detection)
    {
        auto data_copy = data;
        double time = measure_time_ms([&]() {
            rei::rei_sort(data_copy.begin(), data_copy.end(), std::less<T>{}, false);
        });
        std::cout << "rei_sort (no detect): " << std::fixed << std::setprecision(3)
                  << std::setw(10) << time << " ms";
        if (verify && !is_sorted(data_copy)) {
            std::cout << " [FAILED]";
        }
        std::cout << "\n";
    }

    // std::sort
    {
        auto data_copy = data;
        double time = measure_time_ms([&]() {
            std::sort(data_copy.begin(), data_copy.end());
        });
        std::cout << "std::sort:       " << std::fixed << std::setprecision(3)
                  << std::setw(10) << time << " ms";
        if (verify && !is_sorted(data_copy)) {
            std::cout << " [FAILED]";
        }
        std::cout << "\n";
    }

    // std::stable_sort (for comparison)
    {
        auto data_copy = data;
        double time = measure_time_ms([&]() {
            std::stable_sort(data_copy.begin(), data_copy.end());
        });
        std::cout << "std::stable_sort: " << std::fixed << std::setprecision(3)
                  << std::setw(10) << time << " ms";
        if (verify && !is_sorted(data_copy)) {
            std::cout << " [FAILED]";
        }
        std::cout << "\n";
    }
}

// ============================================================================
// Main Benchmark Suite
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          REI SORT - PERFORMANCE BENCHMARK SUITE           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";

    // Configuration
    const size_t small_size = 1000;
    const size_t medium_size = 100'000;
    const size_t large_size = 1'000'000;

    // Small arrays
    run_benchmark("Small Random (int)", generate_random<int>(small_size, 0, 1000));
    run_benchmark("Small Sorted (int)", generate_sorted<int>(small_size));
    run_benchmark("Small Reverse (int)", generate_reverse<int>(small_size));

    // Medium arrays
    run_benchmark("Medium Random (int)", generate_random<int>(medium_size, 0, 1'000'000));
    run_benchmark("Medium Sorted (int)", generate_sorted<int>(medium_size));
    run_benchmark("Medium Reverse (int)", generate_reverse<int>(medium_size));
    run_benchmark("Medium Few Unique (int)", generate_few_unique<int>(medium_size, 10));
    run_benchmark("Medium Nearly Sorted (int)", generate_nearly_sorted<int>(medium_size, 100));

    // Large arrays
    run_benchmark("Large Random (int)", generate_random<int>(large_size, 0, 10'000'000));
    run_benchmark("Large Sorted (int)", generate_sorted<int>(large_size));
    run_benchmark("Large Reverse (int)", generate_reverse<int>(large_size));
    run_benchmark("Large Few Unique (int)", generate_few_unique<int>(large_size, 100));

    // Double precision
    run_benchmark("Medium Random (double)", generate_random<int64_t>(medium_size, 0, 1'000'000));

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   BENCHMARK COMPLETE                       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
