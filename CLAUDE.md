# CLAUDE.md

Ce fichier fournit des conseils à Claude Code (claude.ai/code) lors du travail avec le code de ce dépôt.

## Vue d'ensemble du projet

**20GC** est une implémentation pure en C du 20 Games Challenge utilisant une approche de Développement Dirigé par les Tests (TDD) et une architecture Entity-Component-System (ECS). Le projet utilise SDL2 pour le rendu et la gestion des entrées.

## Langue et documentation

**IMPORTANT** : Dans ce projet :
- La documentation (fichiers .md) est écrite en français
- Les commentaires de code sont écrits en français
- Le code lui-même (noms de variables, fonctions, etc.) reste en anglais
- Les interactions et communications à propos du code se font en français

## Architecture

- **Langage** : C pur
- **Modèle d'architecture** : Entity-Component-System (ECS)
- **Approche de développement** : Développement Dirigé par les Tests (TDD)
- **Graphismes/Entrées** : SDL2

## Contraintes de développement

### Fonctions
- **Maximum 20 lignes par fonction**
- **Une fonction = une responsabilité** (principe de responsabilité unique)
- **Nombre limité de paramètres** par fonction
- Privilégier l'utilisation de structures de données comme paramètres d'entrée

### Tests unitaires
- **Utilisation obligatoire de mocking** pour isoler les unités testées
- **Structures de données en entrée** pour les tests
- Couverture complète de tous les composants et systèmes

### Gestion des ressources
- **Aucune allocation dynamique** (pas de malloc/free)
- **Aucune lecture de fichiers/ressources** à l'exécution
- **Toutes les données sont statiques** et définies dans le code
- **Données connues au moment du cahier des charges**
- Contrôle total du processus de gestion des ressources

## Stratégie de développement

**Approche pragmatique** : Implémentation du premier jeu (Pong) avec TDD/ECS, puis extraction des éléments généralisables.

**Structure par jeu** : Chaque jeu dans son propre répertoire (01_pong, 02_flappy_bird, etc.)

**Outils de build** : Makefile simple + pkg-config pour SDL2, GTest, GMock

**Organisation des includes** : 
- **PCH (Precompiled Headers)** pour toutes les bibliothèques externes (SDL2, GTest, GMock, etc.)
- **Includes relatifs** (`#include "xxx.h"`) uniquement pour le code local du projet
- **Header guards** : Utilisation de `#pragma once` pour tous les fichiers .h
- **Formatage** : Configuration `.clang-format` pour un style de code cohérent
- **Coverage** : Intégration de la couverture de code avec lcov/gcov
- **Scripts de test d'environnement** : Scripts bash pour vérifier la disponibilité des outils (approche macOS d'abord, portabilité ensuite)

## Style de code et conventions

- **Une fonction publique par fichier .c** : Modularité maximale, style Verilog/testbench
- **Structure opaque** : `typedef struct Entity Entity;` avec implémentation cachée
- **Pool statique avec initialisation paresseuse** : Getter `get_pool()` avec `static` et `memset`
- **Early exit/continue** : `if (condition) continue;` sans accolades, code principal moins indenté
- **const partout** : `const` sur tous les paramètres et variables locales possibles
- **API cohérente** : Types uniformes (`uint32_t` partout au lieu de mix `size_t`/`uint32_t`)
- **Assert sur les échecs** : Avec données statiques, les erreurs = bugs de conception
- **Constantes centralisées** : `constants.h` inclus dans PCH

**Processus** :
1. Développer Pong completement avec approche TDD/ECS
2. Extraire les patterns et composants réutilisables
3. Créer une architecture commune pour les jeux suivants
4. Répéter le processus pour chaque nouveau jeu

## État actuel

Projet Pong en développement actif :
- **Infrastructure** : Makefile, PCH, scripts de dépendances, constants.h
- **Tests TDD** : Tests atomiques pour système d'entités (un test = un fichier)
- **Pool d'entités** : Implémentation avec structure opaque et pool statique
- **En cours** : Implémentation des fonctions publiques entity_*

## Configuration de développement

Puisqu'il s'agit d'un projet C avec dépendance SDL2, le développement typique impliquera probablement :
- Installation et liaison de la bibliothèque SDL2
- Configuration du compilateur C (gcc/clang)
- Configuration du système de construction (probablement Makefile ou CMake)
- Intégration du framework de test pour l'approche TDD avec mocking

## Considérations d'architecture ECS

Lors de l'implémentation de l'architecture ECS :
- **Entités** : Identifiants uniques pour les objets de jeu
- **Composants** : Conteneurs de données (position, vélocité, sprite, etc.)
- **Systèmes** : Processeurs de logique qui opèrent sur les entités avec des combinaisons de composants spécifiques

## Notes d'implémentation TDD

Le projet suit la méthodologie TDD, donc :
- Écrire les tests avant d'implémenter les fonctionnalités
- S'assurer que tous les tests passent avant de commiter
- Maintenir une couverture de test complète pour tous les composants et systèmes
- Utiliser des mocks pour isoler les unités sous test