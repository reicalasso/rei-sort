/**
 * REI SORT - Unit Tests
 *
 * Comprehensive test suite covering:
 * - Edge cases (empty, single element)
 * - Already sorted/reverse
 * - Random data
 * - Duplicates
 * - Custom comparators
 * - Key-based sorting
 */

#include "rei_sort.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <cassert>

// ============================================================================
// Test Utilities
// ============================================================================

int test_count = 0;
int test_passed = 0;

#define TEST(name) \
    ++test_count; \
    std::cout << "Testing: " << #name << " ... "; \
    if (test_##name()) { \
        ++test_passed; \
        std::cout << "✓ PASS\n"; \
    } else { \
        std::cout << "✗ FAIL\n"; \
    }

template <typename T>
bool vectors_equal(const std::vector<T>& a, const std::vector<T>& b) {
    return a == b;
}

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
// Basic Tests
// ============================================================================

bool test_empty() {
    std::vector<int> data;
    rei::rei_sort(data);
    return data.empty();
}

bool test_single_element() {
    std::vector<int> data = {42};
    rei::rei_sort(data);
    return data == std::vector<int>{42};
}

bool test_two_elements_sorted() {
    std::vector<int> data = {1, 2};
    rei::rei_sort(data);
    return data == std::vector<int>{1, 2};
}

bool test_two_elements_unsorted() {
    std::vector<int> data = {2, 1};
    rei::rei_sort(data);
    return data == std::vector<int>{1, 2};
}

bool test_already_sorted() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    rei::rei_sort(data);
    return data == std::vector<int>{1, 2, 3, 4, 5};
}

bool test_reverse_sorted() {
    std::vector<int> data = {5, 4, 3, 2, 1};
    rei::rei_sort(data);
    return data == std::vector<int>{1, 2, 3, 4, 5};
}

bool test_all_equal() {
    std::vector<int> data = {3, 3, 3, 3, 3};
    rei::rei_sort(data);
    return data == std::vector<int>{3, 3, 3, 3, 3};
}

// ============================================================================
// Random Data Tests
// ============================================================================

bool test_random_small() {
    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    rei::rei_sort(data);
    auto expected = data;
    std::sort(expected.begin(), expected.end());
    return vectors_equal(data, expected) && is_sorted(data);
}

bool test_random_medium() {
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> dist(0, 1000);

    std::vector<int> data(1000);
    for (auto& x : data) {
        x = dist(rng);
    }

    auto expected = data;
    rei::rei_sort(data);
    std::sort(expected.begin(), expected.end());

    return vectors_equal(data, expected) && is_sorted(data);
}

bool test_random_large() {
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<int> dist(0, 1'000'000);

    std::vector<int> data(100'000);
    for (auto& x : data) {
        x = dist(rng);
    }

    rei::rei_sort(data);
    return is_sorted(data);
}

// ============================================================================
// Duplicate Tests
// ============================================================================

bool test_many_duplicates() {
    std::mt19937_64 rng(999);
    std::uniform_int_distribution<int> dist(0, 5);  // Only 6 unique values

    std::vector<int> data(1000);
    for (auto& x : data) {
        x = dist(rng);
    }

    rei::rei_sort(data);
    return is_sorted(data);
}

bool test_all_duplicates_but_one() {
    std::vector<int> data(1000, 42);
    data[500] = 1;  // One different element
    rei::rei_sort(data);

    return data[0] == 1 && data[1] == 42 && data.back() == 42 && is_sorted(data);
}

// ============================================================================
// String Tests
// ============================================================================

bool test_strings() {
    std::vector<std::string> data = {"banana", "apple", "cherry", "date", "apricot"};
    rei::rei_sort(data);

    std::vector<std::string> expected = {"apple", "apricot", "banana", "cherry", "date"};
    return vectors_equal(data, expected);
}

bool test_strings_with_duplicates() {
    std::vector<std::string> data = {"b", "a", "c", "a", "b"};
    rei::rei_sort(data);

    return is_sorted(data) && data[0] == "a" && data[1] == "a";
}

// ============================================================================
// Custom Comparator Tests
// ============================================================================

bool test_custom_comparator_descending() {
    std::vector<int> data = {1, 5, 3, 9, 2};
    rei::rei_sort(data.begin(), data.end(), std::greater<int>{});

    return data == std::vector<int>{9, 5, 3, 2, 1};
}

bool test_custom_comparator_abs() {
    std::vector<int> data = {-5, 3, -1, 4, -2};

    rei::rei_sort(data.begin(), data.end(), [](int a, int b) {
        return std::abs(a) < std::abs(b);
    });

    // Expected: sorted by absolute value
    return std::abs(data[0]) <= std::abs(data[1]) &&
           std::abs(data[1]) <= std::abs(data[2]) &&
           std::abs(data[2]) <= std::abs(data[3]) &&
           std::abs(data[3]) <= std::abs(data[4]);
}

// ============================================================================
// Key-based Sorting Tests (Schwartzian Transform)
// ============================================================================

bool test_sort_by_key() {
    struct Person {
        std::string name;
        int age;
    };

    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35},
        {"David", 20}
    };

    // Sort by age
    rei::rei_sort_by_key(people.begin(), people.end(),
                         [](const Person& p) { return p.age; });

    return people[0].name == "David" && people[0].age == 20 &&
           people[1].name == "Bob" && people[1].age == 25 &&
           people[2].name == "Alice" && people[2].age == 30 &&
           people[3].name == "Charlie" && people[3].age == 35;
}

bool test_sort_pairs_by_second() {
    std::vector<std::pair<std::string, int>> data = {
        {"b", 2},
        {"a", 3},
        {"c", 1}
    };

    rei::rei_sort_by_key(data.begin(), data.end(),
                         [](const auto& p) { return p.second; });

    return data[0].first == "c" && data[0].second == 1 &&
           data[1].first == "b" && data[1].second == 2 &&
           data[2].first == "a" && data[2].second == 3;
}

// ============================================================================
// Edge Cases
// ============================================================================

bool test_nearly_sorted() {
    std::vector<int> data = {1, 2, 3, 5, 4, 6, 7, 8, 9, 10};  // Only one swap needed
    rei::rei_sort(data);

    return is_sorted(data) && data[0] == 1 && data.back() == 10;
}

bool test_large_range() {
    std::vector<int> data = {1000000, 1, 500000, 250000, 750000};
    rei::rei_sort(data);

    return is_sorted(data) && data[0] == 1 && data.back() == 1000000;
}

bool test_negative_numbers() {
    std::vector<int> data = {-5, 3, -1, 0, -3, 2};
    rei::rei_sort(data);

    return data == std::vector<int>{-5, -3, -1, 0, 2, 3};
}

// ============================================================================
// Detection Tests
// ============================================================================

bool test_detection_disabled_sorted() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    rei::rei_sort(data.begin(), data.end(), std::less<int>{}, false);
    return data == std::vector<int>{1, 2, 3, 4, 5};
}

bool test_detection_disabled_reverse() {
    std::vector<int> data = {5, 4, 3, 2, 1};
    rei::rei_sort(data.begin(), data.end(), std::less<int>{}, false);
    return data == std::vector<int>{1, 2, 3, 4, 5};
}

// ============================================================================
// Stability Tests (rei_sort is NOT stable)
// ============================================================================

bool test_unstable_sort() {
    // This test just verifies that rei_sort works correctly
    // It does NOT require stability
    struct Item {
        int key;
        int id;
    };

    std::vector<Item> data = {
        {3, 1}, {1, 2}, {3, 3}, {2, 4}, {1, 5}
    };

    rei::rei_sort(data.begin(), data.end(), [](const Item& a, const Item& b) {
        return a.key < b.key;
    });

    // Just verify keys are sorted
    return data[0].key == 1 && data[1].key == 1 &&
           data[2].key == 2 && data[3].key == 3 && data[4].key == 3;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              REI SORT - UNIT TEST SUITE                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    // Basic tests
    TEST(empty);
    TEST(single_element);
    TEST(two_elements_sorted);
    TEST(two_elements_unsorted);
    TEST(already_sorted);
    TEST(reverse_sorted);
    TEST(all_equal);

    // Random data
    TEST(random_small);
    TEST(random_medium);
    TEST(random_large);

    // Duplicates
    TEST(many_duplicates);
    TEST(all_duplicates_but_one);

    // Strings
    TEST(strings);
    TEST(strings_with_duplicates);

    // Custom comparators
    TEST(custom_comparator_descending);
    TEST(custom_comparator_abs);

    // Key-based sorting
    TEST(sort_by_key);
    TEST(sort_pairs_by_second);

    // Edge cases
    TEST(nearly_sorted);
    TEST(large_range);
    TEST(negative_numbers);

    // Detection tests
    TEST(detection_disabled_sorted);
    TEST(detection_disabled_reverse);

    // Stability
    TEST(unstable_sort);

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Results: " << test_passed << "/" << test_count << " tests passed";
    if (test_passed == test_count) {
        std::cout << " - ALL TESTS PASSED! ✓";
    }
    std::cout << "           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return (test_passed == test_count) ? 0 : 1;
}
