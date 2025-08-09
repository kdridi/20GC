#pragma once

// Composant Position pour ECS
// Structure de données publique - accès direct

typedef struct {
    float x;
    float y;
} Position;

// Forward declaration pour Entity
typedef struct Entity Entity;

// API pour ECS
Position *position_add(Entity *entity, float x, float y);
Position *position_get(const Entity *entity); // NULL si pas de composant
