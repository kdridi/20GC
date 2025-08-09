// Test TDD pour velocity_add()
// Composant vitesse pour entités mobiles

#include "entity.h"
#include "velocity.h"

static void test_velocity_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    Velocity *const vel = velocity_add(entity, 5.0f, -3.0f);

    // Vérifications
    assert(vel != NULL);
    assert(FLOAT_EQ(vel->dx, 5.0f));
    assert(FLOAT_EQ(vel->dy, -3.0f));
    assert(velocity_get(entity) != NULL);

    printf("✅ velocity_add() : OK\n");
}

int main()
{
    printf("🧪 Test TDD : velocity_add()\n");
    test_velocity_add();
    return 0;
}
