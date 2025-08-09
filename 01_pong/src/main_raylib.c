#include "ball.h"
#include "entity_factory.h"
#include "game_config.h"
#include "input_system.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"
#include <raylib.h>

typedef struct {
    PongEntities game;
    bool running;
} PongAppRaylib;

static bool init_raylib(PongAppRaylib *app)
{
    const GameConfig *config = game_config_get_current();

    // Initialiser Raylib
    InitWindow((int) config->screen_width, (int) config->screen_height, "20GC Pong - Raylib");
    SetTargetFPS(60);

    // Vérifier si l'initialisation a réussi
    if (!IsWindowReady()) {
        printf("Erreur: Impossible d'initialiser la fenêtre Raylib\n");
        return false;
    }

    app->running = true;
    return true;
}

static void render_paddle_raylib(const Position *pos, const Paddle *paddle)
{
    if (!pos || !paddle)
        return;

    // Raylib utilise le coin supérieur gauche comme origine
    // Position ECS = centre, donc convertir
    float paddle_x = pos->x - paddle->width / 2.0f;
    float paddle_y = pos->y - paddle->height / 2.0f;

    DrawRectangle((int) paddle_x, (int) paddle_y,
                  (int) paddle->width, (int) paddle->height, WHITE);
}

static void render_ball_raylib(const Position *pos, const Ball *ball)
{
    if (!pos || !ball)
        return;

    // Raylib DrawCircle utilise le centre
    DrawCircle((int) pos->x, (int) pos->y, ball->radius, WHITE);
}

static void render_field_raylib(void)
{
    const GameConfig *config = game_config_get_current();

    // Ligne centrale en pointillés
    int center_x = (int) config->screen_width / 2;
    for (int y = 0; y < (int) config->screen_height; y += 20) {
        DrawRectangle(center_x - 1, y, 2, 10, WHITE);
    }
}

static void render_scores_raylib(PongAppRaylib *app)
{
    const Score *p1_score = score_get(app->game.player1);
    const Score *p2_score = score_get(app->game.player2);
    const GameConfig *config = game_config_get_current();

    uint32_t score1 = p1_score ? p1_score->points : 0;
    uint32_t score2 = p2_score ? p2_score->points : 0;

    // Scores en haut à gauche et droite
    DrawText(TextFormat("%d", score1), (int) config->screen_width / 4, 50, 48, WHITE);
    DrawText(TextFormat("%d", score2), (int) config->screen_width * 3 / 4, 50, 48, WHITE);
}

static void render_ui_raylib(void)
{
    const GameConfig *config = game_config_get_current();

    // Titre
    const char *title = "20GC PONG - RAYLIB";
    int title_width = MeasureText(title, 20);
    DrawText(title, ((int) config->screen_width - title_width) / 2, 10, 20, WHITE);

    // Contrôles
    const char *controls = "P1: W/S | P2: I/K | ESC: Quit";
    int controls_width = MeasureText(controls, 16);
    DrawText(controls, ((int) config->screen_width - controls_width) / 2,
             (int) config->screen_height - 30, 16, WHITE);
}

static void render_pong_raylib(PongAppRaylib *app)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Terrain
    render_field_raylib();

    // Entités
    const Position *p1_pos = position_get(app->game.player1);
    const Paddle *p1_paddle = paddle_get(app->game.player1);
    render_paddle_raylib(p1_pos, p1_paddle);

    const Position *p2_pos = position_get(app->game.player2);
    const Paddle *p2_paddle = paddle_get(app->game.player2);
    render_paddle_raylib(p2_pos, p2_paddle);

    const Position *ball_pos = position_get(app->game.ball);
    const Ball *ball = ball_get(app->game.ball);
    render_ball_raylib(ball_pos, ball);

    // Interface
    render_scores_raylib(app);
    render_ui_raylib();

    EndDrawing();
}

static void handle_input_raylib(PongAppRaylib *app)
{
    // Reset toutes les actions
    input_system_set_action(PLAYER1_PADDLE_UP, false);
    input_system_set_action(PLAYER1_PADDLE_DOWN, false);
    input_system_set_action(PLAYER2_PADDLE_UP, false);
    input_system_set_action(PLAYER2_PADDLE_DOWN, false);

    // Player 1
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        input_system_set_action(PLAYER1_PADDLE_UP, true);
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        input_system_set_action(PLAYER1_PADDLE_DOWN, true);
    }

    // Player 2
    if (IsKeyDown(KEY_I)) {
        input_system_set_action(PLAYER2_PADDLE_UP, true);
    }
    if (IsKeyDown(KEY_K)) {
        input_system_set_action(PLAYER2_PADDLE_DOWN, true);
    }

    // Quitter
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
        app->running = false;
    }
}

static void cleanup_raylib(PongAppRaylib *app)
{
    destroy_pong_game(&app->game);
    CloseWindow();
}

int main(void)
{
    printf("🎮 20GC Pong - Raylib\n");

    PongAppRaylib app = {0};

    if (!init_raylib(&app)) {
        cleanup_raylib(&app);
        return 1;
    }

    // Créer le jeu (même logique ECS que SDL2/ncurses)
    app.game = create_pong_game();

    // Boucle principale (identique aux autres backends)
    float last_time = (float) GetTime();

    while (app.running) {
        // Input
        handle_input_raylib(&app);

        // Update avec delta time
        float current_time = (float) GetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        // Même logique métier que SDL2/ncurses
        update_pong_game(delta_time);

        // Render
        render_pong_raylib(&app);
    }

    cleanup_raylib(&app);
    printf("\n🎨 Merci d'avoir joué à la version Raylib!\n");
    return 0;
}
