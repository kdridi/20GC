// Pool de composants GameState - gestion de l'état du jeu
// Approche Structure of Arrays (SoA) pour performance

#include "entity.h"
#include "game_state.h"

// Pools de données GameState (SoA)
typedef struct {
    GameState *game_states; // Array contigu de states
    bool *active;           // Array contigu de flags actifs
} GameStatePools;

static GameStatePools get_game_state_pools()
{
    static GameState game_state_pool[MAX_ENTITIES];
    static bool game_state_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(game_state_pool, 0, sizeof(game_state_pool));
        memset(game_state_active, false, sizeof(game_state_active));
        initialized = true;
    }

    return (GameStatePools) {.game_states = game_state_pool, .active = game_state_active};
}

GameState *game_state_add(Entity *const entity, const GameStateValue initial_state)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour game_state_add");

    GameStatePools pools = get_game_state_pools();
    pools.game_states[index] = (GameState) {.state = initial_state};
    pools.active[index] = true;

    return &pools.game_states[index];
}

GameState *game_state_get(Entity *const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour game_state_get");

    GameStatePools pools = get_game_state_pools();
    return pools.active[index] ? &pools.game_states[index] : NULL;
}
