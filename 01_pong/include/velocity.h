#pragma once

// Composant Velocity pour ECS
// Vitesse de déplacement (delta par frame)

typedef struct {
    float dx; // Vitesse en x
    float dy; // Vitesse en y
} Velocity;

// Forward declaration pour Entity
typedef struct Entity Entity;

// API pour ECS
Velocity *velocity_add(Entity *entity, float dx, float dy);
Velocity *velocity_get(const Entity *entity); // NULL si pas de composant
