// Test TDD pour entity_destroy()

#include "entity.h"

static void
test_entity_destroy()
{
    Entity* const entity = entity_create();
    assert(entity != NULL);

    entity_destroy(entity);

    // Après destruction, l'entité doit être inactive
    assert(entity_is_active(entity) == false);

    printf("✅ entity_destroy() : OK\n");
}

int
main()
{
    printf("🧪 Test TDD : entity_destroy()\n");
    test_entity_destroy();
    return 0;
}
