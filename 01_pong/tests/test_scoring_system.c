// Test TDD pour scoring_system_update()
// Système qui détecte les points marqués et met à jour les scores

#include "ball.h"
#include "entity.h"
#include "paddle.h"
#include "position.h"
#include "score.h"
#include "scoring_system.h"

static void test_scoring_system_player1_scores()
{
    // Créer les entités de joueurs complètes (Score + Paddle + Position)
    Entity *const player1_entity = entity_create();
    Entity *const player2_entity = entity_create();
    assert(player1_entity != NULL && player2_entity != NULL);

    // Player1 à gauche (x = 50)
    Score *const player1_score = score_add(player1_entity, 0);
    Position *const player1_pos = position_add(player1_entity, 50.0f, 300.0f);
    Paddle *const player1_paddle = paddle_add(player1_entity, 10.0f, 60.0f);

    // Player2 à droite (x = 750)
    Score *const player2_score = score_add(player2_entity, 0);
    Position *const player2_pos = position_add(player2_entity, 750.0f, 300.0f);
    Paddle *const player2_paddle = paddle_add(player2_entity, 10.0f, 60.0f);

    assert(player1_score != NULL && player1_pos != NULL && player1_paddle != NULL);
    assert(player2_score != NULL && player2_pos != NULL && player2_paddle != NULL);

    // Créer une balle qui sort par la droite (player1 marque)
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, 850.0f, 300.0f); // Hors écran à droite
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball != NULL);

    // Vérifier scores initiaux
    assert(player1_score->points == 0);
    assert(player2_score->points == 0);

    // Appliquer le système scoring
    scoring_system_update();

    // Vérifier que player1 a marqué
    assert(player1_score->points == 1);
    assert(player2_score->points == 0);

    printf("✅ scoring_system_update() player1 scores : OK\n");

    // Nettoyer les entités pour isoler les tests
    entity_destroy(player1_entity);
    entity_destroy(player2_entity);
    entity_destroy(ball_entity);
}

static void test_scoring_system_player2_scores()
{
    // Créer les entités de joueurs complètes (Score + Paddle + Position)
    Entity *const player1_entity = entity_create();
    Entity *const player2_entity = entity_create();
    assert(player1_entity != NULL && player2_entity != NULL);

    // Player1 à gauche (x = 50)
    Score *const player1_score = score_add(player1_entity, 0);
    Position *const player1_pos = position_add(player1_entity, 50.0f, 300.0f);
    Paddle *const player1_paddle = paddle_add(player1_entity, 10.0f, 60.0f);

    // Player2 à droite (x = 750)
    Score *const player2_score = score_add(player2_entity, 0);
    Position *const player2_pos = position_add(player2_entity, 750.0f, 300.0f);
    Paddle *const player2_paddle = paddle_add(player2_entity, 10.0f, 60.0f);

    assert(player1_score != NULL && player1_pos != NULL && player1_paddle != NULL);
    assert(player2_score != NULL && player2_pos != NULL && player2_paddle != NULL);

    // Créer une balle qui sort par la gauche (player2 marque)
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, -10.0f, 300.0f); // Hors écran à gauche
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball != NULL);

    // Vérifier scores initiaux
    assert(player1_score->points == 0);
    assert(player2_score->points == 0);

    // Appliquer le système scoring
    scoring_system_update();

    // Vérifier que player2 a marqué
    assert(player1_score->points == 0);
    assert(player2_score->points == 1);

    printf("✅ scoring_system_update() player2 scores : OK\n");

    // Nettoyer les entités pour isoler les tests
    entity_destroy(player1_entity);
    entity_destroy(player2_entity);
    entity_destroy(ball_entity);
}

static void test_scoring_system_no_score()
{
    // Créer les entités de joueurs complètes (Score + Paddle + Position)
    Entity *const player1_entity = entity_create();
    Entity *const player2_entity = entity_create();
    assert(player1_entity != NULL && player2_entity != NULL);

    // Player1 à gauche (x = 50)
    Score *const player1_score = score_add(player1_entity, 0);
    Position *const player1_pos = position_add(player1_entity, 50.0f, 300.0f);
    Paddle *const player1_paddle = paddle_add(player1_entity, 10.0f, 60.0f);

    // Player2 à droite (x = 750)
    Score *const player2_score = score_add(player2_entity, 0);
    Position *const player2_pos = position_add(player2_entity, 750.0f, 300.0f);
    Paddle *const player2_paddle = paddle_add(player2_entity, 10.0f, 60.0f);

    assert(player1_score != NULL && player1_pos != NULL && player1_paddle != NULL);
    assert(player2_score != NULL && player2_pos != NULL && player2_paddle != NULL);

    // Créer une balle encore dans les limites de l'écran
    Entity *const ball_entity = entity_create();
    assert(ball_entity != NULL);

    Position *const ball_pos = position_add(ball_entity, 400.0f, 300.0f); // Au centre de l'écran
    Ball *const ball = ball_add(ball_entity, 4.0f);
    assert(ball_pos != NULL && ball != NULL);

    // Vérifier scores initiaux
    assert(player1_score->points == 0);
    assert(player2_score->points == 0);

    // Appliquer le système scoring
    scoring_system_update();

    // Vérifier qu'aucun point n'a été marqué
    assert(player1_score->points == 0);
    assert(player2_score->points == 0);

    printf("✅ scoring_system_update() no score : OK\n");

    // Nettoyer les entités pour isoler les tests
    entity_destroy(player1_entity);
    entity_destroy(player2_entity);
    entity_destroy(ball_entity);
}

int main()
{
    printf("🧪 Test TDD : scoring_system_update()\n");
    test_scoring_system_player1_scores();
    test_scoring_system_player2_scores();
    test_scoring_system_no_score();
    return 0;
}
