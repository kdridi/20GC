// Composant GameState - gestion de l'état du jeu
// Structure pour tracker l'état global du jeu (en cours, pause, etc.)

#pragma once

#include "entity.h"

// États possibles du jeu
typedef enum {
    GAME_PLAYING, // Jeu en cours
    GAME_PAUSED,  // Jeu en pause
    GAME_ENDED    // Jeu terminé
} GameStateValue;

// Structure du composant GameState
typedef struct {
    GameStateValue state;
} GameState;

// Ajoute un composant GameState à une entité
GameState *game_state_add(Entity *entity, GameStateValue initial_state);

// Récupère le composant GameState d'une entité (ou NULL si absent)
GameState *game_state_get(Entity *entity);
