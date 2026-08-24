# 20GC

Collection de templates C++23 construits avec CMake et vcpkg.

## Prérequis

- CMake 4.4 ou supérieur
- un compilateur compatible C++23
- Git
- Ninja pour les presets Ninja
- Xcode pour le preset Xcode sur macOS

Pour VS Code, installer les extensions suivantes :

- **CMake Tools** (`ms-vscode.cmake-tools`)
- **C/C++** (`ms-vscode.cpptools`)

## Initialisation

Cloner le dépôt avec ses sous-modules :

```sh
git clone --recurse-submodules git@github.com:kdridi/20GC.git
cd 20GC
```

Pour initialiser les sous-modules dans un dépôt déjà cloné :

```sh
git submodule update --init --recursive
```

Initialiser vcpkg sur macOS ou Linux :

```sh
./extern/vcpkg/bootstrap-vcpkg.sh
```

Sur Windows :

```powershell
.\extern\vcpkg\bootstrap-vcpkg.bat
```

## Ligne de commande avec Ninja

Afficher tous les presets disponibles :

```sh
cmake --list-presets=all
```

### Debug

```sh
cmake --preset ninja-debug
cmake --build --preset ninja-debug
./build/ninja-debug/templates/simple_fmt/simple_fmt
```

### Release

```sh
cmake --preset ninja-release
cmake --build --preset ninja-release
./build/ninja-release/templates/simple_fmt/simple_fmt
```

Pour supprimer le cache CMake et forcer une configuration propre :

```sh
cmake --preset ninja-debug --fresh
```

## Ligne de commande avec Xcode

Générer le projet Xcode en utilisant le chemin physique du dépôt :

```sh
cd "$(pwd -P)"
cmake --preset xcode
```

L'utilisation du chemin physique est importante lorsque le dépôt est atteint par un lien symbolique : Xcode exige que les chemins du projet et des sources soient identiques pour indexer correctement le code.

Compiler en Debug ou Release :

```sh
cmake --build --preset xcode-debug
cmake --build --preset xcode-release
```

Ouvrir le projet généré dans Xcode :

```sh
open "$(pwd -P)/build/xcode/tgc.xcodeproj"
```

Exécuter directement le template compilé :

```sh
./build/xcode/templates/simple_fmt/Debug/simple_fmt
./build/xcode/templates/simple_fmt/Release/simple_fmt
```

Le preset Xcode est disponible uniquement sur macOS.

## Signature locale des exécutables macOS

La signature avec une identité Apple Development stable évite que macOS considère chaque nouveau build comme une nouvelle application lorsqu'il est exécuté depuis un volume amovible.

Afficher les identités disponibles :

```sh
security find-identity -v -p codesigning
```

Configurer un build Ninja signé :

```sh
cmake --preset ninja-debug \
  -DTGC_CODESIGN_IDENTITY="Apple Development: name@example.com (TEAM_ID)"
cmake --build --preset ninja-debug
```

Configurer le projet Xcode signé :

```sh
cmake --preset xcode \
  -DTGC_CODESIGN_IDENTITY="Apple Development: name@example.com (TEAM_ID)"
cmake --build --preset xcode-debug
```

L'identité est conservée dans le cache du répertoire de build. Il faut la fournir à nouveau après une configuration avec `--fresh` ou après avoir supprimé ce répertoire.

Vérifier la signature :

```sh
codesign --verify --strict --verbose=2 \
  build/ninja-debug/templates/simple_fmt/simple_fmt

codesign -dvv --requirements - \
  build/ninja-debug/templates/simple_fmt/simple_fmt
```

L'identité personnelle ne doit pas être ajoutée à `CMakePresets.json`. Elle peut être conservée dans un `CMakeUserPresets.json`, ignoré par Git.

Après configuration depuis le terminal, VS Code réutilise l'identité enregistrée dans le cache CMake correspondant.

## Utilisation avec VS Code

Ouvrir le dépôt :

```sh
code .
```

Ouvrir la palette de commandes avec `Cmd+Shift+P` sur macOS ou `Ctrl+Shift+P` sur Windows/Linux, puis utiliser les commandes suivantes.

### Configurer avec Ninja

1. **CMake: Select Configure Preset**
2. sélectionner **Ninja Debug** ou **Ninja Release**
3. **CMake: Configure**
4. **CMake: Select Build Preset**
5. sélectionner le preset Ninja correspondant
6. **CMake: Build**

### Configurer avec Xcode

1. **CMake: Select Configure Preset**
2. sélectionner **Xcode**
3. **CMake: Configure**
4. **CMake: Select Build Preset**
5. sélectionner **xcode-debug** ou **xcode-release**
6. **CMake: Build**

### Exécuter ou déboguer un template

1. **CMake: Set Launch/Debug Target**
2. sélectionner **simple_fmt**
3. utiliser **CMake: Run Without Debugging** pour l'exécuter
4. ou utiliser **CMake: Debug** pour démarrer le débogueur

Si l'autocomplétion clangd ne se met pas à jour :

1. **CMake: Configure** avec le preset **Ninja Debug**
2. **clangd: Restart language server**
3. si nécessaire, **Developer: Reload Window**

## Organisation des templates

Chaque répertoire `templates/<nom>` produit automatiquement une cible portant le même nom.

Les sources C++ sont découvertes récursivement dans :

```text
templates/<nom>/src
templates/<nom>/platform/<CMAKE_SYSTEM_NAME>
```

Seules les sources correspondant à la plateforme cible sont compilées. Par exemple, sur macOS, CMake utilise `platform/Darwin`.
