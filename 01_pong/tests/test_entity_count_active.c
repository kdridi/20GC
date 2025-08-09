// Test TDD pour entity_count_active()

#include "entity.h"

static void
test_entity_count_active()
{
    const uint32_t initial_count = entity_count_active();

    Entity* const entity1 = entity_create();
    assert(entity_count_active() == initial_count + 1);

    Entity* const entity2 = entity_create();
    assert(entity_count_active() == initial_count + 2);

    entity_destroy(entity1);
    entity_destroy(entity2); // Détruire aussi entity2
    assert(entity_count_active() == initial_count);

    printf("✅ entity_count_active() : OK\n");
}

int
main()
{
    printf("🧪 Test TDD : entity_count_active()\n");
    test_entity_count_active();
    return 0;
}
