// Test TDD pour entity_factory
// Fonctions helper pour créer des entités complètes

#include "ball.h"
#include "entity.h"
#include "entity_factory.h"
#include "game_config.h"
#include "game_state.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"
#include "velocity.h"

static void test_create_player()
{
    // Créer un player complet
    Entity *const player = create_player(100.0f, 250.0f);
    assert(player != NULL);

    const GameConfig *const config = game_config_get_current();

    // Vérifier que tous les composants sont présents
    Position *const pos = position_get(player);
    Paddle *const paddle = paddle_get(player);
    Score *const score = score_get(player);

    assert(pos != NULL && FLOAT_EQ(pos->x, 100.0f) && FLOAT_EQ(pos->y, 250.0f));
    assert(paddle != NULL && FLOAT_EQ(paddle->width, config->paddle_width) && FLOAT_EQ(paddle->height, config->paddle_height));
    assert(score != NULL && score->points == 0);

    printf("✅ create_player() : OK\n");

    // Nettoyer
    entity_destroy(player);
}

static void test_create_ball()
{
    // Créer une balle complète
    Entity *const ball_entity = create_ball(400.0f, 300.0f, 150.0f, -75.0f);
    assert(ball_entity != NULL);

    const GameConfig *const config = game_config_get_current();

    // Vérifier que tous les composants sont présents
    Position *const pos = position_get(ball_entity);
    Velocity *const vel = velocity_get(ball_entity);
    Ball *const ball = ball_get(ball_entity);

    assert(pos != NULL && FLOAT_EQ(pos->x, 400.0f) && FLOAT_EQ(pos->y, 300.0f));
    assert(vel != NULL && FLOAT_EQ(vel->dx, 150.0f) && FLOAT_EQ(vel->dy, -75.0f));
    assert(ball != NULL && FLOAT_EQ(ball->radius, config->ball_radius));

    printf("✅ create_ball() : OK\n");

    // Nettoyer
    entity_destroy(ball_entity);
}

static void test_create_game_state()
{
    // Créer un game state
    Entity *const state_entity = create_game_state();
    assert(state_entity != NULL);

    // Vérifier que le composant GameState est présent
    GameState *const state = game_state_get(state_entity);
    assert(state != NULL && state->state == GAME_PLAYING);

    printf("✅ create_game_state() : OK\n");

    // Nettoyer
    entity_destroy(state_entity);
}

static void test_create_pong_game()
{
    // Créer un jeu Pong complet
    PongEntities entities = create_pong_game();

    // Vérifier que toutes les entités sont créées
    assert(entities.player1 != NULL);
    assert(entities.player2 != NULL);
    assert(entities.ball != NULL);
    assert(entities.game_state != NULL);

    // Vérifier les positions des joueurs
    Position *const p1_pos = position_get(entities.player1);
    Position *const p2_pos = position_get(entities.player2);
    assert(p1_pos != NULL && p1_pos->x < SCREEN_WIDTH / 2.0f); // Player1 à gauche
    assert(p2_pos != NULL && p2_pos->x > SCREEN_WIDTH / 2.0f); // Player2 à droite

    // Vérifier la balle au centre
    Position *const ball_pos = position_get(entities.ball);
    assert(ball_pos != NULL && FLOAT_EQ(ball_pos->x, SCREEN_WIDTH / 2.0f));
    assert(ball_pos != NULL && FLOAT_EQ(ball_pos->y, SCREEN_HEIGHT / 2.0f));

    printf("✅ create_pong_game() : OK\n");

    // Nettoyer avec la nouvelle fonction
    destroy_pong_game(&entities);
}

static void test_destroy_pong_game()
{
    // Créer et détruire un jeu complet
    PongEntities entities = create_pong_game();

    // Sauvegarder les pointeurs avant destruction
    Entity *const player1_ptr = entities.player1;
    Entity *const player2_ptr = entities.player2;
    Entity *const ball_ptr = entities.ball;
    Entity *const game_state_ptr = entities.game_state;

    // Vérifier que les entités sont valides avant destruction
    assert(entity_is_active(player1_ptr));
    assert(entity_is_active(player2_ptr));
    assert(entity_is_active(ball_ptr));
    assert(entity_is_active(game_state_ptr));

    // Détruire le jeu
    destroy_pong_game(&entities);

    // Vérifier que les entités ne sont plus actives (utiliser les anciens pointeurs)
    assert(!entity_is_active(player1_ptr));
    assert(!entity_is_active(player2_ptr));
    assert(!entity_is_active(ball_ptr));
    assert(!entity_is_active(game_state_ptr));

    // Vérifier que la structure a été remise à NULL
    assert(entities.player1 == NULL);
    assert(entities.player2 == NULL);
    assert(entities.ball == NULL);
    assert(entities.game_state == NULL);

    printf("✅ destroy_pong_game() : OK\n");
}

static void test_update_pong_game()
{
    // Créer un jeu
    PongEntities entities = create_pong_game();

    // Sauvegarder positions initiales
    Position *const ball_pos = position_get(entities.ball);
    const float initial_x = ball_pos->x;
    const float initial_y = ball_pos->y;

    // Faire un update avec delta_time (la balle devrait bouger)
    update_pong_game(0.016f); // ~60 FPS

    // Vérifier que la balle a bougé (inverse de FLOAT_EQ)
    const bool ball_moved = !FLOAT_EQ(ball_pos->x, initial_x) || !FLOAT_EQ(ball_pos->y, initial_y);
    assert(ball_moved);

    printf("✅ update_pong_game() : OK\n");

    // Nettoyer
    destroy_pong_game(&entities);
}

int main()
{
    printf("🧪 Test TDD : Entity Factory\n");
    test_create_player();
    test_create_ball();
    test_create_game_state();
    test_create_pong_game();
    test_destroy_pong_game();
    test_update_pong_game();
    return 0;
}
