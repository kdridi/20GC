# Shaders SDL_GPU de Pong

## Organisation

Les sources maintenues manuellement sont :

- `solid.hlsli` : interface HLSL partagée entre les deux étapes ;
- `solid.vert.hlsl` : vertex shader ;
- `solid.frag.hlsl` : fragment shader.

`solid.hlsli` est un fichier d'inclusion, comparable à un header C++. Il n'a
pas de point d'entrée `main` et n'est donc pas compilé seul. Son contenu est
incorporé aux shaders qui l'incluent avant leur compilation.

Les fichiers de `generated/` sont des ressources produites par outil :

- `*.spv.h` contient du SPIR-V pour Vulkan ;
- `*.dxil.h` contient du DXIL pour Direct3D 12 ;
- `*.msl.h` contient du code MSL pour Metal, stocké comme un tableau d'octets.

Chaque résultat de `shadercross` est transformé en tableau C++ par `xxd -i`,
puis embarqué dans l'exécutable. Il ne faut pas modifier ces headers à la main.

## Provenance des variantes actuelles

Les premières variantes embarquées proviennent des shaders `cube` du dossier
`test/testgpu` de SDL 3, distribué sous licence zlib. Les sources HLSL initiales
de Pong étaient identiques à celles de ce test ; les six payloads ont donc été
repris à l'identique et seuls les noms de fichiers et de tableaux ont été
renommés de `cube_*` vers `solid_*`.

Cela explique pourquoi leur création initiale n'a pas nécessité l'installation
de `SDL_shadercross` sur macOS. Pour toute évolution réelle du shader, il faut
en revanche utiliser la procédure de génération ci-dessous.

## Régénération

### Outils requis

- le CLI `shadercross` du projet `SDL_shadercross` ;
- `xxd` ;
- Bash.

Le script accepte aussi des chemins explicites :

```bash
SHADERCROSS=/chemin/vers/shadercross \
XXD=/chemin/vers/xxd \
./build-shaders.sh
```

### Sur une machine où les outils sont installés

Depuis ce dossier :

```bash
./build-shaders.sh
```

Le script compile les deux shaders dans les trois formats, convertit les six
résultats en headers, puis supprime les fichiers binaires intermédiaires.

### Installation reproductible sur Linux x86-64 avec vcpkg

Depuis la racine du dépôt :

```bash
repo_root="$(git rev-parse --show-toplevel)"
vcpkg_root="$repo_root/extern/vcpkg"

(
    cd /tmp
    env -u VCPKG_ROOT \
        "$vcpkg_root/vcpkg" install sdl3-shadercross:x64-linux
)

export PATH="$vcpkg_root/installed/x64-linux/tools/sdl3-shadercross:$PATH"
cd "$repo_root/templates/pong/src/tgc/gpu/shaders"
./build-shaders.sh
```

Sur Debian ou Ubuntu, installer `xxd` au préalable si nécessaire :

```bash
sudo apt-get install xxd
```

Le checkout vcpkg du dépôt fixe la version du port et de ses compilateurs. Une
seconde génération avec le même checkout doit donc être reproductible.

### Image GitHub Container Registry

Le workflow `.github/workflows/shadercross.yml` construit l'image sur un runner
GitHub Linux x86-64 natif et la publie sous :

```text
ghcr.io/kdridi/20gc-shadercross:latest
```

Il est déclenché manuellement, lors d'un changement de l'outillage shader, ou
lors d'un changement des shaders sur `main`. Chaque exécution hors pull request
publie d'abord un tag correspondant au SHA complet du commit. Le tag `latest`
n'est déplacé qu'après validation des headers. Le cache GitHub Actions évite de
recompiler toute la chaîne vcpkg après la première construction.

Après la création du dépôt GitHub et le premier push, ouvrir l'onglet
**Actions**, sélectionner **Shadercross image and shaders**, puis lancer
**Run workflow** si le workflow ne s'est pas déjà déclenché. Le job :

1. construit `tools/shadercross/Dockerfile` pour `linux/amd64` ;
2. publie l'image dans GHCR ;
3. régénère les six headers avec cette image ;
4. échoue si les headers versionnés ne correspondent pas au résultat.

Les pull requests construisent et testent la même image sans la publier.

Si la toute première validation échoue parce que les headers existants ont été
produits par une autre version des compilateurs, l'image du commit est déjà
disponible. La récupérer avec son SHA, régénérer les headers, puis pousser le
résultat :

```bash
SHADERCROSS_IMAGE=ghcr.io/kdridi/20gc-shadercross:<sha-du-commit> \
    ./templates/pong/src/tgc/gpu/shaders/build-shaders-container.sh
```

Le push suivant valide ces headers et publie alors le tag `latest`.

### Utilisation depuis macOS Apple Silicon

Le port vcpkg `sdl3-shadercross` dépend actuellement de `directx-dxc`, qui
n'est pas supporté par le triplet `arm64-osx`. Il ne faut donc pas ajouter ce
port au `vcpkg.json` principal.

Une fois l'image CI publiée, la régénération locale se réduit à :

```bash
./templates/pong/src/tgc/gpu/shaders/build-shaders-container.sh
```

Le conteneur reste en `linux/amd64`, mais il ne fait qu'exécuter les outils déjà
construits. L'émulation sur Apple Silicon ne dure donc que le temps de compiler
les six petits shaders.

L'image GHCR est privée par défaut tant que sa visibilité n'a pas été modifiée
dans les paramètres du package GitHub. Dans ce cas, créer un token GitHub avec
le droit `read:packages`, puis se connecter avant le premier pull :

```bash
echo "$GHCR_TOKEN" | docker login ghcr.io --username kdridi --password-stdin
```

On peut aussi rendre le package public afin que les développeurs n'aient pas à
s'authentifier. Pour utiliser un fork ou une autre image :

```bash
SHADERCROSS_IMAGE=ghcr.io/autre-compte/20gc-shadercross:latest \
    ./templates/pong/src/tgc/gpu/shaders/build-shaders-container.sh
```

La construction locale complète de l'image x86-64 sur Apple Silicon reste
possible, mais elle est volontairement évitée car la compilation vcpkg sous
émulation est très lente.

## Contrôles après génération

Vérifier que les six fichiers attendus existent et examiner leur différence :

```bash
find generated -maxdepth 1 -type f -name 'solid.*.h' | sort
git diff -- generated
```

Puis compiler et lancer Pong :

```bash
cmake --build --preset ninja-debug --target pong_gpu
./build/ninja-debug/templates/pong/pong_gpu
```

Toute modification de `solid.hlsli`, `solid.vert.hlsl` ou `solid.frag.hlsl`
doit être accompagnée de la régénération et de la validation de ces six
headers sur les backends concernés.
