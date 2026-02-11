/**
 * REI SORT - Modern C++20 Hybrid Sorting Algorithm
 *
 * Features:
 * - Header-only template library
 * - Adaptive: sorted/reverse detection
 * - Small arrays → Insertion sort (Binary Search optimized)
 * - Large arrays → Iterative introsort with 3-way partition
 * - Heapsort fallback
 * - Custom comparator support
 * - Move-optimized, cache-friendly
 * - SIMD-friendly memory access patterns
 *
 * NOT stable (like std::sort)
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace rei {

// Configuration constants
inline constexpr std::size_t INSERTION_THRESHOLD = 20;
inline constexpr int INTROSORT_DEPTH_FACTOR = 2;

// ============================================================================
// SORTED/REVERSE DETECTION (Single Pass)
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline std::pair<bool, bool> scan_sorted_and_reverse(RandomIt first, RandomIt last, Compare comp) {
    if (first == last || std::next(first) == last) {
        return {true, true};
    }

    bool is_sorted = true;
    bool is_reverse = true;

    for (auto it = first; std::next(it) != last; ++it) {
        auto next_it = std::next(it);

        if (comp(*next_it, *it)) {  // next < current
            is_sorted = false;
        }
        if (comp(*it, *next_it)) {  // current < next
            is_reverse = false;
        }

        if (!is_sorted && !is_reverse) {
            break;  // Early exit optimization
        }
    }

    return {is_sorted, is_reverse};
}

// ============================================================================
// INSERTION SORT (Optimized for small arrays)
// Uses Binary Search (std::upper_bound) for fewer comparisons
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline void insertion_sort(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;

    for (auto i = std::next(first); i != last; ++i) {
        auto value = std::move(*i);

        // Binary search for insertion point
        // upper_bound returns the first position where value < element is true
        // (i.e., element > value). We insert *before* that.
        auto pos = std::upper_bound(first, i, value, comp);

        // Shift elements to right to make room
        std::move_backward(pos, i, std::next(i));

        *pos = std::move(value);
    }
}

// ============================================================================
// MEDIAN-OF-THREE PIVOT SELECTION
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline RandomIt median_of_three(RandomIt a, RandomIt b, RandomIt c, Compare comp) {
    // Returns iterator to median value
    if (comp(*a, *b)) {
        if (comp(*b, *c)) return b;      // a < b < c
        if (comp(*a, *c)) return c;      // a < c <= b
        return a;                         // c <= a < b
    } else {
        if (comp(*c, *b)) return b;      // c < b <= a
        if (comp(*c, *a)) return c;      // b <= c < a
        return a;                         // b <= a <= c
    }
}

// ============================================================================
// 3-WAY PARTITION (Dutch National Flag)
// Handles duplicates efficiently - major optimization for repeated values
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline std::pair<RandomIt, RandomIt> partition_3way(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return {first, first};

    auto size = std::distance(first, last);
    auto mid = std::next(first, size / 2);
    auto pivot_it = median_of_three(first, mid, std::prev(last), comp);

    // Move pivot to front
    std::iter_swap(first, pivot_it);
    auto pivot = *first;

    // Three-way partition
    auto lt = first;      // Elements < pivot
    auto gt = std::prev(last);  // Elements > pivot
    auto i = std::next(first);

    while (i <= gt) {
        if (comp(*i, pivot)) {
            // *i < pivot
            std::iter_swap(lt, i);
            ++lt;
            ++i;
        } else if (comp(pivot, *i)) {
            // *i > pivot
            std::iter_swap(i, gt);
            --gt;
        } else {
            // *i == pivot
            ++i;
        }
    }

    return {lt, std::next(gt)};
}

// ============================================================================
// HEAPSORT (Fallback for deep recursion)
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline void sift_down(RandomIt first, RandomIt last, RandomIt root, Compare comp) {
    auto size = std::distance(first, last);
    auto root_idx = std::distance(first, root);

    while (2 * root_idx + 1 < size) {
        auto child_idx = 2 * root_idx + 1;
        auto child = std::next(first, child_idx);

        // Choose larger child
        if (child_idx + 1 < size && comp(*child, *std::next(child))) {
            ++child_idx;
            ++child;
        }

        if (!comp(*root, *child)) {
            return;
        }

        std::iter_swap(root, child);
        root = child;
        root_idx = child_idx;
    }
}

template <std::random_access_iterator RandomIt, typename Compare>
inline void heapify(RandomIt first, RandomIt last, Compare comp) {
    auto size = std::distance(first, last);
    if (size <= 1) return;

    // Build heap from bottom up
    for (auto i = (size - 2) / 2; i >= 0; --i) {
        sift_down(first, last, std::next(first, i), comp);
        if (i == 0) break;  // Prevent unsigned underflow
    }
}

template <std::random_access_iterator RandomIt, typename Compare>
inline void heapsort_range(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;

    heapify(first, last, comp);

    auto end = std::prev(last);
    while (end != first) {
        std::iter_swap(first, end);
        sift_down(first, end, first, comp);
        --end;
    }
}

// ============================================================================
// ITERATIVE INTROSORT (Main sorting engine)
// Iterative to avoid stack overflow on large arrays
// ============================================================================

template <std::random_access_iterator RandomIt, typename Compare>
inline void introsort_iterative(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;

    auto total_size = std::distance(first, last);
    int max_depth = INTROSORT_DEPTH_FACTOR * static_cast<int>(std::ceil(std::log2(total_size)));

    // Manual stack for iteration (avoids recursion overhead)
    struct StackFrame {
        RandomIt first;
        RandomIt last;
        int depth;
    };

    std::vector<StackFrame> stack;
    stack.reserve(64);  // Pre-allocate for typical cases
    stack.push_back({first, last, max_depth});

    while (!stack.empty()) {
        auto [lo, hi, depth_limit] = stack.back();
        stack.pop_back();

        while (std::distance(lo, hi) > static_cast<std::ptrdiff_t>(INSERTION_THRESHOLD)) {
            // Fallback to heapsort if too deep
            if (depth_limit == 0) {
                heapsort_range(lo, hi, comp);
                break;
            }

            --depth_limit;
            auto [lt, gt] = partition_3way(lo, hi, comp);

            // Optimize: sort smaller partition first, tail-recurse on larger
            auto left_size = std::distance(lo, lt);
            auto right_size = std::distance(gt, hi);

            if (left_size < right_size) {
                if (left_size > 1) {
                    stack.push_back({lo, lt, depth_limit});
                }
                lo = gt;  // Continue with right partition
            } else {
                if (right_size > 1) {
                    stack.push_back({gt, hi, depth_limit});
                }
                hi = lt;  // Continue with left partition
            }
        }

        // Finish small ranges with insertion sort
        if (std::distance(lo, hi) > 1) {
            insertion_sort(lo, hi, comp);
        }
    }
}

// ============================================================================
// REI SORT - Main API
// ============================================================================

/**
 * rei_sort implementation (detection enabled)
 */
template <std::random_access_iterator RandomIt, typename Compare>
inline void rei_sort_impl(RandomIt first, RandomIt last, Compare comp, bool detect_sorted) {
    auto size = std::distance(first, last);

    if (size <= 1) {
        return;
    }

    // Optional sorted/reverse detection
    if (detect_sorted) {
        auto [is_sorted, is_reverse] = scan_sorted_and_reverse(first, last, comp);

        if (is_sorted) {
            return;  // Already sorted
        }

        if (is_reverse) {
            std::reverse(first, last);  // O(n) reverse
            return;
        }
    }

    // Small array optimization
    if (size <= static_cast<std::ptrdiff_t>(INSERTION_THRESHOLD)) {
        insertion_sort(first, last, comp);
        return;
    }

    // Large array: introsort
    introsort_iterative(first, last, comp);
}

/**
 * Public API: rei_sort with custom comparator
 */
template <std::random_access_iterator RandomIt, typename Compare = std::less<>>
inline void rei_sort(RandomIt first, RandomIt last, Compare comp = Compare{}, bool detect_sorted = true) {
    static_assert(std::random_access_iterator<RandomIt>, "rei_sort requires random access iterator");
    rei_sort_impl(first, last, comp, detect_sorted);
}

/**
 * Convenience: rei_sort for containers
 */
template <typename Container, typename Compare = std::less<>>
inline void rei_sort(Container& container, Compare comp = Compare{}, bool detect_sorted = true) {
    rei_sort(std::begin(container), std::end(container), comp, detect_sorted);
}

/**
 * Convenience: rei_sort with projection (key function)
 * Uses Schwartzian transform (decorate-sort-undecorate) and in-place permutation
 */
template <std::random_access_iterator RandomIt, typename KeyFunc, typename Compare = std::less<>>
inline void rei_sort_by_key(RandomIt first, RandomIt last, KeyFunc key_func, Compare comp = Compare{}) {
    static_assert(std::random_access_iterator<RandomIt>, "rei_sort_by_key requires random access iterator");
    using ValueType = typename std::iterator_traits<RandomIt>::value_type;
    using KeyType = std::invoke_result_t<KeyFunc, ValueType>;

    // Decorate: create pairs of (key, index)
    std::vector<std::pair<KeyType, std::size_t>> decorated;
    auto size = std::distance(first, last);
    decorated.reserve(size);

    std::size_t idx = 0;
    for (auto it = first; it != last; ++it, ++idx) {
        decorated.emplace_back(key_func(*it), idx);
    }

    // Sort by key
    rei_sort(decorated.begin(), decorated.end(),
             [&comp](const auto& a, const auto& b) {
                 return comp(a.first, b.first);
             });

    // Undecorate: in-place permutation (Cycle Decomposition)
    for (size_t i = 0; i < decorated.size(); ++i) {
        size_t current = i;
        // Check if cycle already processed (decorated[current].second == current means it's in correct place)
        if (decorated[current].second != current) { 
            ValueType target_val = std::move(*std::next(first, current)); 
            
            while (true) {
                size_t source = decorated[current].second;
                if (source == i) break; 
                
                *std::next(first, current) = std::move(*std::next(first, source));
                
                // Mark as processed
                decorated[current].second = current; 
                
                current = source;
            }
            *std::next(first, current) = std::move(target_val);
            decorated[current].second = current;
        }
    }
}

} // namespace rei
