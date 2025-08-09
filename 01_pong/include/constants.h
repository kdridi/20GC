#pragma once

// Constantes globales du projet
// Inclus dans les PCH pour disponibilité partout

// Système d'entités
#define MAX_ENTITIES 1000

// Tests et comparaisons flottantes
#define FLOAT_EPSILON 1e-6f
#define DOUBLE_EPSILON 1e-12

// Macros pour comparaisons de flottants
#define FLOAT_EQ(a, b) (fabsf((a) - (b)) < FLOAT_EPSILON)
#define DOUBLE_EQ(a, b) (fabs((a) - (b)) < DOUBLE_EPSILON)

// Pong - constantes de jeu
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_HEIGHT 80
#define PADDLE_WIDTH 12
#define BALL_SIZE 8
