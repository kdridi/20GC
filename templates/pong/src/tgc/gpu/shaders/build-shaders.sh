#!/usr/bin/env bash

set -euo pipefail
cd "$(dirname "$0")"

command -v shadercross >/dev/null 2>&1 || {
    echo "shadercross (SDL_shadercross) is required" >&2
    exit 1
}
command -v xxd >/dev/null 2>&1 || {
    echo "xxd is required" >&2
    exit 1
}

mkdir -p generated

make_header() {
    local binary="$1"
    xxd -i "$binary" \
        | sed -e 's/^unsigned /const unsigned /g' -e 's,^const,static const,' \
        > "generated/${binary}.h"
}

for shader in solid.vert.hlsl solid.frag.hlsl; do
    for format in spv msl dxil; do
        output="${shader/.hlsl/.${format}}"
        shadercross "$shader" -o "$output"
        make_header "$output"
        rm -f "$output"
    done
done
