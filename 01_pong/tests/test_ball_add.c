// Test TDD pour ball_add()
// Composant Ball pour les balles de Pong

#include "ball.h"
#include "entity.h"

static void test_ball_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    const float radius = 5.0f;
    Ball *const ball = ball_add(entity, radius);

    assert(ball != NULL);
    assert(FLOAT_EQ(ball->radius, radius));

    printf("✅ ball_add() : OK\n");
}

int main()
{
    printf("🧪 Test TDD : ball_add()\n");
    test_ball_add();
    return 0;
}
