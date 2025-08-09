#!/bin/bash

# Script de vérification des dépendances pour macOS
# Utilise Homebrew pour installer les outils manquants

echo "🔍 Vérification des dépendances pour 20GC/Pong..."
echo

missing_deps=()

# Fonction de vérification
check_tool() {
    if command -v "$1" &> /dev/null; then
        echo "✅ $1 trouvé: $(which $1)"
    else
        echo "❌ $1 manquant"
        missing_deps+=("$2")
    fi
}

# Vérification des outils essentiels
check_tool "gcc" "gcc"
check_tool "pkg-config" "pkg-config"
check_tool "clang-format" "clang-format"
check_tool "lcov" "lcov"

# Vérification des bibliothèques via pkg-config
echo
echo "📦 Vérification des bibliothèques..."

check_pkg() {
    if pkg-config --exists "$1" 2>/dev/null; then
        echo "✅ $1: $(pkg-config --modversion $1)"
    else
        echo "❌ $1 manquant"
        missing_deps+=("$2")
    fi
}

check_pkg "sdl2" "sdl2"
check_pkg "gtest" "googletest"
check_pkg "gmock" "googletest"

# Résumé et suggestions d'installation
echo
if [ ${#missing_deps[@]} -eq 0 ]; then
    echo "🎉 Toutes les dépendances sont satisfaites !"
    echo "Vous pouvez maintenant exécuter 'make' dans le projet."
else
    echo "⚠️  Dépendances manquantes détectées."
    echo "Installation recommandée avec Homebrew :"
    echo
    for dep in "${missing_deps[@]}"; do
        echo "  brew install $dep"
    done
    echo
    echo "Puis relancez ce script pour vérifier."
fi

echo