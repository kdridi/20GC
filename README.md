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

### Phase complétée : Pong (01_pong) ✅

#### Architecture ECS complète ✅
- **Entités** : Pool statique avec IDs uniques, gestion transparente
- **Composants** : Position, Velocity, Ball, Paddle, Score, GameState avec Structure of Arrays (SoA)
- **Systèmes** : Movement, Bounds, Collision, Input, Scoring, Reset - tous fonctionnels et optimisés
- **Entity Factory** : Helpers pour création d'entités complètes (create_player, create_pong_game)
- **Game Config** : Système de configuration centralisé éliminant tous les magic constants ✅
- **Tests TDD** : 19 tests atomiques passants, approche simple sans frameworks

#### Infrastructure de build ✅
- **Makefile** : Compilation avec warnings stricts, sanitizers, coverage
- **Dépendances** : SDL2 uniquement, intégration PCH optimisée pour warnings
- **Tests** : Approche TDD pure avec `assert()` et `printf()`, exclusion main.o pour éviter conflits
- **Formatage** : clang-format + git hooks automatiques
- **PCH** : Headers précompilés avec SDL2 intégré et pragmas pour warnings stricts

#### SDL2 et Gameplay ✅
- **Rendu** : Interface graphique complète avec SDL2, rectangles blancs pour paddles/balle
- **Contrôles** : Player 1 (W/S ou flèches), Player 2 (I/K), ESC pour quitter
- **Physique** : Collision précise balle-paddle, rebond vertical (haut/bas), traversée horizontale pour points
- **Scoring** : Détection et affichage des scores en temps réel dans la console
- **Performance** : Boucle de jeu 60 FPS avec delta time, paddles réactifs (600px/s)
- **Système de coordonnées** : Cohérence totale entre rendu SDL2 et collision ECS (position = centre)

#### Systèmes de jeu optimisés ✅
- **Movement System** : Applique velocity à position avec delta_time précis
- **Bounds System** : Maintient les paddles dans les limites d'écran uniquement
- **Collision System** : Rebond vertical (haut/bas), collision balle-paddle précise, pas de rebond horizontal
- **Input System** : Contrôles des paddles avec actions métier et reset des états
- **Scoring System** : Détection points par sortie gauche/droite, identification par position paddle
- **Reset System** : Remise au centre de la balle après chaque point marqué

#### Accomplissements techniques ✅
- [x] Système Score et Game State complets ✅
- [x] Reset de partie et gestion de points ✅ 
- [x] Entity Factory pour création simplifiée ✅
- [x] Système de configuration GameConfig ✅
- [x] Intégration SDL2 complète (rendu, événements clavier) ✅
- [x] Boucle de jeu principale fonctionnelle ✅
- [x] **Jeu Pong complet et parfaitement jouable** ✅
- [x] Correction des bugs collision et physique ✅
- [x] Optimisation vitesse paddles et responsivité ✅

### Jeux à venir
- [x] **01_pong** - Jeu complet et jouable avec architecture ECS robuste ✅
- [ ] **02_flappy_bird** - Prêt pour implémentation après extraction des patterns réutilisables
- [ ] **03_breakout** - Planifié
- [ ] *(autres jeux selon la progression)*

### Leçons apprises
- **TDD efficace** : 19 tests atomiques permettent refactoring en toute confiance
- **Architecture ECS** : Modularité excellente, ajout/modification de systèmes sans casse
- **GameConfig centralisé** : Élimination des magic constants facilite le tuning gameplay
- **Système coordonnées** : Cohérence rendu/collision critique pour physique précise
- **SDL2 minimaliste** : Approche simple mais efficace pour prototypage rapide
- **Makefile sophistiqué** : Build robuste avec warnings stricts, sanitizers, tests parallèles
