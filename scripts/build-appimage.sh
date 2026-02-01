#!/bin/sh

set -e

ROOT="$(cd "$(dirname "$0")" && pwd)/.."
BUILD="$ROOT/build"
APPDIR="$BUILD/AppDir"
LINUXDEPLOY="$BUILD/linuxdeploy-x86_64.AppImage"

mkdir -p "$BUILD"

if ! [ -f "$LINUXDEPLOY" ]; then
    wget -O "$LINUXDEPLOY" https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20251107-1/linuxdeploy-x86_64.AppImage
fi

chmod +x "$LINUXDEPLOY"

# Configure, build and install
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD" --config Release
cmake --install "$BUILD" --config Release --prefix "$APPDIR"

# Setup AppDir
cp "$ROOT/assets/AppRun" "$APPDIR"
chmod +x "$APPDIR/AppRun"

cp "$ROOT/assets/sbmap.desktop" "$APPDIR"
cp "$ROOT/assets/sbmap.png" "$APPDIR"

mkdir -p "$APPDIR/usr/bin"
cp "$APPDIR/bin/SBMap" "$APPDIR/usr/bin"

# Create AppImage
cd "$BUILD"
"$LINUXDEPLOY" --appdir "$APPDIR" --output appimage
