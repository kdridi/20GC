#include "ball.h"
#include "entity_factory.h"
#include "game_config.h"
#include "input_system.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"
#include <ncurses.h>

typedef struct {
    WINDOW *window;
    PongEntities game;
    bool running;
    int screen_width;
    int screen_height;
} PongAppNCurses;

static bool init_ncurses(PongAppNCurses *app)
{
    // Initialiser ncurses
    app->window = initscr();
    if (!app->window) {
        printf("Erreur initscr()\n");
        return false;
    }

    // Configuration ncurses
    cbreak();              // Pas de buffer ligne
    noecho();              // Pas d'écho des touches
    nodelay(stdscr, TRUE); // Non-bloquant pour getch()
    keypad(stdscr, TRUE);  // Touches spéciales (flèches)
    curs_set(0);           // Cacher le curseur

    // Dimensions de l'écran
    getmaxyx(stdscr, app->screen_height, app->screen_width);

    // Vérifier taille minimum
    if (app->screen_width < 60 || app->screen_height < 20) {
        endwin();
        printf("Erreur: Terminal trop petit (min 60x20)\n");
        return false;
    }

    return true;
}

static void render_border(PongAppNCurses *app)
{
    // Bordure supérieure et inférieure
    for (int x = 0; x < app->screen_width; x++) {
        mvaddch(0, x, '-');
        mvaddch(app->screen_height - 1, x, '-');
    }

    // Bordures gauche et droite
    for (int y = 0; y < app->screen_height; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, app->screen_width - 1, '|');
    }

    // Coins
    mvaddch(0, 0, '+');
    mvaddch(0, app->screen_width - 1, '+');
    mvaddch(app->screen_height - 1, 0, '+');
    mvaddch(app->screen_height - 1, app->screen_width - 1, '+');

    // Ligne centrale
    int center_x = app->screen_width / 2;
    for (int y = 1; y < app->screen_height - 1; y++) {
        mvaddch(y, center_x, ':');
    }
}

static void render_paddle_ncurses(PongAppNCurses *app, const Position *pos, const Paddle *paddle)
{
    if (!pos || !paddle)
        return;

    const GameConfig *config = game_config_get_current();

    // Convertir coordonnées ECS vers ncurses
    // ECS: position = centre, écran 800x600
    // ncurses: position = coin, écran terminal
    float scale_x = (float) (app->screen_width - 2) / (float) config->screen_width;
    float scale_y = (float) (app->screen_height - 2) / (float) config->screen_height;

    int paddle_x = (int) (pos->x * scale_x) + 1; // +1 pour bordure
    int paddle_center_y = (int) (pos->y * scale_y) + 1;
    int paddle_half_height = (int) (paddle->height * scale_y / 2.0f);

    // Dessiner le paddle verticalement
    for (int dy = -paddle_half_height; dy <= paddle_half_height; dy++) {
        int y = paddle_center_y + dy;
        if (y > 0 && y < app->screen_height - 1) {
            mvaddch(y, paddle_x, '#');
        }
    }
}

static void render_ball_ncurses(PongAppNCurses *app, const Position *pos, const Ball *ball)
{
    if (!pos || !ball)
        return;

    const GameConfig *config = game_config_get_current();

    // Convertir coordonnées ECS vers ncurses
    float scale_x = (float) (app->screen_width - 2) / (float) config->screen_width;
    float scale_y = (float) (app->screen_height - 2) / (float) config->screen_height;

    int ball_x = (int) (pos->x * scale_x) + 1;
    int ball_y = (int) (pos->y * scale_y) + 1;

    if (ball_x > 0 && ball_x < app->screen_width - 1 &&
        ball_y > 0 && ball_y < app->screen_height - 1) {
        mvaddch(ball_y, ball_x, 'O');
    }
}

static void render_scores(PongAppNCurses *app)
{
    const Score *p1_score = score_get(app->game.player1);
    const Score *p2_score = score_get(app->game.player2);

    uint32_t score1 = p1_score ? p1_score->points : 0;
    uint32_t score2 = p2_score ? p2_score->points : 0;

    // Afficher le score en haut au centre
    int center_x = app->screen_width / 2;
    mvprintw(1, center_x - 5, "Score: %u - %u", score1, score2);
}

static void render_controls(PongAppNCurses *app)
{
    // Afficher les contrôles en bas
    mvprintw(app->screen_height - 2, 2, "P1: W/S | P2: I/K | Q: Quit");
}

static void render_pong_ncurses(PongAppNCurses *app)
{
    // Effacer l'écran
    clear();

    // Dessiner la bordure et ligne centrale
    render_border(app);

    // Dessiner les paddles
    const Position *p1_pos = position_get(app->game.player1);
    const Paddle *p1_paddle = paddle_get(app->game.player1);
    render_paddle_ncurses(app, p1_pos, p1_paddle);

    const Position *p2_pos = position_get(app->game.player2);
    const Paddle *p2_paddle = paddle_get(app->game.player2);
    render_paddle_ncurses(app, p2_pos, p2_paddle);

    // Dessiner la balle
    const Position *ball_pos = position_get(app->game.ball);
    const Ball *ball = ball_get(app->game.ball);
    render_ball_ncurses(app, ball_pos, ball);

    // Dessiner le score et contrôles
    render_scores(app);
    render_controls(app);

    // Titre
    mvprintw(0, app->screen_width / 2 - 6, " 20GC Pong ");

    // Actualiser l'affichage
    refresh();
}

static void handle_input_ncurses(PongAppNCurses *app)
{
    int ch = getch();

    // Reset toutes les actions
    input_system_set_action(PLAYER1_PADDLE_UP, false);
    input_system_set_action(PLAYER1_PADDLE_DOWN, false);
    input_system_set_action(PLAYER2_PADDLE_UP, false);
    input_system_set_action(PLAYER2_PADDLE_DOWN, false);

    // Traiter la touche pressée
    switch (ch) {
    // Player 1
    case 'w':
    case 'W':
    case KEY_UP:
        input_system_set_action(PLAYER1_PADDLE_UP, true);
        break;
    case 's':
    case 'S':
    case KEY_DOWN:
        input_system_set_action(PLAYER1_PADDLE_DOWN, true);
        break;

    // Player 2
    case 'i':
    case 'I':
        input_system_set_action(PLAYER2_PADDLE_UP, true);
        break;
    case 'k':
    case 'K':
        input_system_set_action(PLAYER2_PADDLE_DOWN, true);
        break;

    // Quitter
    case 'q':
    case 'Q':
    case 27: // ESC
        app->running = false;
        break;

    default:
        // Aucune action pour les autres touches
        break;
    }
}

static void cleanup_ncurses(PongAppNCurses *app)
{
    destroy_pong_game(&app->game);
    if (app->window) {
        endwin();
    }
}

static void print_scores_console(PongAppNCurses *app)
{
    const Score *p1_score = score_get(app->game.player1);
    const Score *p2_score = score_get(app->game.player2);

    uint32_t score1 = p1_score ? p1_score->points : 0;
    uint32_t score2 = p2_score ? p2_score->points : 0;

    static uint32_t last_score1 = 999, last_score2 = 999;
    if (score1 != last_score1 || score2 != last_score2) {
        // Écrire dans un fichier de log pour éviter de polluer ncurses
        FILE *log = fopen("/tmp/pong_scores.log", "a");
        if (log) {
            fprintf(log, "⚽ Score: %u - %u\n", score1, score2);
            fclose(log);
        }
        last_score1 = score1;
        last_score2 = score2;
    }
}

int main(void)
{
    printf("🏓 20GC Pong - ncurses\n");
    printf("Redimensionnez votre terminal à au moins 60x20\n");
    printf("Appuyez sur Entrée pour commencer...\n");
    getchar();

    PongAppNCurses app = {0};

    if (!init_ncurses(&app)) {
        cleanup_ncurses(&app);
        return 1;
    }

    // Créer le jeu (même logique ECS que SDL2)
    app.game = create_pong_game();
    app.running = true;

    // Boucle principale (identique à SDL2)
    clock_t last_time = clock();

    while (app.running) {
        // Input
        handle_input_ncurses(&app);

        // Update avec delta time
        clock_t current_time = clock();
        float delta_time = (float) (current_time - last_time) / CLOCKS_PER_SEC;
        last_time = current_time;

        // Même logique métier que SDL2
        update_pong_game(delta_time);

        // Log des scores
        print_scores_console(&app);

        // Render
        render_pong_ncurses(&app);

        // ~60 FPS - utiliser nanosleep pour compatibilité
        struct timespec ts = {0, 16000000}; // 16ms en nanosecondes
        nanosleep(&ts, NULL);
    }

    cleanup_ncurses(&app);
    printf("\n🎮 Merci d'avoir joué à la version ncurses!\n");
    printf("Consultez /tmp/pong_scores.log pour l'historique des scores\n");
    return 0;
}
