// Test TDD pour bounds_system_update()
// Système qui maintient les entités dans les limites du terrain

#include "bounds_system.h"
#include "entity.h"
#include "paddle.h"
#include "position.h"

static void test_bounds_system_paddle()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer un paddle en bas de l'écran (limite dépassée)
    Position *const pos = position_add(entity, 400.0f, 650.0f); // Y trop grand
    Paddle *const paddle = paddle_add(entity, 10.0f, 60.0f);
    assert(pos != NULL && paddle != NULL);

    // Vérifier position initiale (hors limites)
    assert(FLOAT_EQ(pos->y, 650.0f));

    // Appliquer le système bounds
    bounds_system_update();

    // Vérifier que le paddle est maintenu dans les limites
    // SCREEN_HEIGHT = 600, paddle height = 60, position = centre
    // donc max Y = 600 - 60/2 = 570
    assert(pos->y <= 570.0f);

    printf("✅ bounds_system_update() paddle : OK\n");
}

int main()
{
    printf("🧪 Test TDD : bounds_system_update()\n");
    test_bounds_system_paddle();
    return 0;
}
