// Test TDD pour collision_system_update()
// Système qui gère les collisions balle-paddle et balle-murs

#include "ball.h"
#include "collision_system.h"
#include "entity.h"
#include "paddle.h"
#include "position.h"
#include "velocity.h"

static void test_collision_system_ball_wall_bounce()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer une balle qui va vers la gauche (hors limites)
    Position *const pos = position_add(entity, -5.0f, 300.0f); // X négatif = hors écran
    Velocity *const vel = velocity_add(entity, -100.0f, 0.0f); // Vitesse vers la gauche
    Ball *const ball = ball_add(entity, 4.0f);
    assert(pos != NULL && vel != NULL && ball != NULL);

    // Vérifier état initial
    assert(FLOAT_EQ(pos->x, -5.0f));
    assert(FLOAT_EQ(vel->dx, -100.0f));

    // Appliquer le système collision
    collision_system_update();

    // Vérifier que la balle a rebondi (vitesse inversée en X)
    assert(vel->dx > 0.0f); // Vitesse maintenant vers la droite
    assert(pos->x >= 0.0f); // Position corrigée dans l'écran

    printf("✅ collision_system_update() ball-wall : OK\n");
}

static void test_collision_system_ball_paddle_bounce()
{
    // Créer balle et paddle
    Entity *const ball_entity = entity_create();
    Entity *const paddle_entity = entity_create();
    assert(ball_entity != NULL && paddle_entity != NULL);

    // Balle qui se dirige vers le paddle
    Position *const ball_pos = position_add(ball_entity, 45.0f, 300.0f);
    Velocity *const ball_vel = velocity_add(ball_entity, -50.0f, 0.0f); // Vers la gauche
    Ball *const ball = ball_add(ball_entity, 4.0f);

    // Paddle positionné pour intercepter
    Position *const paddle_pos = position_add(paddle_entity, 40.0f, 280.0f);
    Paddle *const paddle = paddle_add(paddle_entity, 10.0f, 60.0f);

    assert(ball_pos != NULL && ball_vel != NULL && ball != NULL);
    assert(paddle_pos != NULL && paddle != NULL);

    // État initial
    assert(FLOAT_EQ(ball_vel->dx, -50.0f));

    // Appliquer collision
    collision_system_update();

    // Vérifier rebond de la balle sur le paddle
    assert(ball_vel->dx > 0.0f); // Vitesse inversée

    printf("✅ collision_system_update() ball-paddle : OK\n");
}

int main()
{
    printf("🧪 Test TDD : collision_system_update()\n");
    test_collision_system_ball_wall_bounce();
    test_collision_system_ball_paddle_bounce();
    return 0;
}
