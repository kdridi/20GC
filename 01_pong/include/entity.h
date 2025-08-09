#pragma once

// API du système d'entités avec opaque pointer
// Implémentation cachée, interface stable

// Forward declaration - structure opaque
typedef struct Entity Entity;

// Fonctions publiques (une par fichier .c)
// Types cohérents : uint32_t partout
Entity *entity_create();
void entity_destroy(Entity *entity);
Entity *entity_get_by_id(uint32_t id);
uint32_t entity_count_active();

// Fonctions d'accès pour les tests
uint32_t entity_get_id(const Entity *entity);
bool entity_is_active(const Entity *entity);
