#include "ball.h"
#include "entity_factory.h"
#include "game_config.h"
#include "input_system.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    PongEntities game;
    bool running;
} PongApp;

static bool init_sdl(PongApp *app)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return false;
    }

    const GameConfig *config = game_config_get_current();

    app->window = SDL_CreateWindow("20GC Pong",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   (int) config->screen_width, (int) config->screen_height,
                                   SDL_WINDOW_SHOWN);
    if (!app->window) {
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }

    app->renderer = SDL_CreateRenderer(app->window, -1,
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app->renderer) {
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

static void handle_input(PongApp *app, SDL_Event *event)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    // Reset toutes les actions
    input_system_set_action(PLAYER1_PADDLE_UP, false);
    input_system_set_action(PLAYER1_PADDLE_DOWN, false);
    input_system_set_action(PLAYER2_PADDLE_UP, false);
    input_system_set_action(PLAYER2_PADDLE_DOWN, false);

    // Player 1 : W/S ou Fléches
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        input_system_set_action(PLAYER1_PADDLE_UP, true);
    }
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        input_system_set_action(PLAYER1_PADDLE_DOWN, true);
    }

    // Player 2 : I/K
    if (keys[SDL_SCANCODE_I]) {
        input_system_set_action(PLAYER2_PADDLE_UP, true);
    }
    if (keys[SDL_SCANCODE_K]) {
        input_system_set_action(PLAYER2_PADDLE_DOWN, true);
    }

    // Quitter
    if (event->type == SDL_QUIT || keys[SDL_SCANCODE_ESCAPE]) {
        app->running = false;
    }
}

static void render_paddle(SDL_Renderer *renderer, const Position *pos, const Paddle *paddle)
{
    if (!pos || !paddle)
        return;

    SDL_Rect rect = {
        .x = (int) (pos->x - paddle->width / 2.0f),
        .y = (int) (pos->y - paddle->height / 2.0f),
        .w = (int) paddle->width,
        .h = (int) paddle->height};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    SDL_RenderFillRect(renderer, &rect);
}

static void render_ball(SDL_Renderer *renderer, const Position *pos, const Ball *ball)
{
    if (!pos || !ball)
        return;

    SDL_Rect rect = {
        .x = (int) (pos->x - ball->radius),
        .y = (int) (pos->y - ball->radius),
        .w = (int) (ball->radius * 2.0f),
        .h = (int) (ball->radius * 2.0f)};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    SDL_RenderFillRect(renderer, &rect);
}

static void render(PongApp *app)
{
    // Fond noir
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    // Render paddles
    const Position *p1_pos = position_get(app->game.player1);
    const Paddle *p1_paddle = paddle_get(app->game.player1);
    render_paddle(app->renderer, p1_pos, p1_paddle);

    const Position *p2_pos = position_get(app->game.player2);
    const Paddle *p2_paddle = paddle_get(app->game.player2);
    render_paddle(app->renderer, p2_pos, p2_paddle);

    // Render ball
    const Position *ball_pos = position_get(app->game.ball);
    const Ball *ball = ball_get(app->game.ball);
    render_ball(app->renderer, ball_pos, ball);

    // Ligne centrale
    const GameConfig *config = game_config_get_current();
    SDL_SetRenderDrawColor(app->renderer, 128, 128, 128, 255); // Gris
    int center_x = (int) (config->screen_width / 2);
    SDL_RenderDrawLine(app->renderer, center_x, 0, center_x, (int) config->screen_height);

    SDL_RenderPresent(app->renderer);
}

static void print_scores(PongApp *app)
{
    const Score *p1_score = score_get(app->game.player1);
    const Score *p2_score = score_get(app->game.player2);

    uint32_t score1 = p1_score ? p1_score->points : 0;
    uint32_t score2 = p2_score ? p2_score->points : 0;

    static uint32_t last_score1 = 999, last_score2 = 999;
    if (score1 != last_score1 || score2 != last_score2) {
        printf("⚽ Score: %u - %u\n", score1, score2);
        last_score1 = score1;
        last_score2 = score2;
    }
}

static void cleanup(PongApp *app)
{
    destroy_pong_game(&app->game);
    if (app->renderer)
        SDL_DestroyRenderer(app->renderer);
    if (app->window)
        SDL_DestroyWindow(app->window);
    SDL_Quit();
}

int main(void)
{
    printf("🏓 20GC Pong - SDL2\n");
    printf("Contrôles: P1(W/S ou Flèches) P2(I/K) ESC(Quitter)\n\n");

    PongApp app = {0};

    if (!init_sdl(&app)) {
        cleanup(&app);
        return 1;
    }

    // Créer le jeu
    app.game = create_pong_game();
    app.running = true;

    // Boucle principale
    SDL_Event event;
    Uint32 last_time = SDL_GetTicks();

    while (app.running) {
        // Input
        while (SDL_PollEvent(&event)) {
            handle_input(&app, &event);
        }

        // Update (60 FPS fixe)
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        update_pong_game(delta_time);

        // Afficher scores si changement
        print_scores(&app);

        // Render
        render(&app);

        // 60 FPS
        SDL_Delay(16);
    }

    cleanup(&app);
    printf("\n🎮 Merci d'avoir joué!\n");
    return 0;
}
