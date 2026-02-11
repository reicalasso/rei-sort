# Rei Sort

**High-Performance Hybrid Sorting Algorithm** — Modern C++20 implementation with Python bindings

Rei Sort is a comparison-based, general-purpose sorting algorithm that adapts its strategy based on input characteristics. It combines the best of multiple sorting techniques into a single, efficient implementation.

## 🚀 Features

- **Adaptive Strategy Selection**
  - Already sorted → O(n) early exit
  - Reverse sorted → O(n) reverse operation
  - Small arrays (≤20) → Insertion sort (optimized with Binary Search)
  - Large arrays → Iterative introsort

- **Advanced Optimizations**
  - **In-Place Permutation** for key-based sorting (Memory optimized from 2N to N)
  - **3-way partition** (Dutch National Flag) for handling duplicates efficiently
  - **Median-of-3** pivot selection
  - **Heapsort fallback** for worst-case O(n log n) guarantee
  - **Move semantics** for optimal performance
  - **Cache-friendly** memory access patterns
  - **Header-only** C++ library (zero overhead)

- **Flexible API**
  - Uses C++20 Concepts for better type safety
  - Works with any random access iterator
  - Custom comparators support
  - Key-based sorting (Schwartzian transform with cycle decomposition)
  - NumPy array support (Python)
  - Thread-safe (GIL released during sorting)

## 📊 Complexity

| Scenario | Time Complexity | Space Complexity |
|----------|----------------|------------------|
| Already sorted | O(n) | O(1) |
| Reverse sorted | O(n) | O(1) |
| Random/General | O(n log n) | O(log n) |
| Worst case | O(n log n) | O(log n) |

**Note:** Rei Sort is **NOT stable** — equal elements may be reordered (like `std::sort`, unlike `std::stable_sort` or Timsort)

## 🔧 Installation

### C++ (Header-Only)

Simply include the header:

```cpp
#include "rei_sort.hpp"
```

### Python (with C++ backend)

```bash
cd reicore
pip install .
```

Or build from source:

```bash
cd reicore
mkdir build && cd build
cmake ..
make
```

## 📖 Usage

### C++

```cpp
#include "rei_sort.hpp"
#include <vector>
#include <iostream>

int main() {
    // Basic usage
    std::vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6};
    rei::rei_sort(arr);
    // arr is now [1, 1, 2, 3, 4, 5, 6, 9]

    // Custom comparator (descending)
    rei::rei_sort(arr, std::greater<int>{});

    // Sort by absolute value
    std::vector<int> nums = {-5, 3, -1, 4, -2};
    rei::rei_sort(nums, [](int a, int b) {
        return std::abs(a) < std::abs(b);
    });

    // Sort struct by key
    struct Person {
        std::string name;
        int age;
    };

    std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
    rei::rei_sort_by_key(people.begin(), people.end(),
                         [](const Person& p) { return p.age; });

    return 0;
}
```

### Python

```python
from reicore import rei_sort

# Basic usage (in-place)
arr = [3, 1, 4, 1, 5]
rei_sort(arr)
# arr == [1, 1, 3, 4, 5]

# With key function (Schwartzian transform)
pairs = [("b", 2), ("a", 3), ("c", 1)]
rei_sort(pairs, key=lambda x: x[1])
# pairs == [('c', 1), ('b', 2), ('a', 3)]

# NumPy arrays (zero-copy, very fast!)
import numpy as np
arr = np.array([3, 1, 4, 1, 5], dtype=np.int64)
from reicore import rei_sort_numpy
rei_sort_numpy(arr)

# Disable sorted detection (if you know data is random)
rei_sort(arr, detect_sorted=False)
```

## 🏗️ Building

### Build Everything

```bash
cd reicore
mkdir build && cd build
cmake ..
make
```

### Build Options

```bash
cmake .. \
  -DBUILD_PYTHON_MODULE=ON \   # Build Python bindings (requires pybind11)
  -DBUILD_TESTS=ON \            # Build test suite
  -DBUILD_BENCHMARK=ON \        # Build benchmark suite
  -DBUILD_EXAMPLES=ON           # Build example programs
```

### Run tests, benchmarks, and examples

From a CMake build (inside `reicore/build`):

```bash
./rei_test       # Unit tests
./rei_benchmark  # Performance comparison
./rei_example    # Usage examples
```

**Without CMake** (single-file compile):

```bash
cd reicore
g++ -std=c++20 -O3 -I. -o rei_test test.cpp && ./rei_test
g++ -std=c++20 -O3 -I. -o rei_benchmark benchmark.cpp && ./rei_benchmark
g++ -std=c++20 -O3 -I. -o rei_example example.cpp && ./rei_example
```

Standalone minimal example (from repo root):

```bash
g++ -std=c++20 -O3 -Ireicore -o example_simple example_simple.cpp && ./example_simple
# Output: Before: 5 2 8 1 9 3  →  After: 1 2 3 5 8 9
```

## 📈 Performance

Benchmark results from the included suite (`./rei_benchmark`). Measured with **g++ -std=c++20 -O3** on x86_64 Linux; single run per scenario (warm cache).

### Summary table (representative scenarios)

| Scenario | n | rei_sort | std::sort | std::stable_sort | rei vs std::sort |
|----------|---|----------|-----------|------------------|------------------|
| **Large Sorted** | 1,000,000 | 0.42 ms | 6.36 ms | 6.31 ms | **≈15× faster** |
| **Large Reverse** | 1,000,000 | 0.37 ms | 4.19 ms | 8.50 ms | **≈11× faster** |
| **Large Few Unique** | 1,000,000 (100 unique) | 13.85 ms | 18.63 ms | 28.36 ms | **≈1.35× faster** |
| **Large Random** | 1,000,000 | 43.95 ms | 45.37 ms | 50.10 ms | ~1× (parity) |
| **Medium Sorted** | 100,000 | 0.04 ms | 0.52 ms | 0.47 ms | **≈13× faster** |
| **Medium Reverse** | 100,000 | 0.04 ms | 0.35 ms | 0.71 ms | **≈9× faster** |
| **Medium Few Unique** | 100,000 (10 unique) | 0.73 ms | 1.12 ms | 1.78 ms | **≈1.5× faster** |
| **Medium Random** | 100,000 | 3.38 ms | 3.36 ms | 3.89 ms | ~1× (parity) |

### Full benchmark output (reference)

The suite also measures: small (1k) arrays, nearly sorted data, and `double`. Run locally:

```bash
cd reicore/build && ./rei_benchmark
# or, without CMake: cd reicore && g++ -std=c++20 -O3 -I. -o rei_benchmark benchmark.cpp && ./rei_benchmark
```

**Interpretation:**
- **Sorted / reverse:** Rei Sort uses O(n) detection and reverse → large speedup over `std::sort`.
- **Many duplicates:** 3-way partition reduces work → clear win for rei_sort.
- **Random:** Similar to `std::sort`; small variance between runs is normal.
- **Nearly sorted:** Detection is a single pass; if data is not fully sorted/reverse, introsort runs (no special “nearly sorted” path).

## 🔬 Algorithm Details

### Strategy Selection

```
Input → Size Check → Detection (optional) → Algorithm Selection
                                                     ↓
                                    ┌────────────────┴────────────────┐
                                    │                                 │
                              n ≤ 20 (small)                    n > 20 (large)
                                    ↓                                 ↓
                            Insertion Sort                    Introsort
                          (Binary Search + Move)              (3-way partition)
                                                                 ↓
                                                        Depth limit exceeded?
                                                                 ↓
                                                            Heapsort
```

### 3-Way Partition (Dutch National Flag)

The 3-way partition is crucial for performance on data with duplicates:

```
Before:  [5, 2, 8, 2, 9, 2, 1, 2]  (pivot = 2)
After:   [1, | 2, 2, 2, 2, | 5, 8, 9]
            <2      =2       >2
```

Instead of recursing on equal elements, we skip them entirely, providing **O(n)** performance when all elements are equal.

## 🆚 Comparison with Other Algorithms

| Algorithm | Stable? | Best Case | Average | Worst Case | Space | Notes |
|-----------|---------|-----------|---------|------------|-------|-------|
| **rei_sort** | ❌ | O(n) | O(n log n) | O(n log n) | O(log n) | Fast on duplicates, adaptive |
| std::sort | ❌ | O(n log n) | O(n log n) | O(n log n) | O(log n) | Industry standard |
| std::stable_sort | ✅ | O(n log n) | O(n log n) | O(n log n) | O(n) | Preserves order |
| Timsort | ✅ | O(n) | O(n log n) | O(n log n) | O(n) | Python's default |
| Quicksort | ❌ | O(n log n) | O(n log n) | O(n²) | O(log n) | No worst-case guarantee |
| Heapsort | ❌ | O(n log n) | O(n log n) | O(n log n) | O(1) | Slow in practice |

## 🎯 When to Use Rei Sort

**Best for:**
- General-purpose sorting with unknown data distribution
- Data with many duplicates
- Potentially sorted or nearly-sorted data
- Performance-critical C++ applications
- Python applications needing faster sorting

**Not ideal for:**
- Stable sorting requirements → use `std::stable_sort` or Timsort
- Small constant datasets → any algorithm works
- Non-comparison sorting → use radix/counting sort

## 📚 API Reference

### C++

```cpp
namespace rei {
    // Sort range [first, last)
    template <std::random_access_iterator RandomIt, typename Compare = std::less<>>
    void rei_sort(RandomIt first, RandomIt last,
                  Compare comp = Compare{},
                  bool detect_sorted = true);

    // Sort container
    template <typename Container, typename Compare = std::less<>>
    void rei_sort(Container& container,
                  Compare comp = Compare{},
                  bool detect_sorted = true);

    // Sort by key (Schwartzian transform)
    template <std::random_access_iterator RandomIt, typename KeyFunc, typename Compare = std::less<>>
    void rei_sort_by_key(RandomIt first, RandomIt last,
                         KeyFunc key_func,
                         Compare comp = Compare{});
}
```

### Python

```python
def rei_sort(arr, key=None, detect_sorted=True):
    """
    Sort list in-place

    Parameters:
    -----------
    arr : list
        List to sort (modified in-place)
    key : callable, optional
        Key function for sorting (called once per element)
    detect_sorted : bool, default=True
        Enable O(n) pre-scan for already sorted/reversed arrays

    Returns:
    --------
    list : The same list, sorted in-place
    """

def rei_sort_numpy(arr, detect_sorted=True):
    """
    Sort NumPy array in-place (zero-copy)

    Supported dtypes: int32, int64, float32, float64
    """
```

## 🧪 Testing

The C++ test suite includes **24 unit tests**; all must pass before release.

```bash
cd reicore/build && ./rei_test
# or: cd reicore && g++ -std=c++20 -O3 -I. -o rei_test test.cpp && ./rei_test
```

**Current status:** **24/24 tests passed.**

| Category | Tests |
|----------|--------|
| **Edge cases** | `empty`, `single_element`, `two_elements_sorted`, `two_elements_unsorted` |
| **Order** | `already_sorted`, `reverse_sorted`, `all_equal` |
| **Random** | `random_small`, `random_medium`, `random_large` |
| **Duplicates** | `many_duplicates`, `all_duplicates_but_one` |
| **Types** | `strings`, `strings_with_duplicates` |
| **Comparators** | `custom_comparator_descending`, `custom_comparator_abs` |
| **Key-based** | `sort_by_key`, `sort_pairs_by_second` |
| **Other** | `nearly_sorted`, `large_range`, `negative_numbers`, `detection_disabled_sorted`, `detection_disabled_reverse`, `unstable_sort` |

### Example program output

`./rei_example` runs 8 demos: basic int sort, strings, descending order, sort by |x|, struct by key, pairs by second element, sorted/reverse detection (O(n)), and a large array. Example snippet:

```
Example 1: Basic Integer Sorting
Before: [3, 1, 4, 1, 5, 9, 2, 6, 5, 3]
After: [1, 1, 2, 3, 3, 4, 5, 5, 6, 9]
...
Example 7: Already Sorted Detection (O(n))
Sorted data: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
After rei_sort (detected as sorted): [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
Reverse data: [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
After rei_sort (detected and reversed): [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
```

## � API Documentation

The code is fully documented with **Doxygen** comments for API reference generation.

### Generate Doxygen Documentation

```bash
# Install Doxygen (if not already installed)
sudo pacman -S doxygen graphviz  # Arch/EndeavourOS
sudo apt install doxygen graphviz  # Ubuntu/Debian
brew install doxygen graphviz       # macOS

# Generate HTML documentation (from project root)
doxygen Doxyfile

# Open in browser
xdg-open docs/html/index.html  # Linux
open docs/html/index.html      # macOS
```

All public functions include:
- **@brief** - Short description
- **@tparam** - Template parameter explanations
- **@param** - Parameter descriptions
- **@complexity** - Time and space complexity analysis
- **@note** - Important usage notes
- **@example** - Code examples

## 🔮 Future Roadmap

See [ROADMAP.md](ROADMAP.md) for detailed development plans. Highlights:

### Version 2.1.0 (Q2 2026)
- 🔹 **Stable Sort** - `rei_stable_sort` with order preservation
- 🔹 **Enhanced Documentation** - MkDocs user guide + GitHub Pages

### Version 2.2.0 (Q3 2026)
- ⚡ **SIMD Optimizations** - AVX2/AVX-512 vectorization for primitives
- 🔀 **Parallel Sorting** - `rei_par_sort` with multi-threading

### Version 3.0.0 (Q4 2026)
- 🎯 **String Specialization** - Radix sort for string arrays
- 📊 **Partial Sort** - `rei_partial_sort` for top-K problems
- 🧪 **Adaptive Timsort Mode** - Run detection for real-world data

**Want to contribute?** Check [ROADMAP.md](ROADMAP.md) for details!

## �📝 Requirements

### C++
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.15+ (optional; you can compile with `g++ -std=c++20 -O3 -I.` as shown above)

### Python
- Python 3.8+
- pybind11 (for building the `reicore` extension)
- NumPy (optional, for `rei_sort_numpy`)

## 📜 License

MIT License - see LICENSE file for details

## 👤 Author

**Rei** - Creator of Rei Sort

## 🙏 Acknowledgments

Inspired by:
- **Introsort** (David Musser) - std::sort implementation
- **3-way Quicksort** (Dijkstra) - Dutch National Flag partition
- **Timsort** (Tim Peters) - Adaptive sorting ideas

---

**Made with ❤️ by Rei**
