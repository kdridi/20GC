// Composant Paddle - caractéristiques des raquettes
// Structure exposée pour performance (pas d'allocation)

#pragma once

#include "entity.h"

// Structure Paddle : données spécifiques aux raquettes
typedef struct {
    float width;
    float height;
} Paddle;

// Ajouter composant Paddle à une entité
Paddle *paddle_add(Entity *entity, float width, float height);

// Récupérer le composant Paddle d'une entité (NULL si absent)
Paddle *paddle_get(Entity *entity);
