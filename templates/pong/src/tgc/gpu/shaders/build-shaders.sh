#!/usr/bin/env bash

set -euo pipefail
cd "$(dirname "$0")"

shadercross="${SHADERCROSS:-shadercross}"
xxd="${XXD:-xxd}"

command -v "$shadercross" >/dev/null 2>&1 || {
    echo "shadercross (SDL_shadercross) is required; see README.md" >&2
    exit 1
}
command -v "$xxd" >/dev/null 2>&1 || {
    echo "xxd is required; see README.md" >&2
    exit 1
}

temporary_directory="$(mktemp -d "${TMPDIR:-/tmp}/tgc-pong-shaders.XXXXXX")"
intermediate_files=()

cleanup() {
    rm -rf "$temporary_directory"
    if ((${#intermediate_files[@]} > 0)); then
        rm -f "${intermediate_files[@]}"
    fi
}
trap cleanup EXIT

make_header() {
    local binary="$1"
    "$xxd" -i "$binary" \
        | sed -e 's/^unsigned /const unsigned /g' -e 's,^const,static const,' \
        > "$temporary_directory/${binary}.h"
}

for shader in solid.vert.hlsl solid.frag.hlsl; do
    for format in spv msl dxil; do
        output="${shader/.hlsl/.${format}}"
        intermediate_files+=("$output")
        "$shadercross" "$shader" -o "$output"
        make_header "$output"
        rm -f "$output"
    done
done

mkdir -p generated
mv "$temporary_directory"/*.h generated/
