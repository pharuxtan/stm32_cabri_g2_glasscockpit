<div align="center">
  <h1>Guimbal Cabri G2 Glass Cockpit</h1>
  <p>Implémentation du glass cockpit du Guimbal Cabri G2 sur le kit STM32U5G9J-DK2</p>
  <p>Dans le cadre du projet Simulateur de vol Guimbal Cabri G2 de l'IUT de Cachan</p>
</div>

---

<!-- TOC -->

- [Usage](#usage)
- [Compilation du projet](#compilation-du-projet)
  - [Prérequis](#prérequis)
  - [Compilation du projet STM32](#compilation-du-projet-stm32)
  - [Compilation de l'interface X-Plane](#compilation-de-linterface-x-plane)
  - [Développement](#développement)

<!-- /TOC -->

## Usage

Vous retrouverez le README en anglais [ici](/README.md)

Le Cabri G2 est un hélicoptère conçu par l'entreprise Guimbal. Le projet de simulateur de vol de l'IUT de Cachan a pour but de réaliser, à échelle réel, une reproduction du cockpit de l'hélicptère. Ce projet contient l'implémentation du glass cockpit sur le kit STM32U5G9J-DK2.

Il contient également l'implémentation du programme récupérant les données du simulateur X-Plane, permettant d'envoyer les informations en temps réel du simulateur vers la carte STM32. Ce trouvant dans [XPlaneInterface](/XPlaneInterface)

## Compilation du projet

### Prérequis

Pour le projet STM32, vous aurez besoin de :
 - STM32CubeIDE 0.18.0

Pour l'interface X-Plane, vous aurez besoin de :
 - Windows
 - Zig 0.14.0 - https://zvm.app/

Une fois que vous avez installé ZVM, vous devez installer Zig 0.14.0. Pour cela, il vous suffit d'exécuter les commandes suivantes :

```sh
zvm install 0.14.0
zvm use 0.14.0
```

### Compilation du projet STM32

Pour le projet stm32, ouvrez le projet sur STM32CubeIDE.
Une fois ouvert, branchez la carte et lancez le programme.

### Compilation de l'interface X-Plane

Pour compiler, exécutez à la racine du projet dans XPlaneInterface la commande suivante :

```sh
zig build
```

Vous trouverez l'exécutable compilé dans le dossier `XPlaneInterface/build/Release`

### Développement

Il est recommandé d'utiliser [Visual Studio Code](https://code.visualstudio.com/) pour développer l'interface X-Plane, car il intègre tous les outils nécessaires pour déboguer efficacement. Néanmoins, si vous souhaitez compiler en mode Debug, exécutez à la racine du projet dans XPlaneInterface la commande suivante :

```sh
zig build -Ddev
```

Vous trouverez l'exécutable compilé ainsi que le PDB associé dans le dossier `XPlaneInterface/build/Debug`