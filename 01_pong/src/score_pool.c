// Pool de composants Score - gestion des points des joueurs
// Approche Structure of Arrays (SoA) pour performance

#include "entity.h"
#include "score.h"

// Pools de données Score (SoA)
typedef struct {
    Score *scores; // Array contigu de scores
    bool *active;  // Array contigu de flags actifs
} ScorePools;

static ScorePools get_score_pools()
{
    static Score score_pool[MAX_ENTITIES];
    static bool score_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(score_pool, 0, sizeof(score_pool));
        memset(score_active, false, sizeof(score_active));
        initialized = true;
    }

    return (ScorePools) {.scores = score_pool, .active = score_active};
}

Score *score_add(Entity *const entity, const uint32_t initial_points)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour score_add");

    ScorePools pools = get_score_pools();
    pools.scores[index] = (Score) {.points = initial_points};
    pools.active[index] = true;

    return &pools.scores[index];
}

Score *score_get(Entity *const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    const uint32_t index = entity_get_id(entity) - 1;
    assert(index < MAX_ENTITIES && "Entity invalide pour score_get");

    ScorePools pools = get_score_pools();
    return pools.active[index] ? &pools.scores[index] : NULL;
}
