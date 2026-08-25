# Pong — SDL_GPU + EnTT

Template Pong minimal utilisant l'API bas niveau `SDL_GPU` (et non
`SDL_Renderer`), EnTT et les callbacks d'application SDL3.

## Contrôles

- joueur gauche : `W` / `S`
- joueur droit : flèches haut / bas
- manettes : stick gauche ou croix directionnelle (une manette par joueur)
- tactile : une zone par moitié d'écran
- `R` : remettre le score à zéro
- `Escape` : quitter

Le terrain est exprimé en unités monde (`16 × 9`) avec Y orienté vers le haut.
Le viewport GPU conserve ce ratio avec letterboxing, indépendamment de la
résolution et de la densité de pixels de la fenêtre.

## Architecture

`main.cpp` ne connaît aucun jeu : il transmet les callbacks SDL à
`tgc::core::Application`. Le cycle de vie commun passe par `tgc::core::IGame`.
`src/tgc/games/pong/Bootstrap.cpp` enregistre la configuration et construit
`tgc::games::pong::Pong` sans introduire de dépendance du socle générique vers
le jeu.

Les chemins suivent les namespaces : le code générique vit sous `src/tgc`, et
le code spécifique à Pong sous `src/tgc/games/pong`. Les fichiers `.h`
contiennent les déclarations et les `.cpp` les implémentations non inline.

## Shaders

Les shaders HLSL sources sont dans `src/tgc/gpu/shaders`. Les variantes SPIR-V, DXIL
et MSL sont précompilées et embarquées dans l'exécutable afin de ne pas
compiler des shaders à l'exécution. Pour les régénérer, installer la CLI
[SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross), puis lancer :

```sh
./src/tgc/gpu/shaders/build-shaders.sh
```

## Portabilité

- Vulkan/SPIR-V : Linux, Windows et plateformes disposant du backend Vulkan ;
- D3D12/DXIL : Windows et Xbox avec leur environnement SDL approprié ;
- Metal/MSL : macOS et appareils iOS/tvOS compatibles.

`SDL_MAIN_USE_CALLBACKS` évite d'imposer une boucle principale bloquante et
prépare le cycle de vie mobile/Web. Toutefois, SDL_GPU 3.4 ne fournit pas de
backend WebGPU officiel : le template ne peut donc pas encore rendre dans un
navigateur avec SDL_GPU seul. Les consoles nécessitent également les ports,
SDK et parfois formats de shaders fournis par leur constructeur.
