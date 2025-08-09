// Test TDD pour position_add()
// Premier test : ajouter un composant Position à une entité

#include "entity.h"
#include "position.h"

static void test_position_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    Position *const pos = position_add(entity, 10.0f, 20.0f);

    // Vérifications
    assert(pos != NULL);
    assert(pos->x == 10.0f);
    assert(pos->y == 20.0f);
    assert(position_get(entity) != NULL);

    printf("✅ position_add() : OK\n");
}

int main()
{
    printf("🧪 Test TDD : position_add()\n");
    test_position_add();
    return 0;
}
