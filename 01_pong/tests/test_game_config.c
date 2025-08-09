#include "game_config.h"
#include "pch.h"

static void test_game_config_default_values(void)
{
    printf("test_game_config_default_values... ");

    const GameConfig *const config = game_config_get_default();
    assert(config != NULL);

    // Vérifier les dimensions de l'écran
    assert(config->screen_width == 800);
    assert(config->screen_height == 600);

    // Vérifier les dimensions des paddles
    assert(FLOAT_EQ(config->paddle_width, 12.0f));
    assert(FLOAT_EQ(config->paddle_height, 80.0f));
    assert(FLOAT_EQ(config->paddle_speed, 300.0f));

    // Vérifier les positions initiales des paddles
    assert(FLOAT_EQ(config->player1_x, 50.0f));
    assert(FLOAT_EQ(config->player2_x, 750.0f));

    // Vérifier les propriétés de la balle
    assert(FLOAT_EQ(config->ball_radius, 4.0f));
    assert(FLOAT_EQ(config->ball_speed_x, 150.0f));
    assert(FLOAT_EQ(config->ball_speed_y, 100.0f));

    // Vérifier les paramètres de jeu
    assert(config->max_score == 11);
    assert(config->points_to_win == 11);

    printf("OK\n");
}

static void test_game_config_singleton(void)
{
    printf("test_game_config_singleton... ");

    const GameConfig *const config1 = game_config_get_default();
    const GameConfig *const config2 = game_config_get_default();

    // Les deux appels doivent retourner la même instance
    assert(config1 == config2);

    printf("OK\n");
}

static void test_game_config_custom(void)
{
    printf("test_game_config_custom... ");

    GameConfig custom = {
        .screen_width = 1024,
        .screen_height = 768,
        .paddle_width = 15.0f,
        .paddle_height = 100.0f,
        .paddle_speed = 400.0f,
        .player1_x = 60.0f,
        .player2_x = 964.0f,
        .ball_radius = 5.0f,
        .ball_speed_x = 200.0f,
        .ball_speed_y = 150.0f,
        .max_score = 21,
        .points_to_win = 21};

    game_config_set_custom(&custom);
    const GameConfig *const config = game_config_get_current();

    assert(config->screen_width == 1024);
    assert(config->screen_height == 768);
    assert(FLOAT_EQ(config->paddle_width, 15.0f));
    assert(config->max_score == 21);

    // Restaurer la configuration par défaut
    game_config_reset_to_default();
    const GameConfig *const default_config = game_config_get_current();
    assert(default_config->screen_width == 800);

    printf("OK\n");
}

int main(void)
{
    test_game_config_default_values();
    test_game_config_singleton();
    test_game_config_custom();
    return 0;
}
