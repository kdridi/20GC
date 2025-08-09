// Test TDD pour movement_system_update()
// Système qui déplace les entités ayant Position + Velocity

#include "entity.h"
#include "movement_system.h"
#include "position.h"
#include "velocity.h"

static void test_movement_system_update()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Ajouter Position et Velocity à l'entité
    Position *const pos = position_add(entity, 10.0f, 20.0f);
    Velocity *const vel = velocity_add(entity, 5.0f, -3.0f);
    assert(pos != NULL && vel != NULL);

    // Position initiale
    assert(FLOAT_EQ(pos->x, 10.0f));
    assert(FLOAT_EQ(pos->y, 20.0f));

    // Appliquer le mouvement (delta time = 1.0f pour simplicité)
    movement_system_update(1.0f);

    // Vérifier la nouvelle position
    assert(FLOAT_EQ(pos->x, 15.0f)); // 10 + 5*1
    assert(FLOAT_EQ(pos->y, 17.0f)); // 20 + (-3)*1

    printf("✅ movement_system_update() : OK\n");
}

int main()
{
    printf("🧪 Test TDD : movement_system_update()\n");
    test_movement_system_update();
    return 0;
}
