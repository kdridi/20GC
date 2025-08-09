#pragma once

// Precompiled Header - Bibliothèques standard C et SDL2
// Les libs externes (SDL2) incluses pour l'interface graphique

// Bibliothèques standard C
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SDL2 pour l'interface graphique
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop

// Constantes du projet
#include "constants.h"
