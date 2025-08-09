// Test TDD pour game_state_add()
// Composant GameState pour gérer l'état du jeu

#include "entity.h"
#include "game_state.h"

static void test_game_state_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Ajouter un composant GameState en état PLAYING
    GameState *const state = game_state_add(entity, GAME_PLAYING);
    assert(state != NULL);

    // Vérifier l'état initial
    assert(state->state == GAME_PLAYING);

    printf("✅ game_state_add() : OK\n");
}

static void test_game_state_transitions()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer avec état initial PLAYING
    GameState *const state = game_state_add(entity, GAME_PLAYING);
    assert(state != NULL && state->state == GAME_PLAYING);

    // Transition vers PAUSED
    state->state = GAME_PAUSED;
    assert(state->state == GAME_PAUSED);

    // Transition vers ENDED
    state->state = GAME_ENDED;
    assert(state->state == GAME_ENDED);

    // Vérifier via get() aussi
    GameState *const retrieved_state = game_state_get(entity);
    assert(retrieved_state != NULL && retrieved_state->state == GAME_ENDED);

    printf("✅ game state transitions : OK\n");
}

static void test_game_state_get_null_for_no_component()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Récupérer state d'une entité sans composant GameState
    GameState *const state = game_state_get(entity);
    assert(state == NULL);

    printf("✅ game_state_get() null sans composant : OK\n");
}

int main()
{
    printf("🧪 Test TDD : GameState component\n");
    test_game_state_add();
    test_game_state_transitions();
    test_game_state_get_null_for_no_component();
    return 0;
}
