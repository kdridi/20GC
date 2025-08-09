// Système Input - gestion des entrées pour contrôler les paddles
// Actions métier pour les deux joueurs de Pong

#include "input_system.h"
#include "entity.h"
#include "game_config.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"

// État des actions de jeu
static bool action_states[4] = {false, false, false, false};

void input_system_set_action(const GameAction action, const bool active)
{
    assert(action >= 0 && action < 4 && "Action invalide");
    action_states[action] = active;
}

void input_system_update()
{
    const GameConfig *const config = game_config_get_current();
    // Utiliser delta_time = 1/60 pour une vitesse constante
    // TODO: Passer delta_time en paramètre quand SDL2 sera intégré
    const float paddle_speed = config->paddle_speed / 60.0f;

    // Parcours de toutes les entités pour trouver les paddles
    for (uint32_t entity_id = 1; entity_id <= MAX_ENTITIES; entity_id++) {
        Entity *const entity = entity_get_by_id(entity_id);
        if (!entity || !entity_is_active(entity))
            continue;

        Position *const pos = position_get(entity);
        Paddle *const paddle = paddle_get(entity);

        // Ignorer si ce n'est pas un paddle avec position
        if (!pos || !paddle)
            continue;

        // Déterminer quel joueur selon la position X
        // Joueur 1 = paddle gauche (X < centre écran)
        // Joueur 2 = paddle droit (X >= centre écran)
        const bool is_player1 = pos->x < ((float) config->screen_width / 2.0f);

        if (is_player1) {
            // Contrôles joueur 1
            if (action_states[PLAYER1_PADDLE_UP])
                pos->y -= paddle_speed;
            if (action_states[PLAYER1_PADDLE_DOWN])
                pos->y += paddle_speed;
        } else {
            // Contrôles joueur 2
            if (action_states[PLAYER2_PADDLE_UP])
                pos->y -= paddle_speed;
            if (action_states[PLAYER2_PADDLE_DOWN])
                pos->y += paddle_speed;
        }
    }
}
