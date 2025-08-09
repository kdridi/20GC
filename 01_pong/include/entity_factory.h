// Entity Factory - helpers pour créer des entités complètes
// Fonctions factory pour créer des entités avec les bons composants

#pragma once

#include "entity.h"

// Structure pour retourner toutes les entités d'un jeu Pong
typedef struct {
    Entity *player1;
    Entity *player2;
    Entity *ball;
    Entity *game_state;
} PongEntities;

// Crée un joueur complet (Position + Paddle + Score)
Entity *create_player(float x, float y);

// Crée une balle complète (Position + Velocity + Ball)
Entity *create_ball(float x, float y, float dx, float dy);

// Crée une entité game state (GameState)
Entity *create_game_state(void);

// Crée un jeu Pong complet avec tous les éléments
PongEntities create_pong_game(void);

// Détruit toutes les entités d'un jeu Pong
void destroy_pong_game(PongEntities *entities);

// Met à jour un jeu Pong (appelle tous les systèmes)
void update_pong_game(float delta_time);
