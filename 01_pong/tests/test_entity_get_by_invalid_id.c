// Test TDD pour entity_get_by_id() - cas d'erreur ID invalide

#include "entity.h"

static void
test_entity_get_by_invalid_id()
{
    Entity* const found = entity_get_by_id(99999);

    // ID invalide doit retourner NULL
    assert(found == NULL);

    printf("✅ entity_get_by_id() invalid ID : OK\n");
}

int
main()
{
    printf("🧪 Test TDD : entity_get_by_id() - ID invalide\n");
    test_entity_get_by_invalid_id();
    return 0;
}
