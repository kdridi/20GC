// Pool de composants Ball - gestion des balles
// Approche Structure of Arrays (SoA) pour performance

#include "ball.h"
#include "entity.h"

// Pools de données Ball (SoA)
typedef struct {
    Ball *balls;  // Array contigu de balls
    bool *active; // Array contigu de flags actifs
} BallPools;

static BallPools get_ball_pools()
{
    static Ball ball_pool[MAX_ENTITIES];
    static bool ball_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(ball_pool, 0, sizeof(ball_pool));
        memset(ball_active, false, sizeof(ball_active));
        initialized = true;
    }

    return (BallPools) {.balls = ball_pool, .active = ball_active};
}

Ball *ball_add(Entity *const entity, const float radius)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour ball_add");

    BallPools pools = get_ball_pools();
    pools.balls[index] = (Ball) {.radius = radius};
    pools.active[index] = true;

    return &pools.balls[index];
}

Ball *ball_get(Entity *const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour ball_get");

    BallPools pools = get_ball_pools();
    return pools.active[index] ? &pools.balls[index] : NULL;
}
