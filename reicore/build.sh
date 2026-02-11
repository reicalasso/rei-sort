#!/bin/bash
# Rei Sort - Quick Build Script

set -e  # Exit on error

echo "╔════════════════════════════════════════════════════════════╗"
echo "║          REI SORT - BUILD SCRIPT                          ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Clean previous build
if [ -d "build" ]; then
    echo "🧹 Cleaning previous build..."
    rm -rf build
fi

# Create build directory
echo "📁 Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "⚙️  Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_PYTHON_MODULE=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_BENCHMARK=ON \
    -DBUILD_EXAMPLES=ON

# Build
echo "🔨 Building..."
make -j$(nproc)

echo ""
echo "✅ Build complete!"
echo ""
echo "Available executables:"
echo "  ./rei_test       - Run unit tests"
echo "  ./rei_benchmark  - Run performance benchmarks"
echo "  ./rei_example    - Run examples"
echo ""
echo "Python module: build/reicore*.so"
echo ""
