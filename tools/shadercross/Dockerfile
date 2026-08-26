# syntax=docker/dockerfile:1.7

FROM ubuntu:24.04 AS build

ARG DEBIAN_FRONTEND=noninteractive
ARG VCPKG_COMMIT=0ac8df3b98e3afcd8bf075fa74a6bd2c32613345

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        autoconf \
        autoconf-archive \
        automake \
        bison \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        flex \
        git \
        libltdl-dev \
        libtool \
        ninja-build \
        pkg-config \
        python3 \
        tar \
        unzip \
        zip \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg \
    && git -C /opt/vcpkg checkout "${VCPKG_COMMIT}" \
    && /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

RUN mkdir -p /opt/shadercross-manifest /opt/vcpkg-triplets \
    && printf '%s\n' \
        'set(VCPKG_TARGET_ARCHITECTURE x64)' \
        'set(VCPKG_CRT_LINKAGE dynamic)' \
        'set(VCPKG_LIBRARY_LINKAGE static)' \
        'set(VCPKG_CMAKE_SYSTEM_NAME Linux)' \
        'set(VCPKG_BUILD_TYPE release)' \
        > /opt/vcpkg-triplets/x64-linux-release.cmake \
    && printf '%s\n' \
        '{' \
        '  "builtin-baseline": "'"${VCPKG_COMMIT}"'",' \
        '  "dependencies": [' \
        '    {' \
        '      "name": "sdl3",' \
        '      "default-features": false,' \
        '      "features": ["vulkan"]' \
        '    },' \
        '    "sdl3-shadercross"' \
        '  ]' \
        '}' \
        > /opt/shadercross-manifest/vcpkg.json

RUN --mount=type=cache,target=/root/.cache/vcpkg/archives \
    /opt/vcpkg/vcpkg install \
        --x-manifest-root=/opt/shadercross-manifest \
        --overlay-triplets=/opt/vcpkg-triplets \
        --triplet=x64-linux-release \
        --clean-after-build

FROM ubuntu:24.04 AS runtime

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        ca-certificates \
        libstdc++6 \
        xxd \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/shadercross-manifest/vcpkg_installed/x64-linux-release /opt/shadercross

ENV PATH="/opt/shadercross/tools/sdl3-shadercross:${PATH}"
ENV LD_LIBRARY_PATH="/opt/shadercross/lib:/opt/shadercross/bin"

WORKDIR /shaders

RUN shadercross --help >/dev/null
