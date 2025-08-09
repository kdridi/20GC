// Pool statique de composants Position - Structure of Arrays pour performances

#include "entity.h"
#include "position.h"

// Structure of Arrays pour cache-friendly et SIMD
typedef struct {
    Position *positions; // Array contigu de positions
    bool *active;        // Array contigu de flags actifs
} PositionPools;

// Getter des pools avec initialisation paresseuse
static PositionPools get_position_pools()
{
    static Position position_pool[MAX_ENTITIES];
    static bool position_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(position_pool, 0, sizeof(position_pool));
        memset(position_active, false, sizeof(position_active));
        initialized = true;
    }

    return (PositionPools) {position_pool, position_active};
}

// Trouve l'index d'une entité (utilise l'ID pour l'instant)
static bool find_entity_index(const Entity *entity, uint32_t *index)
{
    const uint32_t id = entity_get_id(entity);
    if (id == 0 || id > MAX_ENTITIES)
        return false;

    *index = id - 1; // Les IDs commencent à 1
    return true;
}

Position *position_add(Entity *entity, float x, float y)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    uint32_t index;
    const bool found = find_entity_index(entity, &index);
    assert(found && "Entity invalide pour position_add");

    const PositionPools pools = get_position_pools();

    pools.positions[index].x = x;
    pools.positions[index].y = y;
    pools.active[index] = true;

    return &pools.positions[index];
}

Position *position_get(const Entity *entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    uint32_t index;
    if (!find_entity_index(entity, &index))
        return NULL;

    const PositionPools pools = get_position_pools();
    if (!pools.active[index])
        return NULL;

    return &pools.positions[index];
}
