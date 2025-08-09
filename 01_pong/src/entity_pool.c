// Pool statique d'entités avec initialisation paresseuse

#include "entity.h"

// Structure complète de l'entité (cachée du header)
struct Entity {
    uint32_t id;
    bool active;
};

// Getter du pool avec initialisation automatique
static struct Entity*
get_pool()
{
    static struct Entity pool[MAX_ENTITIES];
    static bool initialized = false;

    if (initialized)
        return pool;

    memset(pool, 0, sizeof(pool));
    initialized = true;
    return pool;
}

// Trouve un slot libre (fonction privée)
static bool
find_free_slot(uint32_t* const slot_index)
{
    struct Entity* const pool = get_pool();

    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        if (pool[i].active)
            continue;

        *slot_index = i;
        return true;
    }

    // Pool plein = bug de conception avec données statiques
    assert(false && "Pool d'entités plein - augmenter MAX_ENTITIES");
    return false;
}

// Trouve un slot par ID (fonction privée)
static bool
find_slot_by_id(const uint32_t id, uint32_t* const slot_index)
{
    struct Entity* const pool = get_pool();

    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        if (!pool[i].active || pool[i].id != id)
            continue;

        *slot_index = i;
        return true;
    }

    // ID non trouvé = cas normal pour entity_get_by_id avec ID invalide
    return false;
}

// ============================================================================
// FONCTIONS PUBLIQUES
// ============================================================================

Entity*
entity_create()
{
    uint32_t slot_index = 0;
    const bool found = find_free_slot(&slot_index);

    // Avec données statiques, pas de slot libre = bug de conception
    assert(found && "Impossible de créer une entité - pool plein");

    struct Entity* const pool = get_pool();
    struct Entity* const entity = &pool[slot_index];

    // Générateur d'ID statique
    static uint32_t next_id = 1;

    entity->id = next_id++;
    entity->active = true;

    return entity;
}

void
entity_destroy(Entity* const entity)
{
    assert(entity != NULL && "Entity à détruire ne peut pas être NULL");

    // Réinitialisation complète de l'entité
    memset(entity, 0, sizeof(struct Entity));
}

Entity*
entity_get_by_id(const uint32_t id)
{
    uint32_t slot_index = 0;
    const bool found = find_slot_by_id(id, &slot_index);

    if (!found)
        return NULL;

    struct Entity* const pool = get_pool();
    return &pool[slot_index];
}

uint32_t
entity_count_active()
{
    struct Entity* const pool = get_pool();
    uint32_t count = 0;

    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        if (!pool[i].active)
            continue;

        count++;
    }

    return count;
}

uint32_t
entity_get_id(const Entity* const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");
    return entity->id;
}

bool
entity_is_active(const Entity* const entity)
{
    assert(entity != NULL && "Entity ne peut pas être NULL");
    return entity->active;
}
