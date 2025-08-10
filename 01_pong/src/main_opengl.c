#include "ball.h"
#include "entity_factory.h"
#include "game_config.h"
#include "input_system.h"
#include "paddle.h"
#include "pch.h"
#include "position.h"
#include "score.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

// Vertex shader source - positions en NDC
const char *vertex_shader_source = "#version 330 core\n"
                                   "layout (location = 0) in vec2 aPos;\n"
                                   "uniform mat4 projection;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
                                   "}\0";

// Fragment shader source - couleur blanche uniforme
const char *fragment_shader_source = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "uniform vec3 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(color, 1.0f);\n"
                                     "}\n\0";

typedef struct {
    GLFWwindow *window;
    PongEntities game;
    bool running;

    // OpenGL objects
    GLuint shader_program;
    GLuint rectangle_VAO, rectangle_VBO;
    GLuint circle_VAO, circle_VBO;

    // Shader uniforms
    GLint projection_loc;
    GLint color_loc;

    // Matrices
    float projection_matrix[16];
} PongAppOpenGL;

// Fonction utilitaire pour créer une matrice de projection orthographique
static void create_ortho_matrix(float *matrix, float left, float right, float bottom, float top)
{
    // Initialiser à zéro
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0f;
    }

    // Matrice orthographique adaptée pour ECS (Y inversé)
    matrix[0] = 2.0f / (right - left);
    matrix[5] = -2.0f / (bottom - top); // Inverser Y pour correspondre à ECS
    matrix[10] = -1.0f;
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = (bottom + top) / (bottom - top); // Ajuster translation Y
    matrix[15] = 1.0f;
}

static GLuint compile_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Vérifier la compilation
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("Erreur compilation shader: %s\n", info_log);
        return 0;
    }

    return shader;
}

static GLuint create_shader_program(void)
{
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

    if (vertex_shader == 0 || fragment_shader == 0) {
        return 0;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Vérifier le linkage
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("Erreur linkage shader: %s\n", info_log);
        return 0;
    }

    // Nettoyer les shaders individuels
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

static void setup_rectangle_geometry(PongAppOpenGL *app)
{
    // Rectangle unit (sera transformé par les uniforms)
    float vertices[] = {
        -0.5f, -0.5f, // Bottom left
        0.5f, -0.5f,  // Bottom right
        0.5f, 0.5f,   // Top right
        -0.5f, 0.5f   // Top left
    };

    glGenVertexArrays(1, &app->rectangle_VAO);
    glGenBuffers(1, &app->rectangle_VBO);

    glBindVertexArray(app->rectangle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->rectangle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void setup_circle_geometry(PongAppOpenGL *app)
{
    // Générer un cercle avec triangles
    const int segments = 32;
    const int vertices_count = (1 + segments + 1) * 2; // Centre + périmètre + fermeture
    float *vertices = malloc(vertices_count * sizeof(float));

    // Centre du cercle
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;

    // Points du périmètre
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float) M_PI * (float) i / (float) segments;
        vertices[2 + i * 2] = cosf(angle) * 0.5f; // Rayon 0.5 (sera scalé)
        vertices[2 + i * 2 + 1] = sinf(angle) * 0.5f;
    }

    glGenVertexArrays(1, &app->circle_VAO);
    glGenBuffers(1, &app->circle_VBO);

    glBindVertexArray(app->circle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->circle_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void) scancode; // Unused
    (void) mods;     // Unused

    PongAppOpenGL *app = (PongAppOpenGL *) glfwGetWindowUserPointer(window);

    // Reset toutes les actions au début de chaque frame
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        bool pressed = (action == GLFW_PRESS);

        switch (key) {
        // Player 1
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
            input_system_set_action(PLAYER1_PADDLE_UP, pressed);
            break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            input_system_set_action(PLAYER1_PADDLE_DOWN, pressed);
            break;

        // Player 2
        case GLFW_KEY_I:
            input_system_set_action(PLAYER2_PADDLE_UP, pressed);
            break;
        case GLFW_KEY_K:
            input_system_set_action(PLAYER2_PADDLE_DOWN, pressed);
            break;

        // Quitter
        case GLFW_KEY_ESCAPE:
            if (pressed) {
                app->running = false;
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;

        default:
            break;
        }
    }
}

static bool init_opengl(PongAppOpenGL *app)
{
    const GameConfig *config = game_config_get_current();

    // Initialiser GLFW
    if (!glfwInit()) {
        printf("Erreur: Impossible d'initialiser GLFW\n");
        return false;
    }

    // Configuration OpenGL moderne
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS

    // Créer la fenêtre
    app->window = glfwCreateWindow((int) config->screen_width, (int) config->screen_height,
                                   "20GC Pong - OpenGL", NULL, NULL);
    if (!app->window) {
        printf("Erreur: Impossible de créer la fenêtre GLFW\n");
        glfwTerminate();
        return false;
    }

    // Contexte OpenGL
    glfwMakeContextCurrent(app->window);
    glfwSetWindowUserPointer(app->window, app);
    glfwSetKeyCallback(app->window, key_callback);

    // Initialiser GLEW
    if (glewInit() != GLEW_OK) {
        printf("Erreur: Impossible d'initialiser GLEW\n");
        return false;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    // Configuration OpenGL
    glViewport(0, 0, (int) config->screen_width, (int) config->screen_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fond noir

    // Créer le programme shader
    app->shader_program = create_shader_program();
    if (app->shader_program == 0) {
        return false;
    }

    // Récupérer les locations des uniforms
    app->projection_loc = glGetUniformLocation(app->shader_program, "projection");
    app->color_loc = glGetUniformLocation(app->shader_program, "color");

    // Créer la matrice de projection (coordonnées ECS: (0,0) = coin haut-gauche)
    printf("Debug: screen dimensions %ux%u\n", config->screen_width, config->screen_height);
    create_ortho_matrix(app->projection_matrix, 0.0f, (float) config->screen_width,
                        0.0f, (float) config->screen_height);

    // Debug: afficher la matrice
    printf("Debug: projection matrix:\n");
    for (int i = 0; i < 4; i++) {
        printf("[%f %f %f %f]\n",
               (double) app->projection_matrix[i * 4], (double) app->projection_matrix[i * 4 + 1],
               (double) app->projection_matrix[i * 4 + 2], (double) app->projection_matrix[i * 4 + 3]);
    }

    // Setup géométrie
    setup_rectangle_geometry(app);
    setup_circle_geometry(app);

    app->running = true;
    return true;
}

static void render_rectangle_opengl(PongAppOpenGL *app, float x, float y, float width, float height,
                                    float r, float g, float b)
{
    const GameConfig *config = game_config_get_current();

    glUseProgram(app->shader_program);

    // Matrice identité (pas de transformation)
    float identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    glUniformMatrix4fv(app->projection_loc, 1, GL_FALSE, identity);
    glUniform3f(app->color_loc, r, g, b);

    // Convertir directement en coordonnées NDC (-1 à +1)
    float ndc_x = (2.0f * x / (float) config->screen_width) - 1.0f;  // 0-800 -> -1 à +1
    float ndc_y = 1.0f - (2.0f * y / (float) config->screen_height); // 0-600 -> +1 à -1 (Y inversé)
    float ndc_w = width / (float) config->screen_width * 2.0f;       // Largeur en NDC
    float ndc_h = height / (float) config->screen_height * 2.0f;     // Hauteur en NDC

    float vertices[8];
    vertices[0] = ndc_x - ndc_w / 2; // x1
    vertices[1] = ndc_y - ndc_h / 2; // y1
    vertices[2] = ndc_x + ndc_w / 2; // x2
    vertices[3] = ndc_y - ndc_h / 2; // y2
    vertices[4] = ndc_x + ndc_w / 2; // x3
    vertices[5] = ndc_y + ndc_h / 2; // y3
    vertices[6] = ndc_x - ndc_w / 2; // x4
    vertices[7] = ndc_y + ndc_h / 2; // y4

    // Debug pour le premier appel
    static bool first_call = true;
    if (first_call) {
        printf("Debug: ECS (%.1f,%.1f) -> NDC (%.3f,%.3f) size (%.3f,%.3f)\n",
               (double) x, (double) y, (double) ndc_x, (double) ndc_y, (double) ndc_w, (double) ndc_h);
        first_call = false;
    }

    // Mettre à jour les données du buffer
    glBindVertexArray(app->rectangle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->rectangle_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

static void render_circle_opengl(PongAppOpenGL *app, float x, float y, float radius,
                                 float r, float g, float b)
{
    const GameConfig *config = game_config_get_current();

    glUseProgram(app->shader_program);

    // Matrice identité
    float identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    glUniformMatrix4fv(app->projection_loc, 1, GL_FALSE, identity);
    glUniform3f(app->color_loc, r, g, b);

    // Convertir en NDC
    float ndc_x = (2.0f * x / (float) config->screen_width) - 1.0f;
    float ndc_y = 1.0f - (2.0f * y / (float) config->screen_height);
    float ndc_radius = radius / (float) config->screen_width * 2.0f; // Utiliser screen_width pour garder les proportions

    // Générer le cercle en NDC
    const int segments = 32;
    const int vertices_count = (1 + segments + 1) * 2;
    float *vertices = malloc(vertices_count * sizeof(float));

    // Centre du cercle
    vertices[0] = ndc_x;
    vertices[1] = ndc_y;

    // Points du périmètre
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * (float) M_PI * (float) i / (float) segments;
        vertices[2 + i * 2] = ndc_x + cosf(angle) * ndc_radius;
        vertices[2 + i * 2 + 1] = ndc_y + sinf(angle) * ndc_radius;
    }

    // Mettre à jour le buffer
    glBindVertexArray(app->circle_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->circle_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_count * sizeof(float), vertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 34);
    glBindVertexArray(0);

    free(vertices);
}

static void render_field_opengl(PongAppOpenGL *app)
{
    const GameConfig *config = game_config_get_current();

    // Test simple : un gros rectangle au centre pour vérifier le rendu de base
    float test_x = (float) config->screen_width / 2.0f;  // 400
    float test_y = (float) config->screen_height / 2.0f; // 300
    printf("Debug: rendering test rectangle at (%.1f, %.1f) size 100x100\n",
           (double) test_x, (double) test_y);
    render_rectangle_opengl(app, test_x, test_y, 100.0f, 100.0f, 1.0f, 0.0f, 0.0f); // Rouge

    // Ligne centrale en pointillés
    float center_x = (float) config->screen_width / 2.0f;
    float dash_height = 10.0f;
    float dash_spacing = 20.0f;

    for (float y = 0; y < (float) config->screen_height; y += dash_spacing) {
        render_rectangle_opengl(app, center_x, y + dash_height / 2, 2.0f, dash_height, 1.0f, 1.0f, 1.0f);
    }
}

static void render_scores_opengl(PongAppOpenGL *app)
{
    // Note: Rendu de texte en OpenGL est complexe
    // Pour cette démo, on affiche juste des rectangles comme indicateurs de score
    const Score *p1_score = score_get(app->game.player1);
    const Score *p2_score = score_get(app->game.player2);
    const GameConfig *config = game_config_get_current();

    uint32_t score1 = p1_score ? p1_score->points : 0;
    uint32_t score2 = p2_score ? p2_score->points : 0;

    // Score P1 (rectangles à gauche)
    float p1_x = (float) config->screen_width / 4.0f;
    for (uint32_t i = 0; i < score1 && i < 10; i++) {
        render_rectangle_opengl(app, p1_x + i * 15.0f, 30.0f, 10.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    }

    // Score P2 (rectangles à droite)
    float p2_x = (float) config->screen_width * 3.0f / 4.0f;
    for (uint32_t i = 0; i < score2 && i < 10; i++) {
        render_rectangle_opengl(app, p2_x + i * 15.0f, 30.0f, 10.0f, 10.0f, 1.0f, 1.0f, 1.0f);
    }
}

static void render_pong_opengl(PongAppOpenGL *app)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Terrain
    render_field_opengl(app);

    // Paddles
    const Position *p1_pos = position_get(app->game.player1);
    const Paddle *p1_paddle = paddle_get(app->game.player1);
    if (p1_pos && p1_paddle) {
        render_rectangle_opengl(app, p1_pos->x, p1_pos->y, p1_paddle->width, p1_paddle->height,
                                1.0f, 1.0f, 1.0f);
    }

    const Position *p2_pos = position_get(app->game.player2);
    const Paddle *p2_paddle = paddle_get(app->game.player2);
    if (p2_pos && p2_paddle) {
        render_rectangle_opengl(app, p2_pos->x, p2_pos->y, p2_paddle->width, p2_paddle->height,
                                1.0f, 1.0f, 1.0f);
    }

    // Balle
    const Position *ball_pos = position_get(app->game.ball);
    const Ball *ball = ball_get(app->game.ball);
    if (ball_pos && ball) {
        render_circle_opengl(app, ball_pos->x, ball_pos->y, ball->radius, 1.0f, 1.0f, 1.0f);
    }

    // Score
    render_scores_opengl(app);

    glfwSwapBuffers(app->window);
}

static void cleanup_opengl(PongAppOpenGL *app)
{
    destroy_pong_game(&app->game);

    // Nettoyer OpenGL
    glDeleteVertexArrays(1, &app->rectangle_VAO);
    glDeleteBuffers(1, &app->rectangle_VBO);
    glDeleteVertexArrays(1, &app->circle_VAO);
    glDeleteBuffers(1, &app->circle_VBO);
    glDeleteProgram(app->shader_program);

    if (app->window) {
        glfwDestroyWindow(app->window);
    }
    glfwTerminate();
}

int main(void)
{
    printf("🔥 20GC Pong - OpenGL + Shaders\n");

    PongAppOpenGL app = {0};

    if (!init_opengl(&app)) {
        cleanup_opengl(&app);
        return 1;
    }

    // Créer le jeu (même logique ECS)
    app.game = create_pong_game();

    // Boucle principale
    double last_time = glfwGetTime();

    while (app.running && !glfwWindowShouldClose(app.window)) {
        // Input (géré par callback)
        glfwPollEvents();

        // Update avec delta time
        double current_time = glfwGetTime();
        float delta_time = (float) (current_time - last_time);
        last_time = current_time;

        // Même logique métier que les autres backends
        update_pong_game(delta_time);

        // Render
        render_pong_opengl(&app);
    }

    cleanup_opengl(&app);
    printf("\n🚀 Merci d'avoir joué à la version OpenGL!\n");
    return 0;
}
