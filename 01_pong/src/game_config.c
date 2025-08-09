#include "game_config.h"
#include "pch.h"

// Configuration par défaut du jeu
static const GameConfig default_config = {
    // Dimensions de l'écran
    .screen_width = 800,
    .screen_height = 600,

    // Configuration des paddles
    .paddle_width = 12.0f,
    .paddle_height = 80.0f,
    .paddle_speed = 300.0f, // Pixels par seconde

    // Positions initiales des joueurs
    .player1_x = 50.0f,  // Joueur 1 à gauche
    .player2_x = 750.0f, // Joueur 2 à droite (800 - 50)

    // Configuration de la balle
    .ball_radius = 4.0f,
    .ball_speed_x = 150.0f, // Vitesse horizontale initiale
    .ball_speed_y = 100.0f, // Vitesse verticale initiale

    // Paramètres de jeu
    .max_score = 11,    // Premier à 11 points gagne
    .points_to_win = 11 // Points nécessaires pour gagner
};

// Configuration actuelle (peut être personnalisée)
static GameConfig current_config;
static bool config_initialized = false;

// Initialiser la configuration si nécessaire
static void ensure_config_initialized(void)
{
    if (!config_initialized) {
        memcpy(&current_config, &default_config, sizeof(GameConfig));
        config_initialized = true;
    }
}

const GameConfig *game_config_get_default(void)
{
    return &default_config;
}

const GameConfig *game_config_get_current(void)
{
    ensure_config_initialized();
    return &current_config;
}

void game_config_set_custom(const GameConfig *config)
{
    assert(config != NULL);
    ensure_config_initialized();
    memcpy(&current_config, config, sizeof(GameConfig));
}

void game_config_reset_to_default(void)
{
    ensure_config_initialized();
    memcpy(&current_config, &default_config, sizeof(GameConfig));
}
