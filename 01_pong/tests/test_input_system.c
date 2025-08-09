// Test TDD pour input_system_update()
// Système qui gère les entrées pour contrôler les paddles des deux joueurs

#include "entity.h"
#include "input_system.h"
#include "paddle.h"
#include "position.h"

static void test_input_system_player1_paddle_move_up()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer un paddle joueur 1
    Position *const pos = position_add(entity, 50.0f, 300.0f);
    Paddle *const paddle = paddle_add(entity, 10.0f, 60.0f);
    assert(pos != NULL && paddle != NULL);

    // Position initiale
    const float initial_y = pos->y;

    // Simuler action "joueur 1 déplace paddle vers le haut"
    input_system_set_action(PLAYER1_PADDLE_UP, true);
    input_system_update();

    // Vérifier que le paddle a bougé vers le haut
    assert(pos->y < initial_y);

    printf("✅ input_system_update() player1 paddle up : OK\n");
}

static void test_input_system_player2_paddle_move_down()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer un paddle joueur 2
    Position *const pos = position_add(entity, 750.0f, 250.0f);
    Paddle *const paddle = paddle_add(entity, 10.0f, 60.0f);
    assert(pos != NULL && paddle != NULL);

    // Position initiale
    const float initial_y = pos->y;

    // Simuler action "joueur 2 déplace paddle vers le bas"
    input_system_set_action(PLAYER2_PADDLE_DOWN, true);
    input_system_update();

    // Vérifier que le paddle a bougé vers le bas
    assert(pos->y > initial_y);

    printf("✅ input_system_update() player2 paddle down : OK\n");
}

static void test_input_system_no_input()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer un paddle
    Position *const pos = position_add(entity, 50.0f, 300.0f);
    Paddle *const paddle = paddle_add(entity, 10.0f, 60.0f);
    assert(pos != NULL && paddle != NULL);

    // Position initiale
    const float initial_y = pos->y;

    // Aucune action active
    input_system_set_action(PLAYER1_PADDLE_UP, false);
    input_system_set_action(PLAYER1_PADDLE_DOWN, false);
    input_system_set_action(PLAYER2_PADDLE_UP, false);
    input_system_set_action(PLAYER2_PADDLE_DOWN, false);
    input_system_update();

    // Vérifier que le paddle n'a pas bougé
    assert(FLOAT_EQ(pos->y, initial_y));

    printf("✅ input_system_update() no input : OK\n");
}

int main()
{
    printf("🧪 Test TDD : input_system_update()\n");
    test_input_system_player1_paddle_move_up();
    test_input_system_player2_paddle_move_down();
    test_input_system_no_input();
    return 0;
}
