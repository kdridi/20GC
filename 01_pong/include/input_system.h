// Système Input - gestion des entrées pour contrôler les paddles
// Actions métier pour les deux joueurs de Pong

#pragma once

// Actions de jeu pour les paddles
typedef enum {
    PLAYER1_PADDLE_UP,
    PLAYER1_PADDLE_DOWN,
    PLAYER2_PADDLE_UP,
    PLAYER2_PADDLE_DOWN
} GameAction;

// Définit l'état d'une action (active/inactive)
void input_system_set_action(GameAction action, bool active);

// Met à jour le système d'entrées (déplace les paddles selon les actions actives)
void input_system_update();
