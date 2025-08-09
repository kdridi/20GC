# 20GC

Implémentation pure en C du 20 Games Challenge utilisant une approche de Développement Dirigé par les Tests (TDD) et une architecture Entity-Component-System (ECS), avec SDL2 pour le rendu et la gestion des entrées.

## À propos du 20 Games Challenge

Le 20 Games Challenge est un défi d'apprentissage du développement de jeux vidéo basé sur la recréation de jeux classiques. L'objectif est de développer ses compétences progressivement, sans contrainte de temps stricte, en implémentant des jeux de complexité croissante.

## Liste des jeux

1. **Pong** - Jeu de tennis de table classique
2. **Flappy Bird** - Jeu de vol à travers des obstacles
3. **Breakout** - Casse-briques avec balle et raquette
4. **Jetpack Joyride** - Course à progression infinie
5. **Space Invaders** - Défense contre invasion alien
6. **Frogger** - Traversée d'obstacles en mouvement
7. **River Raid** - Jeu de tir en survol de rivière
8. **Asteroids** - Destruction d'astéroïdes dans l'espace
9. **Spacewar!** - Combat spatial entre deux vaisseaux
10. **Indy 500** - Course automobile sur circuit
11. **Lunar Lander** - Atterrissage lunaire en douceur
12. **Pac-Man** - Collecte de pastilles dans un labyrinthe
13. **Tic Tac Toe** - Morpion classique
14. **Conway's Game of Life** - Automate cellulaire
15. **Super Mario Bros.** - Plateforme à défilement horizontal
16. **Pitfall** - Aventure dans la jungle
17. **VVVVVV** - Plateforme avec inversion de gravité
18. **Worms** - Combat au tour par tour avec trajectoires
19. **Dig Dug** - Creusement souterrain et élimination d'ennemis
20. **Motherload** - Forage et collecte de ressources

*Note : Cette liste peut être adaptée selon les objectifs d'apprentissage spécifiques.*

## Progression du projet

### Phase actuelle : Pong (01_pong)

#### Architecture ECS implémentée ✅
- **Entités** : Pool statique avec IDs uniques, gestion transparente
- **Composants** : Position et Velocity avec Structure of Arrays (SoA) pour performance
- **Systèmes** : Movement system combinant Position + Velocity
- **Tests TDD** : Couverture 94.2% avec tests atomiques (un fichier = un test)

#### Infrastructure de build ✅
- **Makefile** : Compilation avec warnings stricts, sanitizers, coverage
- **Dépendances** : SDL2 uniquement (GTest/GMock retirés pour simplicité)
- **Tests** : Approche simple avec `assert()` et `printf()` 
- **Formatage** : clang-format + git hooks automatiques
- **PCH** : Headers précompilés pour accélération du build

#### Prochain étapes
- [ ] Composants spécifiques Pong (Ball, Paddle)
- [ ] Systèmes collision et rendu
- [ ] Intégration SDL2 complète

### Jeux à venir
- [x] **01_pong** - Architecture ECS de base implementée
- [ ] **02_flappy_bird** - Planifié après extraction patterns
- [ ] **03_breakout** - Planifié
- [ ] *(autres jeux selon la progression)*
