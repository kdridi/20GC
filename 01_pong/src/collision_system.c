// Système Collision - gestion des collisions balle-paddle et balle-murs
// Détecte et résout les collisions entre objets de jeu

#include "collision_system.h"
#include "ball.h"
#include "entity.h"
#include "paddle.h"
#include "position.h"
#include "velocity.h"

// Déclaration forward de la fonction statique
static void collision_check_ball_paddle(Entity *const ball_entity, Position *const ball_pos,
                                        Velocity *const ball_vel, Ball *const ball);

void collision_system_update()
{
    // Parcours de toutes les entités possibles pour trouver les balles
    for (uint32_t entity_id = 1; entity_id <= MAX_ENTITIES; entity_id++) {
        Entity *const entity = entity_get_by_id(entity_id);
        if (!entity || !entity_is_active(entity))
            continue;

        Position *const pos = position_get(entity);
        Velocity *const vel = velocity_get(entity);
        Ball *const ball = ball_get(entity);

        // Ignorer si ce n'est pas une balle avec position et vitesse
        if (!pos || !vel || !ball)
            continue;

        // Collision avec les murs (limites horizontales)
        if (pos->x <= 0.0f && vel->dx < 0.0f) {
            vel->dx = -vel->dx; // Inverser la vitesse X
            pos->x = 0.0f;      // Corriger la position
        } else if (pos->x >= SCREEN_WIDTH && vel->dx > 0.0f) {
            vel->dx = -vel->dx;
            pos->x = SCREEN_WIDTH;
        }

        // Collision avec les paddles
        collision_check_ball_paddle(entity, pos, vel, ball);
    }
}

static void collision_check_ball_paddle(Entity *const ball_entity, Position *const ball_pos,
                                        Velocity *const ball_vel, Ball *const ball)
{
    // Parcours des entités pour trouver les paddles
    for (uint32_t entity_id = 1; entity_id <= MAX_ENTITIES; entity_id++) {
        Entity *const paddle_entity = entity_get_by_id(entity_id);
        if (!paddle_entity || !entity_is_active(paddle_entity) || paddle_entity == ball_entity)
            continue;

        Position *const paddle_pos = position_get(paddle_entity);
        Paddle *const paddle = paddle_get(paddle_entity);

        if (!paddle_pos || !paddle)
            continue;

        // Détection de collision AABB (Axis-Aligned Bounding Box)
        // Balle comme rectangle de taille (radius*2, radius*2)
        const float ball_width = ball->radius * 2.0f;
        const float ball_height = ball->radius * 2.0f;

        const bool collision = (ball_pos->x < paddle_pos->x + paddle->width) &&
                               (ball_pos->x + ball_width > paddle_pos->x) &&
                               (ball_pos->y < paddle_pos->y + paddle->height) &&
                               (ball_pos->y + ball_height > paddle_pos->y);

        if (collision) {
            // Rebond horizontal
            ball_vel->dx = -ball_vel->dx;

            // Corriger la position pour éviter la pénétration
            if (ball_vel->dx > 0.0f) {
                ball_pos->x = paddle_pos->x + paddle->width;
            } else {
                ball_pos->x = paddle_pos->x - ball->radius * 2.0f;
            }
        }
    }
}
