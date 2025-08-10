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

// Shader ultra-simple - pas de transformation, coordonnées directes en [-1,1]
const char *vertex_shader_source = "#version 330 core\n"
                                   "layout (location = 0) in vec2 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
                                   "}\0";

const char *fragment_shader_source = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                     "}\n\0";

typedef struct {
    GLFWwindow *window;
    PongEntities game;
    bool running;
    GLuint shader_program;
    GLuint VAO, VBO;
} PongAppOpenGL;

static GLuint compile_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

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

    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("Erreur linkage shader: %s\n", info_log);
        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void) window;
    printf("Debug: Framebuffer resized to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) mods;

    PongAppOpenGL *app = (PongAppOpenGL *) glfwGetWindowUserPointer(window);

    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        bool pressed = (action == GLFW_PRESS);

        switch (key) {
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
            input_system_set_action(PLAYER1_PADDLE_UP, pressed);
            break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            input_system_set_action(PLAYER1_PADDLE_DOWN, pressed);
            break;
        case GLFW_KEY_I:
            input_system_set_action(PLAYER2_PADDLE_UP, pressed);
            break;
        case GLFW_KEY_K:
            input_system_set_action(PLAYER2_PADDLE_DOWN, pressed);
            break;
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

    if (!glfwInit()) {
        printf("Erreur: Impossible d'initialiser GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    app->window = glfwCreateWindow((int) config->screen_width, (int) config->screen_height,
                                   "20GC Pong - OpenGL Simple", NULL, NULL);
    if (!app->window) {
        printf("Erreur: Impossible de créer la fenêtre GLFW\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(app->window);
    glfwSetWindowUserPointer(app->window, app);
    glfwSetKeyCallback(app->window, key_callback);
    glfwSetFramebufferSizeCallback(app->window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        printf("Erreur: Impossible d'initialiser GLEW\n");
        return false;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    // DIAGNOSTIC: Vérifier les dimensions réelles de la fenêtre
    int framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(app->window, &framebuffer_width, &framebuffer_height);
    printf("Debug: Window size: %dx%d, Framebuffer: %dx%d\n",
           (int) config->screen_width, (int) config->screen_height,
           framebuffer_width, framebuffer_height);

    // SOLUTION 1: Utiliser les dimensions réelles du framebuffer
    glViewport(0, 0, framebuffer_width, framebuffer_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    app->shader_program = create_shader_program();
    if (app->shader_program == 0) {
        return false;
    }

    // Setup géométrie simple
    glGenVertexArrays(1, &app->VAO);
    glGenBuffers(1, &app->VBO);

    glBindVertexArray(app->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), NULL, GL_DYNAMIC_DRAW); // 4 points * 2 coords

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    app->running = true;
    return true;
}

// Fonction ultra-simple pour dessiner un rectangle en NDC
static void draw_rectangle(PongAppOpenGL *app, float center_x, float center_y, float width, float height)
{
    const GameConfig *config = game_config_get_current();
    (void) config; // Éviter warning unused

    // Conversion ECS -> NDC fonctionnelle pour écran Retina
    float ndc_center_x = (center_x / (float) config->screen_width) * 2.0f - 1.0f;  // 0-800 -> -1 à +1
    float ndc_center_y = 1.0f - (center_y / (float) config->screen_height) * 2.0f; // 0-600 -> +1 à -1
    float ndc_half_w = (width / (float) config->screen_width);                     // demi-largeur en NDC
    float ndc_half_h = (height / (float) config->screen_height);                   // demi-hauteur en NDC

    float vertices[8] = {
        ndc_center_x - ndc_half_w, ndc_center_y - ndc_half_h, // bottom-left
        ndc_center_x + ndc_half_w, ndc_center_y - ndc_half_h, // bottom-right
        ndc_center_x + ndc_half_w, ndc_center_y + ndc_half_h, // top-right
        ndc_center_x - ndc_half_w, ndc_center_y + ndc_half_h  // top-left
    };

    glUseProgram(app->shader_program);
    glBindVertexArray(app->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, app->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

static void render_pong_opengl(PongAppOpenGL *app)
{
    const GameConfig *config = game_config_get_current();

    glClear(GL_COLOR_BUFFER_BIT);

    // Ligne centrale en pointillés
    float center_x = (float) config->screen_width / 2.0f;
    for (float y = 0; y < (float) config->screen_height; y += 20.0f) {
        draw_rectangle(app, center_x, y + 5.0f, 2.0f, 10.0f);
    }

    // Paddles
    const Position *p1_pos = position_get(app->game.player1);
    const Paddle *p1_paddle = paddle_get(app->game.player1);
    if (p1_pos && p1_paddle) {
        draw_rectangle(app, p1_pos->x, p1_pos->y, p1_paddle->width, p1_paddle->height);
    }

    const Position *p2_pos = position_get(app->game.player2);
    const Paddle *p2_paddle = paddle_get(app->game.player2);
    if (p2_pos && p2_paddle) {
        draw_rectangle(app, p2_pos->x, p2_pos->y, p2_paddle->width, p2_paddle->height);
    }

    // Balle (rendue comme un petit carré)
    const Position *ball_pos = position_get(app->game.ball);
    const Ball *ball = ball_get(app->game.ball);
    if (ball_pos && ball) {
        float ball_size = ball->radius * 2.0f;
        draw_rectangle(app, ball_pos->x, ball_pos->y, ball_size, ball_size);
    }

    glfwSwapBuffers(app->window);
}

static void cleanup_opengl(PongAppOpenGL *app)
{
    destroy_pong_game(&app->game);

    glDeleteVertexArrays(1, &app->VAO);
    glDeleteBuffers(1, &app->VBO);
    glDeleteProgram(app->shader_program);

    if (app->window) {
        glfwDestroyWindow(app->window);
    }
    glfwTerminate();
}

int main(void)
{
    printf("🔥 20GC Pong - OpenGL Simple\n");

    PongAppOpenGL app = {0};

    if (!init_opengl(&app)) {
        cleanup_opengl(&app);
        return 1;
    }

    app.game = create_pong_game();

    double last_time = glfwGetTime();

    while (app.running && !glfwWindowShouldClose(app.window)) {
        glfwPollEvents();

        double current_time = glfwGetTime();
        float delta_time = (float) (current_time - last_time);
        last_time = current_time;

        update_pong_game(delta_time);

        render_pong_opengl(&app);
    }

    cleanup_opengl(&app);
    printf("\n🚀 OpenGL Simple fini!\n");
    return 0;
}
