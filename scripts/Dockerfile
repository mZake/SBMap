# syntax=docker/dockerfile:1
FROM debian:11

ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update -y                                                                   && \
    apt-get install -y clang cmake file git libasound2-dev libpulse-dev libaudio-dev    \
        libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev libxrandr-dev    \
        libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev libxkbcommon-dev  \
        libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev        \
        libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev pkg-config wget           && \
    rm -rf /var/lib/apt/lists/*

# Clone repository and build AppImage
WORKDIR /repos/
RUN git clone https://github.com/mZake/SBMap.git    && \
    cd SBMap                                        && \
    export APPIMAGE_EXTRACT_AND_RUN=1               && \
    export CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)      && \
    scripts/build-appimage.sh

FROM scratch
COPY --from=0 /repos/SBMap/build /output
