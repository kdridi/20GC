// Test TDD pour paddle_add()
// Composant Paddle pour les raquettes de Pong

#include "entity.h"
#include "paddle.h"

static void test_paddle_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    const float width = 10.0f;
    const float height = 60.0f;
    Paddle *const paddle = paddle_add(entity, width, height);

    assert(paddle != NULL);
    assert(FLOAT_EQ(paddle->width, width));
    assert(FLOAT_EQ(paddle->height, height));

    printf("✅ paddle_add() : OK\n");
}

int main()
{
    printf("🧪 Test TDD : paddle_add()\n");
    test_paddle_add();
    return 0;
}
