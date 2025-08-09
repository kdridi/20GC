// Test TDD pour score_add()
// Composant Score pour gérer les points des joueurs

#include "entity.h"
#include "score.h"

static void test_score_add()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Ajouter un composant Score avec 0 points initiaux
    Score *const score = score_add(entity, 0);
    assert(score != NULL);

    // Vérifier les valeurs initiales
    assert(score->points == 0);

    printf("✅ score_add() : OK\n");
}

static void test_score_direct_access()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Créer un score à 3 points
    Score *const score = score_add(entity, 3);
    assert(score != NULL && score->points == 3);

    // Incrémenter directement (comme les autres composants)
    score->points++;

    // Vérifier l'incrémentation
    assert(score->points == 4);

    // Vérifier via get() aussi
    Score *const retrieved_score = score_get(entity);
    assert(retrieved_score != NULL && retrieved_score->points == 4);

    printf("✅ score direct access : OK\n");
}

static void test_score_get_null_for_no_component()
{
    Entity *const entity = entity_create();
    assert(entity != NULL);

    // Récupérer score d'une entité sans composant Score
    Score *const score = score_get(entity);
    assert(score == NULL);

    printf("✅ score_get() null sans composant : OK\n");
}

int main()
{
    printf("🧪 Test TDD : Score component\n");
    test_score_add();
    test_score_direct_access();
    test_score_get_null_for_no_component();
    return 0;
}
