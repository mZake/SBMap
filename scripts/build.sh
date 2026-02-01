#!/bin/sh

set -e

# Get absolute path to root and build directory
ROOT="$(cd "$(dirname "$0")" && pwd)/.."
BUILD="$ROOT/build"

mkdir -p "$BUILD"

# Configure and build
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD" --config Release
