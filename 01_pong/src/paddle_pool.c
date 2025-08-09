// Pool de composants Paddle - gestion des raquettes
// Approche Structure of Arrays (SoA) pour performance

#include "entity.h"
#include "paddle.h"

// Pools de données Paddle (SoA)
typedef struct {
    Paddle *paddles; // Array contigu de paddles
    bool *active;    // Array contigu de flags actifs
} PaddlePools;

static PaddlePools get_paddle_pools()
{
    static Paddle paddle_pool[MAX_ENTITIES];
    static bool paddle_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(paddle_pool, 0, sizeof(paddle_pool));
        memset(paddle_active, false, sizeof(paddle_active));
        initialized = true;
    }

    return (PaddlePools) {.paddles = paddle_pool, .active = paddle_active};
}

Paddle *paddle_add(Entity *const entity, const float width, const float height)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour paddle_add");

    PaddlePools pools = get_paddle_pools();
    pools.paddles[index] = (Paddle) {.width = width, .height = height};
    pools.active[index] = true;

    return &pools.paddles[index];
}

Paddle *paddle_get(Entity *const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour paddle_get");

    PaddlePools pools = get_paddle_pools();
    return pools.active[index] ? &pools.paddles[index] : NULL;
}
