// Composant Score - gestion des points des joueurs
// Structure simple pour tracker les scores

#pragma once

#include "entity.h"

// Structure du composant Score
typedef struct {
    uint32_t points;
} Score;

// Ajoute un composant Score à une entité
Score *score_add(Entity *entity, uint32_t initial_points);

// Récupère le composant Score d'une entité (ou NULL si absent)
Score *score_get(Entity *entity);
