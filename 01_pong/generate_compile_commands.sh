#!/bin/bash

# Génère compile_commands.json pour VS Code
# Basé sur notre Makefile

cat > compile_commands.json << 'EOF'
[
  {
    "directory": "/Users/kdridi/git/github.com/kdridi/20GC.git/01_pong",
    "command": "gcc -std=c23 -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wdouble-promotion -Wfloat-equal -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wmissing-prototypes -Wnull-dereference -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings -g3 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -Iinclude -include include/pch.h -c src/entity_pool.c",
    "file": "src/entity_pool.c"
  }
]
EOF

# Ajouter automatiquement tous les fichiers de test
for test_file in tests/test_*.c; do
  if [ -f "$test_file" ]; then
    filename=$(basename "$test_file")
    cat >> compile_commands.json << EOF
  ,{
    "directory": "/Users/kdridi/git/github.com/kdridi/20GC.git/01_pong",
    "command": "gcc -std=c23 -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wdouble-promotion -Wfloat-equal -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wmissing-prototypes -Wnull-dereference -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings -g3 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -I/opt/homebrew/Cellar/googletest/1.17.0/include -DGTEST_HAS_PTHREAD=1 -I/opt/homebrew/Cellar/googletest/1.17.0/include -DGTEST_HAS_PTHREAD=1 -Iinclude -include include/pch.h -c $test_file",
    "file": "$test_file"
  }
EOF
  fi
done

echo "]" >> compile_commands.json

echo "compile_commands.json généré avec succès"