// Système Reset - remet la balle au centre après un point
// Détecte les balles sorties de l'écran et les remet au centre

#include "reset_system.h"
#include "ball.h"
#include "entity.h"
#include "game_config.h"
#include "pch.h"
#include "position.h"
#include "velocity.h"

void reset_system_update(void)
{
    const GameConfig *const config = game_config_get_current();

    // Parcourir toutes les entités pour trouver les balles
    for (uint32_t i = 1; i <= MAX_ENTITIES; ++i) {
        Entity *const entity = entity_get_by_id(i);
        if (entity == NULL || !entity_is_active(entity)) {
            continue;
        }

        Ball *const ball = ball_get(entity);
        Position *const position = position_get(entity);
        Velocity *const velocity = velocity_get(entity);

        if (ball == NULL || position == NULL || velocity == NULL) {
            continue; // Pas une balle complète
        }

        // Vérifier si la balle a quitté l'écran
        if (position->x < 0.0f || position->x > (float) config->screen_width) {
            // Déterminer la direction avant de modifier la position
            const bool ball_out_left = position->x < 0.0f;

            // Remettre la balle au centre
            position->x = (float) config->screen_width / 2.0f;
            position->y = (float) config->screen_height / 2.0f;

            // Réinitialiser la vitesse avec direction appropriée
            if (ball_out_left) {
                // Sortie par la gauche, renvoyer vers la droite
                velocity->dx = config->ball_speed_x;
            } else {
                // Sortie par la droite, renvoyer vers la gauche
                velocity->dx = -config->ball_speed_x;
            }
            velocity->dy = config->ball_speed_y; // Vitesse verticale standard
        }
    }
}
