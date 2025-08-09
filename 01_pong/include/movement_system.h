#pragma once

// Système Movement pour ECS
// Traite les entités ayant Position + Velocity

// Update du système - applique les vitesses aux positions
// delta_time : temps écoulé depuis la dernière frame
void movement_system_update(float delta_time);
