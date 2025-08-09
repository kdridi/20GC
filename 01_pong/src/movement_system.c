// Système Movement - applique les vitesses aux positions
// Approche 1: Itération par entités avec APIs get()

#include "movement_system.h"
#include "entity.h"
#include "position.h"
#include "velocity.h"

void movement_system_update(float delta_time)
{
    // Parcours de toutes les entités possibles
    for (uint32_t entity_id = 1; entity_id <= MAX_ENTITIES; entity_id++) {
        Entity *const entity = entity_get_by_id(entity_id);
        if (!entity || !entity_is_active(entity))
            continue;

        // Récupérer les composants (NULL si pas présents)
        Position *const pos = position_get(entity);
        Velocity *const vel = velocity_get(entity);

        // Traiter seulement les entités qui ont les deux composants
        if (!pos || !vel)
            continue;

        // Appliquer la vitesse à la position
        pos->x += vel->dx * delta_time;
        pos->y += vel->dy * delta_time;
    }
}
