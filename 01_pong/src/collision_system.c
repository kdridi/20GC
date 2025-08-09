// Système Collision - gestion des collisions balle-paddle et balle-murs
// Détecte et résout les collisions entre objets de jeu

#include "collision_system.h"
#include "ball.h"
#include "entity.h"
#include "game_config.h"
#include "paddle.h"
#include "pch.h"
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

        const GameConfig *const config = game_config_get_current();

        // Collision avec les murs verticaux (haut/bas) - la balle rebondit
        if (pos->y <= ball->radius && vel->dy < 0.0f) {
            vel->dy = -vel->dy;    // Inverser la vitesse Y
            pos->y = ball->radius; // Corriger la position
        } else if (pos->y >= (float) config->screen_height - ball->radius && vel->dy > 0.0f) {
            vel->dy = -vel->dy;
            pos->y = (float) config->screen_height - ball->radius;
        }

        // NOTE: Pas de collision avec les murs horizontaux (gauche/droite)
        // La balle doit pouvoir sortir pour marquer des points

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
        // Position = centre des objets, convertir en coins pour collision
        const float ball_left = ball_pos->x - ball->radius;
        const float ball_right = ball_pos->x + ball->radius;
        const float ball_top = ball_pos->y - ball->radius;
        const float ball_bottom = ball_pos->y + ball->radius;

        const float paddle_left = paddle_pos->x - paddle->width / 2.0f;
        const float paddle_right = paddle_pos->x + paddle->width / 2.0f;
        const float paddle_top = paddle_pos->y - paddle->height / 2.0f;
        const float paddle_bottom = paddle_pos->y + paddle->height / 2.0f;

        const bool collision = (ball_right > paddle_left) &&
                               (ball_left < paddle_right) &&
                               (ball_bottom > paddle_top) &&
                               (ball_top < paddle_bottom);

        if (collision) {
            // Rebond horizontal
            ball_vel->dx = -ball_vel->dx;

            // Corriger la position pour éviter la pénétration
            // Position = centre, donc ajuster depuis le centre du paddle
            if (ball_vel->dx > 0.0f) {
                // Balle va vers la droite, placer à droite du paddle
                ball_pos->x = paddle_right + ball->radius;
            } else {
                // Balle va vers la gauche, placer à gauche du paddle
                ball_pos->x = paddle_left - ball->radius;
            }
        }
    }
}
