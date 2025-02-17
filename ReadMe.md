# Pokemon themed Flipper firmware
Here is a custom theme for the Flipper Zero portable multi-tool. 
This fork is based off of the modified firmware from [MuddledBox](https://github.com/MuddledBox/flipperzero-firmware) and includes artwork based on characters belonging to [The Pokémon Company](https://www.pokemon.com/us/). This is intended for personal use only.

![Poliwhirl](https://i.imgur.com/RncLTdm.gif)

To install this firmware on to your Flipper Zero:

- Download the latest firmware release
- Using the QFlipper application, connect your Flipper Zero and select "Install from file"
- Select the downloaded firmware and wait for the installation to finish.

![Pikachu](https://i.imgur.com/khuKFJc.png)

To add the custom idle animations:

- from this Repository, navigate to assets>resources>dolphin and copy the contents of that folder to the "dolphin" folder on the root of the SD card

![Alakazam](https://i.imgur.com/IJnzL79.png)

This is a work in progress, so check back for periodic updates.

![Mewtwo](https://i.imgur.com/5v9Mat2.png)

Enjoy!

# Flipper Zero Firmware

[![Discord](https://img.shields.io/discord/740930220399525928.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](http://flipperzero.one/discord)

![Show me the code](https://habrastorage.org/webt/eo/m0/e4/eom0e4btudte7nrhnyic-laiog0.png)

Welcome to [Flipper Zero](https://flipperzero.one/)'s Firmware repo!
Our goal is to create nice and clean code with good documentation, to make it a pleasure for everyone to work with.

# Update firmware

[Get Latest Firmware from Update Server](https://update.flipperzero.one/)

Flipper Zero's firmware consists of three components:

- Core2 firmware set - proprietary components by ST: FUS + radio stack. FUS is flashed at factory and you should never update it.
- Core1 Bootloader - controls basic hardware initialization and loads firmware.
- Core1 Firmware - HAL + OS + Drivers + Applications.

All 3 of them must be flashed in order described.

## With STLink

### Core1 Bootloader + Firmware

Prerequisites:

- Linux / macOS
- Terminal
- [arm-gcc-none-eabi](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- openocd

One liner: `make flash`

### Core2 flashing procedures

Prerequisites:

- Linux / macOS
- Terminal
- STM32_Programmer_CLI (v2.5.0) added to $PATH

One liner: `make flash_radio`

## With USB DFU 

1. Download latest [Firmware](https://update.flipperzero.one)

2. Reboot Flipper to Bootloader
 - Press and hold `← Left` + `↩ Back` for reset 
 - Release `↩ Back` and keep holding `← Left` until blue LED lights up
 - Release `← Left`
<!-- ![Switch to DFU sequence](https://habrastorage.org/webt/uu/c3/g2/uuc3g2n36f2sju19rskcvjzjf6w.png) -->

3. Run `dfu-util -D full.dfu -a 0`

# Build with Docker

## Prerequisites

1. Install [Docker Engine and Docker Compose](https://www.docker.com/get-started)
2. Prepare the container:

 ```sh
 docker-compose up -d
 ```

## Compile everything

```sh
docker-compose exec dev make
```

Check `dist/` for build outputs.

Use **`flipper-z-{target}-full-{suffix}.dfu`** to flash your device.

# Build on Linux/macOS

## macOS Prerequisites

Make sure you have [brew](https://brew.sh) and install all the dependencies:
```sh
brew bundle --verbose
```

## Linux Prerequisites

### gcc-arm-none-eabi

```sh
toolchain="gcc-arm-none-eabi-10.3-2021.10"
toolchain_package="$toolchain-$(uname -m)-linux"

wget -P /opt "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/$toolchain_package.tar.bz2"

tar xjf /opt/$toolchain_package.tar.bz2 -C /opt
rm /opt/$toolchain_package.tar.bz2

for file in /opt/$toolchain/bin/* ; do ln -s "${file}" "/usr/bin/$(basename ${file})" ; done
```

### Optional dependencies

- openocd (debugging/flashing over SWD)
- heatshrink (compiling image assets)
- clang-format (code formatting)
- dfu-util (flashing over USB DFU)
- protobuf (compiling proto sources)

For example, to install them on Debian, use:
```sh
apt update
apt install openocd clang-format-13 dfu-util protobuf-compiler
```

heatshrink has to be compiled [from sources](https://github.com/atomicobject/heatshrink).

## Compile everything

```sh
make
```

Check `dist/` for build outputs.

Use **`flipper-z-{target}-full-{suffix}.dfu`** to flash your device.

## Flash everything

Connect your device via ST-Link and run:
```sh
make whole
```

# Links

* Discord: [flipp.dev/discord](https://flipp.dev/discord)
* Website: [flipperzero.one](https://flipperzero.one)
* Kickstarter page: [kickstarter.com](https://www.kickstarter.com/projects/flipper-devices/flipper-zero-tamagochi-for-hackers)
* Forum: [forum.flipperzero.one](https://forum.flipperzero.one/)

# Project structure

- `applications`    - Applications and services used in firmware
- `assets`          - Assets used by applications and services
- `bootloader`      - Bootloader source code
- `core`            - Furi Core: os level primitives and helpers
- `debug`           - Debug tool: GDB-plugins, SVD-file and etc
- `docker`          - Docker image sources (used for firmware build automation)
- `documentation`   - Documentation generation system configs and input files
- `firmware`        - Firmware source code
- `lib`             - Our and 3rd party libraries, drivers and etc...
- `make`            - Make helpers
- `scripts`         - Supplementary scripts and python libraries home

Also pay attention to `ReadMe.md` files inside of those directories.
