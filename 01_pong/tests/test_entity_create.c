// Test TDD pour entity_create()
// Une fonction = un fichier = un test focalisé

#include "entity.h"

static void
test_entity_create()
{
    Entity* const entity = entity_create();

    // Vérification du comportement attendu
    assert(entity != NULL);
    assert(entity_get_id(entity) > 0);
    assert(entity_is_active(entity) == true);

    printf("✅ entity_create() : OK\n");
}

int
main()
{
    printf("🧪 Test TDD : entity_create()\n");
    test_entity_create();
    return 0;
}
