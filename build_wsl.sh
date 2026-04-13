#!/bin/bash
# Build CalculatorDLL as a Linux shared library (.so) using CMake.
# Run this script inside WSL from the project root directory.
#
# Prerequisites (install once):
#   sudo apt update && sudo apt install -y build-essential cmake
#
# Usage:
#   chmod +x build_wsl.sh
#   ./build_wsl.sh

set -e

BUILD_DIR="build_linux"

echo "=== Building CalculatorDLL (.so) for Linux ==="

# Configure
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build "$BUILD_DIR" --config Debug -j "$(nproc)"

echo ""
echo "=== Build complete ==="
echo "Shared library: $BUILD_DIR/lib/libCalculatorDLL.so"
echo "Test executable: $BUILD_DIR/bin/CPPTest"
echo ""
echo "To run the C++ test:"
echo "  cd $BUILD_DIR/bin"
echo "  LD_LIBRARY_PATH=../lib ./CPPTest"
echo ""
echo "To use with C# (dotnet on Linux):"
echo "  cp $BUILD_DIR/lib/libCalculatorDLL.so <your-dotnet-output-dir>/"
echo "  cd <your-dotnet-output-dir>"
echo "  dotnet run"
