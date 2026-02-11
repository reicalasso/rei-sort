/**
 * REI SORT - Python Bindings (pybind11)
 *
 * High-performance Python wrapper with:
 * - NumPy array support
 * - GIL release for better threading
 * - Type-safe overloads
 * - Custom comparator support
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include "rei_sort.hpp"

namespace py = pybind11;

// ============================================================================
// Python List Sorting (with type inference)
// ============================================================================

template <typename T>
void sort_python_list_impl(py::list& lst, bool detect_sorted) {
    std::vector<T> vec;
    vec.reserve(lst.size());

    // Extract to C++ vector
    for (auto item : lst) {
        vec.push_back(item.cast<T>());
    }

    // Sort with GIL released (allows parallel Python threads)
    {
        py::gil_scoped_release release;
        rei::rei_sort(vec.begin(), vec.end(), std::less<T>{}, detect_sorted);
    }

    // Write back to Python list
    for (size_t i = 0; i < vec.size(); ++i) {
        lst[i] = vec[i];
    }
}

void sort_python_list(py::list& lst, bool detect_sorted) {
    if (lst.empty()) return;

    // Type inference from first element
    auto first = lst[0];

    try {
        if (py::isinstance<py::int_>(first)) {
            sort_python_list_impl<long long>(lst, detect_sorted);
        } else if (py::isinstance<py::float_>(first)) {
            sort_python_list_impl<double>(lst, detect_sorted);
        } else if (py::isinstance<py::str>(first)) {
            sort_python_list_impl<std::string>(lst, detect_sorted);
        } else {
            // Fallback: use Python comparison (slower)
            py::gil_scoped_release release;
            std::sort(lst.begin(), lst.end(), [](py::handle a, py::handle b) {
                return a.attr("__lt__")(b).cast<bool>();
            });
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("rei_sort: Failed to sort list - " + std::string(e.what()));
    }
}

// ============================================================================
// Python List with Key Function (Schwartzian Transform)
// ============================================================================

template <typename T>
void sort_python_list_key_impl(py::list& lst, py::function key_func, bool detect_sorted) {
    struct Item {
        py::object key_val;
        T original;
        size_t index;  // For stable sort fallback
    };

    std::vector<Item> decorated;
    decorated.reserve(lst.size());

    // Decorate: compute keys
    for (size_t i = 0; i < lst.size(); ++i) {
        auto item = lst[i];
        py::object key_val = key_func(item);
        decorated.push_back({key_val, item.cast<T>(), i});
    }

    // Sort by key (GIL released)
    {
        py::gil_scoped_release release;
        rei::rei_sort(decorated.begin(), decorated.end(),
                      [](const Item& a, const Item& b) {
                          py::gil_scoped_acquire acquire;
                          return a.key_val.attr("__lt__")(b.key_val).cast<bool>();
                      },
                      detect_sorted);
    }

    // Undecorate: write back
    for (size_t i = 0; i < decorated.size(); ++i) {
        lst[i] = decorated[i].original;
    }
}

void sort_python_list_key(py::list& lst, py::function key_func, bool detect_sorted) {
    if (lst.empty()) return;

    auto first = lst[0];

    try {
        if (py::isinstance<py::int_>(first)) {
            sort_python_list_key_impl<long long>(lst, key_func, detect_sorted);
        } else if (py::isinstance<py::float_>(first)) {
            sort_python_list_key_impl<double>(lst, key_func, detect_sorted);
        } else if (py::isinstance<py::str>(first)) {
            sort_python_list_key_impl<std::string>(lst, key_func, detect_sorted);
        } else {
            sort_python_list_key_impl<py::object>(lst, key_func, detect_sorted);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("rei_sort with key: Failed - " + std::string(e.what()));
    }
}

// ============================================================================
// NumPy Array Sorting (zero-copy when possible)
// ============================================================================

template <typename T>
void sort_numpy_array_impl(py::array_t<T>& arr, bool detect_sorted) {
    auto buf = arr.request();
    T* ptr = static_cast<T*>(buf.ptr);
    size_t size = buf.size;

    // Sort with GIL released
    {
        py::gil_scoped_release release;
        rei::rei_sort(ptr, ptr + size, std::less<T>{}, detect_sorted);
    }
}

void sort_numpy_array(py::array& arr, bool detect_sorted) {
    if (arr.size() == 0) return;

    // Ensure array is writable and C-contiguous
    if (!(arr.flags() & py::array::c_style)) {
        throw std::runtime_error("rei_sort: NumPy array must be C-contiguous");
    }

    if (!arr.writeable()) {
        throw std::runtime_error("rei_sort: NumPy array must be writable");
    }

    auto dtype = arr.dtype();

    if (dtype.is(py::dtype::of<int32_t>())) {
        sort_numpy_array_impl(arr.cast<py::array_t<int32_t>&>(), detect_sorted);
    } else if (dtype.is(py::dtype::of<int64_t>())) {
        sort_numpy_array_impl(arr.cast<py::array_t<int64_t>&>(), detect_sorted);
    } else if (dtype.is(py::dtype::of<float>())) {
        sort_numpy_array_impl(arr.cast<py::array_t<float>&>(), detect_sorted);
    } else if (dtype.is(py::dtype::of<double>())) {
        sort_numpy_array_impl(arr.cast<py::array_t<double>&>(), detect_sorted);
    } else {
        throw std::runtime_error("rei_sort: Unsupported NumPy dtype");
    }
}

// ============================================================================
// Pybind11 Module Definition
// ============================================================================

PYBIND11_MODULE(reicore, m) {
    m.doc() = "Rei Sort - High-performance C++ sorting engine";

    // Main sorting function (Python list)
    m.def("rei_sort",
          [](py::list lst, py::object key, bool detect_sorted) {
              if (key.is_none()) {
                  sort_python_list(lst, detect_sorted);
              } else {
                  sort_python_list_key(lst, key.cast<py::function>(), detect_sorted);
              }
              return lst;
          },
          py::arg("arr"),
          py::arg("key") = py::none(),
          py::arg("detect_sorted") = true,
          R"(
          Rei Sort - Hybrid comparison-based sorting algorithm

          Parameters:
          -----------
          arr : list
              List to sort (in-place)
          key : callable, optional
              Key function for sorting (called once per element)
          detect_sorted : bool, default=True
              Enable O(n) pre-scan for already sorted/reversed arrays

          Returns:
          --------
          list
              The same list, sorted in-place

          Notes:
          ------
          - NOT stable (equal elements may be reordered)
          - O(n log n) average and worst case
          - O(n) best case for sorted/reversed arrays (if detect_sorted=True)
          - In-place sorting with O(log n) extra space

          Examples:
          ---------
          >>> arr = [3, 1, 4, 1, 5]
          >>> rei_sort(arr)
          [1, 1, 3, 4, 5]

          >>> pairs = [("b", 2), ("a", 3), ("c", 1)]
          >>> rei_sort(pairs, key=lambda x: x[1])
          [('c', 1), ('b', 2), ('a', 3)]
          )");

    // NumPy array sorting
    m.def("rei_sort_numpy",
          &sort_numpy_array,
          py::arg("arr"),
          py::arg("detect_sorted") = true,
          R"(
          Rei Sort for NumPy arrays (zero-copy, in-place)

          Parameters:
          -----------
          arr : numpy.ndarray
              1D NumPy array to sort (must be C-contiguous and writable)
          detect_sorted : bool, default=True
              Enable O(n) pre-scan for already sorted/reversed arrays

          Supported dtypes: int32, int64, float32, float64
          )");

    // Configuration constants
    m.attr("INSERTION_THRESHOLD") = rei::INSERTION_THRESHOLD;
    m.attr("INTROSORT_DEPTH_FACTOR") = rei::INTROSORT_DEPTH_FACTOR;

    // Version info
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "Rei";
}
