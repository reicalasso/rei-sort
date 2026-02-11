# Changelog

All notable changes to Rei Sort will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.1] - 2026-02-11

### Added
- **Comprehensive Doxygen documentation** for all public API functions
- **ROADMAP.md** outlining future development plans (stable sort, SIMD, parallel sorting)
- **Doxyfile** configuration for generating HTML API documentation
- **CHANGELOG.md** for tracking version history
- `.vscode/c_cpp_properties.json` for proper C++20 IntelliSense configuration

### Changed
- **Insertion Sort** now uses **binary search** (`std::upper_bound`) for fewer comparisons
- **`rei_sort_by_key`** now uses **in-place permutation** (cycle decomposition) instead of full data copy
  - **Memory optimization**: Reduced from 2N to N space complexity
- All template functions now use **C++20 concepts** (`std::random_access_iterator`) for better type safety
- README updated with:
  - Future Roadmap section
  - API Documentation generation instructions
  - Updated feature list highlighting new optimizations

### Fixed
- **Critical Python GIL bug** in `reicore.cpp` - Python object comparisons now correctly hold GIL
  - Prevents potential segmentation faults when sorting generic Python objects
- Added `<cstddef>` include for proper `std::size_t` resolution

### Documentation
- Doxygen comments include:
  - `@brief` descriptions for all functions
  - `@tparam` explanations for template parameters
  - `@param` descriptions with behavioral notes
  - `@complexity` analysis (time and space)
  - `@example` code snippets
  - `@note` for important usage caveats

## [2.0.0] - 2026-02-10

### Added
- Initial public release of Rei Sort
- **Adaptive hybrid sorting algorithm** combining:
  - O(n) sorted/reverse detection
  - Insertion sort for small arrays
  - Iterative introsort with 3-way partition
  - Heapsort fallback for worst-case guarantee
- **Python bindings** via pybind11
  - `rei_sort()` for Python lists
  - `rei_sort_numpy()` for zero-copy NumPy arrays
  - GIL release for thread-safe parallel execution
- **Key-based sorting** with Schwartzian transform
- **Comprehensive test suite** (24 unit tests)
- **Benchmark suite** comparing against `std::sort` and `std::stable_sort`
- CMake build system with optional components
- Header-only C++ library (zero installation overhead)

### Performance
- **15× faster** than `std::sort` on already sorted data (1M elements)
- **11× faster** on reverse sorted data
- **1.35× faster** on data with many duplicates
- **Parity** with `std::sort` on random data

---

## Release Notes

### Version Numbering
- **2.x.y** - Current series (C++20, modern features)
- **x.0.0** - Major version (breaking changes)
- **x.y.0** - Minor version (new features, backward compatible)
- **x.y.z** - Patch version (bug fixes, optimizations)

### Upgrade Guide

#### From 2.0.0 to 2.0.1
No breaking changes. Drop-in replacement. Benefits:
- Better memory efficiency for key-based sorting
- Fewer comparisons in insertion sort
- Improved documentation

---

**GitHub Repository:** [https://github.com/rei/rei-sort](https://github.com/rei/rei-sort)  
**License:** MIT
