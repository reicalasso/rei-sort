# Rei Sort - Development Roadmap

This document outlines planned features and improvements for future releases.

## Version 2.1.0 (Q2 2026)

### Stable Sort Implementation
- [ ] **`rei_stable_sort`** - Merge Sort based stable sorting
  - Preserves relative order of equal elements
  - Useful for multi-level sorting (sort by age, then by name)
  - Target complexity: O(n log n) time, O(n) space
  - API: Same as `rei_sort` but guarantees stability
  ```cpp
  rei::rei_stable_sort(container.begin(), container.end());
  rei::rei_stable_sort_by_key(container, [](auto& x) { return x.key; });
  ```

### Enhanced Documentation
- [ ] **Doxygen HTML Documentation** ✅ (Doxygen comments added in v2.0.1)
  - Generate API reference website
  - Host on GitHub Pages
  - Include visual algorithm diagrams
- [ ] **MkDocs User Guide**
  - Tutorial-style documentation
  - Performance tuning guide
  - Migration guide from std::sort

## Version 2.2.0 (Q3 2026)

### SIMD Optimizations
- [ ] **Vectorized Comparisons** for primitive types
  - Use `std::simd` (C++23) or compiler intrinsics (AVX2/AVX-512)
  - Target: 2-4× speedup on large primitive arrays
  - Platforms: x86_64 (AVX2), ARM (NEON)
  - Optional compile-time flag: `-DREI_SORT_USE_SIMD=ON`
  
- [ ] **Vectorized Partition** for 3-way partition
  - SIMD-accelerated element classification (<, =, >)
  - Particularly effective for arrays with many duplicates

### Parallel Sorting
- [ ] **`rei_par_sort`** - Multi-threaded sorting
  - Use C++17 parallel algorithms or custom thread pool
  - Automatic thread count detection
  - Target: 4-8× speedup on 8+ core systems
  ```cpp
  rei::rei_par_sort(large_array.begin(), large_array.end());
  ```

## Version 3.0.0 (Q4 2026)

### Extended Type Support
- [ ] **String Optimization**
  - Radix sort for fixed-length strings
  - Multi-key Quicksort for variable-length strings
  - UTF-8 aware sorting

- [ ] **Custom Allocator Support**
  - Allow users to provide custom allocators for internal buffers
  - PMR (Polymorphic Memory Resource) compatibility

### Advanced Features
- [ ] **Partial Sort**
  - `rei_partial_sort(first, middle, last)` - only sort first N elements
  - More efficient than full sort when only top-K needed
  
- [ ] **Adaptive Timsort Mode**
  - Detect runs (sorted subsequences) like Python's Timsort
  - Better performance on real-world "nearly sorted" data

### Benchmarking & Profiling
- [ ] **Comprehensive Benchmark Suite**
  - Compare against: std::sort, pdqsort, BlockQuicksort, Ska_sort
  - Test on diverse datasets: random, sorted, reverse, organ pipe, etc.
  - Automated CI benchmark regression tests

- [ ] **Performance Profiling Tools**
  - Branch misprediction analysis
  - Cache miss analysis (Valgrind/perf integration)
  - Comparison count tracking

## Research & Exploration

### Future Possibilities (TBD)
- **GPU Sorting** - CUDA/OpenCL for massive arrays (10M+ elements)
- **Network Sorting** - Batcher's odd-even mergesort for fixed-size arrays
- **Adaptive Dual-Pivot Quicksort** - Alternative to 3-way partition
- **Branch-Free Implementations** - Reduce branch mispredictions
- **Floating-Point Radix Sort** - For `float`/`double` arrays

---

## Contributing

Interested in implementing one of these features? Check out [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines!

## Versioning

We follow [Semantic Versioning](https://semver.org/):
- **MAJOR**: Breaking API changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes and minor improvements

---

**Last Updated:** February 11, 2026  
**Current Version:** 2.0.1
