/**
 * REI SORT - Example Usage
 *
 * Demonstrates various features of rei_sort:
 * - Basic sorting
 * - Custom comparators
 * - Key-based sorting
 * - Different data types
 */

#include "rei_sort.hpp"
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// Helper Functions
// ============================================================================

template <typename T>
void print_vector(const std::string& label, const std::vector<T>& vec) {
    std::cout << label << ": [";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << vec[i];
    }
    std::cout << "]\n";
}

// ============================================================================
// Example 1: Basic Integer Sorting
// ============================================================================

void example_basic() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 1: Basic Integer Sorting\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    print_vector("Before", data);

    rei::rei_sort(data);
    print_vector("After", data);
}

// ============================================================================
// Example 2: String Sorting
// ============================================================================

void example_strings() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 2: String Sorting\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<std::string> words = {"banana", "apple", "cherry", "date", "apricot"};
    print_vector("Before", words);

    rei::rei_sort(words);
    print_vector("After", words);
}

// ============================================================================
// Example 3: Descending Order (Custom Comparator)
// ============================================================================

void example_descending() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 3: Descending Order\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
    print_vector("Before", data);

    rei::rei_sort(data.begin(), data.end(), std::greater<int>{});
    print_vector("After (descending)", data);
}

// ============================================================================
// Example 4: Sorting by Absolute Value
// ============================================================================

void example_absolute_value() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 4: Sorting by Absolute Value\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<int> data = {-5, 3, -1, 4, -2, 0};
    print_vector("Before", data);

    rei::rei_sort(data.begin(), data.end(), [](int a, int b) {
        return std::abs(a) < std::abs(b);
    });
    print_vector("After (by |x|)", data);
}

// ============================================================================
// Example 5: Sorting Custom Structs
// ============================================================================

struct Person {
    std::string name;
    int age;

    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        return os << "{" << p.name << ", " << p.age << "}";
    }
};

void example_custom_struct() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 5: Sorting Custom Structs by Age\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35},
        {"David", 20}
    };

    print_vector("Before", people);

    // Sort by age using key function
    rei::rei_sort_by_key(people.begin(), people.end(), [](const Person& p) {
        return p.age;
    });

    print_vector("After (by age)", people);
}

// ============================================================================
// Example 6: Sorting Pairs by Second Element
// ============================================================================

void example_pairs() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 6: Sorting Pairs by Second Element\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<std::pair<std::string, int>> data = {
        {"b", 2},
        {"a", 3},
        {"c", 1}
    };

    std::cout << "Before: [";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "(" << data[i].first << ", " << data[i].second << ")";
    }
    std::cout << "]\n";

    rei::rei_sort_by_key(data.begin(), data.end(), [](const auto& p) {
        return p.second;
    });

    std::cout << "After:  [";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "(" << data[i].first << ", " << data[i].second << ")";
    }
    std::cout << "]\n";
}

// ============================================================================
// Example 7: Already Sorted Detection
// ============================================================================

void example_detection() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 7: Already Sorted Detection (O(n))\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<int> sorted_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> reverse_data = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    print_vector("Sorted data", sorted_data);
    rei::rei_sort(sorted_data);  // Will detect and return immediately
    print_vector("After rei_sort (detected as sorted)", sorted_data);

    std::cout << "\n";

    print_vector("Reverse data", reverse_data);
    rei::rei_sort(reverse_data);  // Will detect and reverse
    print_vector("After rei_sort (detected and reversed)", reverse_data);

    std::cout << "\nNote: Both cases run in O(n) time!\n";
}

// ============================================================================
// Example 8: Large Array Performance
// ============================================================================

void example_large_array() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Example 8: Large Array (showing first/last 5 elements)\n";
    std::cout << std::string(60, '=') << "\n";

    std::vector<int> data(10000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<int>(data.size() - i);  // Reverse order
    }

    std::cout << "Before: [";
    for (int i = 0; i < 5; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << data[i];
    }
    std::cout << ", ..., ";
    for (size_t i = data.size() - 5; i < data.size(); ++i) {
        if (i > data.size() - 5) std::cout << ", ";
        std::cout << data[i];
    }
    std::cout << "]\n";

    rei::rei_sort(data);

    std::cout << "After:  [";
    for (int i = 0; i < 5; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << data[i];
    }
    std::cout << ", ..., ";
    for (size_t i = data.size() - 5; i < data.size(); ++i) {
        if (i > data.size() - 5) std::cout << ", ";
        std::cout << data[i];
    }
    std::cout << "]\n";

    std::cout << "Array size: " << data.size() << " elements\n";
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              REI SORT - C++ EXAMPLES                       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";

    example_basic();
    example_strings();
    example_descending();
    example_absolute_value();
    example_custom_struct();
    example_pairs();
    example_detection();
    example_large_array();

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    ALL EXAMPLES COMPLETE                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
