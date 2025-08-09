# Variables
CC = gcc
CSTD = -std=c23
WARNINGS = -Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion -Wcast-align \
           -Wcast-qual -Wdouble-promotion -Wfloat-equal -Wformat=2 -Winit-self \
           -Wmissing-declarations -Wmissing-include-dirs -Wmissing-prototypes -Wnull-dereference \
           -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wswitch-default \
           -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings
SANITIZERS = -fsanitize=address,undefined -fno-omit-frame-pointer
DEBUG_FLAGS = -g3 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG -flto
COVERAGE_FLAGS = --coverage
CFLAGS_BASE = $(CSTD) $(WARNINGS)

# Configuration par mode
ifndef RELEASE
    ifndef COVERAGE
        CFLAGS = $(CFLAGS_BASE) $(DEBUG_FLAGS) $(SANITIZERS)
    else
        CFLAGS = $(CFLAGS_BASE) $(DEBUG_FLAGS) $(COVERAGE_FLAGS)
    endif
else
    CFLAGS = $(CFLAGS_BASE) $(RELEASE_FLAGS)
endif

# Precompiled Header
PCH_FILE = include/pch.h
PCH_OUT = include/pch.h.gch

# Répertoires
SRCDIR = src
TESTDIR = tests
INCDIR = include
BUILDDIR = build
COVERAGE_DIR = coverage_report

# Bibliothèques via pkg-config
SDL2_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL2_LIBS = $(shell pkg-config --libs sdl2)
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

# Flags complètes
PROD_CFLAGS = $(CFLAGS) $(SDL2_CFLAGS) -I$(INCDIR)
TEST_CFLAGS = $(CFLAGS) $(SDL2_CFLAGS) -I$(INCDIR)
LIBS = $(SDL2_LIBS)

# Fichiers source
PROD_SOURCES = $(wildcard $(SRCDIR)/*.c)
TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
PROD_OBJECTS = $(PROD_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/prod/%.o)
# Objets pour les tests : exclure tous les main*.o qui contiennent main()
TEST_LIB_OBJECTS = $(filter-out $(BUILDDIR)/prod/main.o $(BUILDDIR)/prod/main_ncurses.o $(BUILDDIR)/prod/main_raylib.o, $(PROD_OBJECTS))
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(BUILDDIR)/test/%.o)
TEST_TARGETS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(BUILDDIR)/%)

# Cibles principales  
TARGET_SDL2 = pong_sdl2
TARGET_NCURSES = pong_ncurses
TARGET_RAYLIB = pong_raylib

# Backends disponibles
BACKENDS = sdl2 ncurses raylib

.PHONY: all clean test format dirs pch debug release coverage deps help $(BACKENDS)

# Build tous les backends par défaut
all: $(BACKENDS)

# Backends
sdl2: dirs pch $(TARGET_SDL2)
ncurses: dirs pch $(TARGET_NCURSES)
raylib: dirs pch $(TARGET_RAYLIB)

debug: all

release:
	$(MAKE) RELEASE=1 all

test: dirs pch $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
		echo "Exécution de $$test..."; \
		./$$test || exit 1; \
	done

coverage: clean
	$(MAKE) COVERAGE=1 test
	@echo "Génération du rapport de coverage..."
	lcov --capture --directory . --output-file coverage.info --exclude '*/tests/*' --ignore-errors unused
	genhtml coverage.info --output-directory $(COVERAGE_DIR)
	@echo "Rapport de coverage généré dans $(COVERAGE_DIR)/index.html"
	@echo "Ouvrir avec: open $(COVERAGE_DIR)/index.html"

# Vérification des dépendances
deps:
	@./check_deps.sh

# Création des répertoires
dirs:
	@mkdir -p $(BUILDDIR)/prod $(BUILDDIR)/test

# Compilation du PCH
pch: dirs $(PCH_OUT)

$(PCH_OUT): $(PCH_FILE)
	$(CC) $(PROD_CFLAGS) -c $< -o $@

# Exécutable SDL2
$(TARGET_SDL2): $(filter-out $(BUILDDIR)/prod/main_ncurses.o $(BUILDDIR)/prod/main_raylib.o, $(PROD_OBJECTS))
	$(CC) $^ $(SDL2_LIBS) $(if $(findstring fsanitize,$(CFLAGS)),$(SANITIZERS)) $(if $(findstring coverage,$(CFLAGS)),$(COVERAGE_FLAGS)) -o $@

# Exécutable ncurses  
$(TARGET_NCURSES): $(filter-out $(BUILDDIR)/prod/main.o $(BUILDDIR)/prod/main_raylib.o, $(PROD_OBJECTS))
	$(CC) $^ -lncurses $(if $(findstring fsanitize,$(CFLAGS)),$(SANITIZERS)) $(if $(findstring coverage,$(CFLAGS)),$(COVERAGE_FLAGS)) -o $@

# Exécutable Raylib
$(TARGET_RAYLIB): $(filter-out $(BUILDDIR)/prod/main.o $(BUILDDIR)/prod/main_ncurses.o, $(PROD_OBJECTS))
	$(CC) $^ $(RAYLIB_LIBS) $(if $(findstring fsanitize,$(CFLAGS)),$(SANITIZERS)) $(if $(findstring coverage,$(CFLAGS)),$(COVERAGE_FLAGS)) -o $@

# Lien de compatibilité (optionnel)
pong: $(TARGET_SDL2)
	@ln -sf $(TARGET_SDL2) pong

# Règle pour compiler chaque test individuellement
$(BUILDDIR)/test_%: $(BUILDDIR)/test/test_%.o $(TEST_LIB_OBJECTS)
	$(CC) $^ $(LIBS) $(if $(findstring fsanitize,$(CFLAGS)),$(SANITIZERS)) $(if $(findstring coverage,$(CFLAGS)),$(COVERAGE_FLAGS)) -lstdc++ -o $@

# Compilation des objets de production
$(BUILDDIR)/prod/%.o: $(SRCDIR)/%.c $(PCH_OUT)
	$(CC) $(PROD_CFLAGS) -include $(PCH_FILE) -c $< -o $@

# Compilation des objets de test
$(BUILDDIR)/test/%.o: $(TESTDIR)/%.c $(PCH_OUT)
	$(CC) $(TEST_CFLAGS) -include $(PCH_FILE) -c $< -o $@

# Formatage du code
format:
	@find $(SRCDIR) $(TESTDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

# Nettoyage
clean:
	rm -rf $(BUILDDIR) pong $(TARGET_SDL2) $(TARGET_NCURSES) $(TARGET_RAYLIB) $(PCH_OUT) $(COVERAGE_DIR) coverage.info *.gcda *.gcno

# Aide
help:
	@echo "🎯 Cibles de compilation :"
	@echo "  all/debug - Compile Pong SDL2 en mode debug (avec sanitizers)"
	@echo "  release   - Compile en mode release optimisé"
	@echo "  test      - Compile et exécute les tests TDD"
	@echo "  coverage  - Exécute les tests et génère le rapport de coverage"
	@echo ""
	@echo "🎮 Backends graphiques disponibles :"
	@echo "  sdl2      - Version SDL2 (graphique, fenêtrée)"
	@echo "  ncurses   - Version ncurses (terminal, ASCII)"
	@echo "  raylib    - Version Raylib (moderne, hardware accelerated)"
	@echo ""
	@echo "🔧 Utilitaires :"
	@echo "  format    - Formate le code avec clang-format"
	@echo "  deps      - Vérifie les dépendances système"
	@echo "  clean     - Nettoie les fichiers de build"
	@echo "  help      - Affiche cette aide"
	@echo ""
	@echo "📦 Exécutables générés :"
	@echo "  ./pong_sdl2    - Version SDL2"
	@echo "  ./pong_ncurses - Version ncurses"
	@echo "  ./pong_raylib  - Version Raylib"
	@echo "  ./pong         - Lien symbolique vers pong_sdl2"
	@echo ""
	@echo "⚙️ Variables de build :"
	@echo "  RELEASE=1  - Active le mode release (-O3 -flto)"
	@echo "  COVERAGE=1 - Active le mode coverage (gcov/lcov)"