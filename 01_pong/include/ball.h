// Composant Ball - caractéristiques des balles
// Structure exposée pour performance (pas d'allocation)

#pragma once

#include "entity.h"

// Structure Ball : données spécifiques aux balles
typedef struct {
    float radius;
} Ball;

// Ajouter composant Ball à une entité
Ball *ball_add(Entity *entity, float radius);

// Récupérer le composant Ball d'une entité (NULL si absent)
Ball *ball_get(Entity *entity);
