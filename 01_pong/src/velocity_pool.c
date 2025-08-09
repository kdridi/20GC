// Pool statique de composants Velocity - Structure of Arrays pour performances

#include "entity.h"
#include "velocity.h"

// Structure of Arrays pour cache-friendly et SIMD
typedef struct {
    Velocity *velocities; // Array contigu de vitesses
    bool *active;         // Array contigu de flags actifs
} VelocityPools;

// Getter des pools avec initialisation paresseuse
static VelocityPools get_velocity_pools()
{
    static Velocity velocity_pool[MAX_ENTITIES];
    static bool velocity_active[MAX_ENTITIES];
    static bool initialized = false;

    if (!initialized) {
        memset(velocity_pool, 0, sizeof(velocity_pool));
        memset(velocity_active, false, sizeof(velocity_active));
        initialized = true;
    }

    return (VelocityPools) {velocity_pool, velocity_active};
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

Velocity *velocity_add(Entity *entity, float dx, float dy)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    uint32_t index;
    const bool found = find_entity_index(entity, &index);
    assert(found && "Entity invalide pour velocity_add");

    const VelocityPools pools = get_velocity_pools();

    pools.velocities[index].dx = dx;
    pools.velocities[index].dy = dy;
    pools.active[index] = true;

    return &pools.velocities[index];
}

Velocity *velocity_get(const Entity *entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");

    uint32_t index;
    if (!find_entity_index(entity, &index))
        return NULL;

    const VelocityPools pools = get_velocity_pools();
    if (!pools.active[index])
        return NULL;

    return &pools.velocities[index];
}
