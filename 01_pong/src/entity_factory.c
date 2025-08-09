// Entity Factory - helpers pour créer des entités complètes
// Fonctions factory pour créer des entités avec les bons composants

#include "entity_factory.h"
#include "ball.h"
#include "bounds_system.h"
#include "collision_system.h"
#include "entity.h"
#include "game_state.h"
#include "input_system.h"
#include "movement_system.h"
#include "paddle.h"
#include "position.h"
#include "reset_system.h"
#include "score.h"
#include "scoring_system.h"
#include "velocity.h"

Entity *create_player(const float x, const float y)
{
    Entity *const entity = entity_create();
    assert(entity != NULL && "Échec création entité player");

    // Ajouter les composants du joueur
    position_add(entity, x, y);
    paddle_add(entity, 10.0f, 60.0f); // Taille standard paddle
    score_add(entity, 0);             // Score initial

    return entity;
}

Entity *create_ball(const float x, const float y, const float dx, const float dy)
{
    Entity *const entity = entity_create();
    assert(entity != NULL && "Échec création entité ball");

    // Ajouter les composants de la balle
    position_add(entity, x, y);
    velocity_add(entity, dx, dy);
    ball_add(entity, 4.0f); // Rayon standard

    return entity;
}

Entity *create_game_state(void)
{
    Entity *const entity = entity_create();
    assert(entity != NULL && "Échec création entité game state");

    // Ajouter le composant GameState
    game_state_add(entity, GAME_PLAYING);

    return entity;
}

PongEntities create_pong_game(void)
{
    // Créer tous les éléments du jeu
    Entity *const player1 = create_player(50.0f, SCREEN_HEIGHT / 2.0f);
    Entity *const player2 = create_player(SCREEN_WIDTH - 60.0f, SCREEN_HEIGHT / 2.0f);
    Entity *const ball = create_ball(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 150.0f, 100.0f);
    Entity *const game_state = create_game_state();

    return (PongEntities) {
        .player1 = player1,
        .player2 = player2,
        .ball = ball,
        .game_state = game_state};
}

void destroy_pong_game(PongEntities *const entities)
{
    assert(entities != NULL && "PongEntities ne peut pas être NULL");

    // Détruire toutes les entités
    if (entities->player1 != NULL) {
        entity_destroy(entities->player1);
        entities->player1 = NULL;
    }
    if (entities->player2 != NULL) {
        entity_destroy(entities->player2);
        entities->player2 = NULL;
    }
    if (entities->ball != NULL) {
        entity_destroy(entities->ball);
        entities->ball = NULL;
    }
    if (entities->game_state != NULL) {
        entity_destroy(entities->game_state);
        entities->game_state = NULL;
    }
}

void update_pong_game(const float delta_time)
{
    // Mettre à jour tous les systèmes dans l'ordre logique
    input_system_update();              // 1. Traiter les entrées
    movement_system_update(delta_time); // 2. Déplacer les objets
    bounds_system_update();             // 3. Contraindre dans les limites
    collision_system_update();          // 4. Gérer les collisions
    scoring_system_update();            // 5. Détecter les points
    reset_system_update();              // 6. Remettre la balle si nécessaire
}
