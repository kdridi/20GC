// Test TDD pour reset_system_update()
// Système qui remet la balle au centre après un point marqué

#include "ball.h"
#include "entity.h"
#include "position.h"
#include "reset_system.h"
#include "velocity.h"

static void test_reset_system_ball_out_left()
{
    // Créer une balle qui est sortie par la gauche
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, -20.0f, 250.0f); // Hors écran à gauche
    Velocity *const ball_vel = velocity_add(ball_entity, -100.0f, 50.0f);
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball_vel != NULL && ball != NULL);

    // Vérifier état initial
    assert(ball_pos->x < 0.0f);

    // Appliquer le système reset
    reset_system_update();

    // Vérifier que la balle est revenue au centre
    assert(FLOAT_EQ(ball_pos->x, SCREEN_WIDTH / 2.0f));
    assert(FLOAT_EQ(ball_pos->y, SCREEN_HEIGHT / 2.0f));

    // Vérifier que la vitesse a été réinitialisée (direction vers la droite)
    assert(ball_vel->dx > 0.0f);

    printf("✅ reset_system_update() ball out left : OK\n");

    // Nettoyer
    entity_destroy(ball_entity);
}

static void test_reset_system_ball_out_right()
{
    // Créer une balle qui est sortie par la droite
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, 850.0f, 200.0f); // Hors écran à droite
    Velocity *const ball_vel = velocity_add(ball_entity, 100.0f, -30.0f);
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball_vel != NULL && ball != NULL);

    // Vérifier état initial
    assert(ball_pos->x > SCREEN_WIDTH);

    // Appliquer le système reset
    reset_system_update();

    // Vérifier que la balle est revenue au centre
    assert(FLOAT_EQ(ball_pos->x, SCREEN_WIDTH / 2.0f));
    assert(FLOAT_EQ(ball_pos->y, SCREEN_HEIGHT / 2.0f));

    // Vérifier que la vitesse a été réinitialisée (direction vers la gauche)
    assert(ball_vel->dx < 0.0f);

    printf("✅ reset_system_update() ball out right : OK\n");

    // Nettoyer
    entity_destroy(ball_entity);
}

static void test_reset_system_ball_in_bounds()
{
    // Créer une balle qui est encore dans les limites
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, 400.0f, 300.0f); // Au centre
    Velocity *const ball_vel = velocity_add(ball_entity, 150.0f, -75.0f);
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball_vel != NULL && ball != NULL);

    // Sauvegarder l'état initial
    const float initial_x = ball_pos->x;
    const float initial_y = ball_pos->y;
    const float initial_dx = ball_vel->dx;
    const float initial_dy = ball_vel->dy;

    // Appliquer le système reset
    reset_system_update();

    // Vérifier que rien n'a changé
    assert(FLOAT_EQ(ball_pos->x, initial_x));
    assert(FLOAT_EQ(ball_pos->y, initial_y));
    assert(FLOAT_EQ(ball_vel->dx, initial_dx));
    assert(FLOAT_EQ(ball_vel->dy, initial_dy));

    printf("✅ reset_system_update() ball in bounds : OK\n");

    // Nettoyer
    entity_destroy(ball_entity);
}

int main()
{
    printf("🧪 Test TDD : reset_system_update()\n");
    test_reset_system_ball_out_left();
    test_reset_system_ball_out_right();
    test_reset_system_ball_in_bounds();
    return 0;
}
