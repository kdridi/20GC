// Système Bounds - maintient les entités dans les limites du terrain
// Empêche les paddles de sortir de l'écran

#include "bounds_system.h"
#include "entity.h"
#include "game_config.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"

void bounds_system_update()
{
    // Parcours de toutes les entités possibles
    for (uint32_t entity_id = 1; entity_id <= MAX_ENTITIES; entity_id++) {
        Entity *const entity = entity_get_by_id(entity_id);
        if (!entity || !entity_is_active(entity))
            continue;

        Position *const pos = position_get(entity);
        if (!pos)
            continue;

        // Vérifier si l'entité est un paddle
        Paddle *const paddle = paddle_get(entity);
        if (paddle) {
            const GameConfig *const config = game_config_get_current();

            // Contraindre le paddle dans les limites verticales
            // Le paddle ne peut pas dépasser le haut ou le bas de l'écran
            const float min_y = 0.0f;
            const float max_y = (float) config->screen_height - paddle->height;

            if (pos->y < min_y)
                pos->y = min_y;
            else if (pos->y > max_y)
                pos->y = max_y;
        }
    }
}
