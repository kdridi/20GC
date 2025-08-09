#pragma once

// Structure contenant toute la configuration du jeu
typedef struct GameConfig {
    // Dimensions de l'écran
    uint32_t screen_width;
    uint32_t screen_height;

    // Configuration des paddles
    float paddle_width;
    float paddle_height;
    float paddle_speed; // Vitesse de déplacement des paddles (pixels/seconde)

    // Positions initiales des joueurs
    float player1_x; // Position X du joueur 1 (gauche)
    float player2_x; // Position X du joueur 2 (droite)

    // Configuration de la balle
    float ball_radius;
    float ball_speed_x; // Vitesse horizontale initiale
    float ball_speed_y; // Vitesse verticale initiale

    // Paramètres de jeu
    uint32_t max_score;     // Score maximum pour gagner
    uint32_t points_to_win; // Points nécessaires pour gagner
} GameConfig;

// Obtenir la configuration par défaut (singleton)
const GameConfig *game_config_get_default(void);

// Obtenir la configuration actuelle
const GameConfig *game_config_get_current(void);

// Définir une configuration personnalisée
void game_config_set_custom(const GameConfig *config);

// Restaurer la configuration par défaut
void game_config_reset_to_default(void);
