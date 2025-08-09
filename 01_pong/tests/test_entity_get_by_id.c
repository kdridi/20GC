// Test TDD pour entity_get_by_id() - cas nominal

#include "entity.h"

static void
test_entity_get_by_id()
{
    Entity* const entity = entity_create();
    const uint32_t id = entity_get_id(entity);

    Entity* const found = entity_get_by_id(id);

    // Doit retrouver la même entité
    assert(found == entity);
    assert(entity_get_id(found) == id);
    assert(entity_is_active(found) == true);

    printf("✅ entity_get_by_id() : OK\n");
}

int
main()
{
    printf("🧪 Test TDD : entity_get_by_id()\n");
    test_entity_get_by_id();
    return 0;
}
