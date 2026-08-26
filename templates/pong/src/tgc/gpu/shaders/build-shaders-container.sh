#!/usr/bin/env bash

set -euo pipefail

script_directory="$(cd "$(dirname "$0")" && pwd)"
image="${SHADERCROSS_IMAGE:-ghcr.io/kdridi/20gc-shadercross:latest}"

command -v docker >/dev/null 2>&1 || {
    echo "Docker is required; see README.md" >&2
    exit 1
}

docker pull "$image"
docker run --rm \
    --platform linux/amd64 \
    --user "$(id -u):$(id -g)" \
    --volume "$script_directory:/shaders" \
    "$image" \
    ./build-shaders.sh
