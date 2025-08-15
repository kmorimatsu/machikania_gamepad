# machikania_gamepad
USB Gamepad for MachiKania

## What is this?
Provide a gamepad used for MachiKania (especially for ResTouch version). This gamepad emulate USB keyboard to provide the arrow buttons and start/fire buttons information in MachiKania. Raspberry Pi Pico or RP2040 board is used.

## How to compile
cmake and make. The pico-sdk (ver 2.1.1 is confirmed for building) with submodules (execute "Submodule Update" for git clone) is required. 

## How to connect to MachiKania
Use a USB-OTG cable (with power input) for the connection to MachiKania.

## Circuit

Connect six button switches to the defined ports of RP2040:
```console
#define BUTTON_UP_PORT    12
#define BUTTON_LEFT_PORT  13
#define BUTTON_RIGHT_PORT 14
#define BUTTON_DOWN_PORT  15
#define BUTTON_START_PORT 1
#define BUTTON_FIRE_PORT  5
```
Connect the other part of switch to GND. See the definitions in "main.c".
