<div align="center">
  <h1>Guimbal Cabri G2 Glass Cockpit</h1>
  <p>Implementation of the Guimbal Cabri G2 glass cockpit for the STM32U5G9J-DK2 kit</p>
  <p>As part of the Guimbal Cabri G2 Flight Simulator project at IUT de Cachan</p>
</div>

---

<!-- TOC -->

- [Usage](#usage)
- [Project Compilation](#project-compilation)
  - [Prerequisites](#prerequisites)
  - [STM32 Project Compilation](#stm32-project-compilation)
  - [X-Plane Interface Compilation](#x-plane-interface-compilation)
  - [Development](#development)

<!-- /TOC -->

## Usage

You can find the French version of the README [here](/README_FR.md)

The Cabri G2 is a helicopter designed by Guimbal. The flight simulator project at IUT de Cachan aims to create a full-scale reproduction of the helicopter’s cockpit. This project includes the implementation of the glass cockpit for the STM32U5G9J-DK2 kit.

It also includes the program that retrieves data from the X-Plane simulator, enabling real-time transmission of simulator data to the STM32 board. This can be found in [XPlaneInterface](/XPlaneInterface)

## Project Compilation

### Prerequisites

For the STM32 project, you will need:
 - STM32CubeIDE 0.18.0

For the X-Plane interface, you will need:
 - Windows
 - Zig 0.14.0 - https://zvm.app/

Once ZVM is installed, you need to install Zig 0.14.0 by running the following commands:

```sh
zvm install 0.14.0
zvm use 0.14.0
```

### STM32 Project Compilation

For the STM32 project, open it in STM32CubeIDE.
Once opened, connect the board and launch the program.

### X-Plane Interface Compilation

To compile, run the following command in the XPlaneInterface root directory:

```sh
zig build
```

The compiled executable can be found in `XPlaneInterface/build/Release`

### Development

[Visual Studio Code](https://code.visualstudio.com/) is recommended for X-Plane interface development, as it includes all the necessary tools for efficient debugging. However, if you wish to compile in Debug mode, run the following command in the XPlaneInterface root directory:

```sh
zig build -Ddev
```

The compiled executable and associated PDB file will be in `XPlaneInterface/build/Debug`