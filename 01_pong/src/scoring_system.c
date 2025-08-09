// Système Scoring - détection des points marqués
// Détecte quand la balle sort de l'écran et met à jour les scores

#include "scoring_system.h"
#include "ball.h"
#include "entity.h"
#include "game_config.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"

void scoring_system_update(void)
{
    // Trouver les scores des joueurs par position de paddle
    Score *player1_score = NULL; // Player à gauche
    Score *player2_score = NULL; // Player à droite

    for (uint32_t i = 1; i <= MAX_ENTITIES; ++i) {
        Entity *const entity = entity_get_by_id(i);
        if (entity == NULL || !entity_is_active(entity)) {
            continue;
        }

        Score *const score = score_get(entity);
        Paddle *const paddle = paddle_get(entity);
        Position *const paddle_pos = position_get(entity);

        // Cette entité doit avoir Score, Paddle et Position pour être un joueur
        if (score == NULL || paddle == NULL || paddle_pos == NULL) {
            continue;
        }

        const GameConfig *const config = game_config_get_current();

        // Identifier le joueur par la position du paddle
        if (paddle_pos->x < (float) config->screen_width / 2.0f) {
            player1_score = score; // Paddle à gauche = player1
        } else {
            player2_score = score; // Paddle à droite = player2
        }
    }

    // Vérifier qu'on a les deux joueurs
    if (player1_score == NULL || player2_score == NULL) {
        return;
    }

    const GameConfig *const config = game_config_get_current();

    // Parcourir toutes les entités pour trouver les balles
    for (uint32_t i = 1; i <= MAX_ENTITIES; ++i) {
        Entity *const entity = entity_get_by_id(i);
        if (entity == NULL || !entity_is_active(entity)) {
            continue;
        }

        Ball *const ball = ball_get(entity);
        Position *const position = position_get(entity);

        if (ball == NULL || position == NULL) {
            continue; // Pas une balle
        }

        // Vérifier si la balle a quitté l'écran
        if (position->x < 0.0f) {
            // Balle sortie par la gauche, player2 marque
            player2_score->points++;
        } else if (position->x > (float) config->screen_width) {
            // Balle sortie par la droite, player1 marque
            player1_score->points++;
        }
    }
}
