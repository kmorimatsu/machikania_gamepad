# machikania_gamepad
USB Gamepad for MachiKania

## What is this?
Provide a gamepad used for MachiKania (especially for ResTouch version). This gamepad emulate USB keyboard to provide the arrow buttons and start/fire buttons information in MachiKania. Raspberry Pi Pico or RP2040 board is used.

## How to compile
cmake and make. The pico-sdk (ver 2.1.1 is confirmed for building) with submodules (execute "Submodule Update" for git clone) is required. 

## How to connect to MachiKania
Use a USB-OTG cable (with power input) for the connection to MachiKania.

## Circuit
![schematic.png](shematic.png)
Connect six button switches to the defined ports of RP2040:
```console
#define BUTTON_UP_PORT    10
#define BUTTON_LEFT_PORT  13
#define BUTTON_RIGHT_PORT 11
#define BUTTON_DOWN_PORT  12
#define BUTTON_START_PORT 14
#define BUTTON_FIRE_PORT  9
```
Connect the other part of switch to GND. See the definitions in "config.h".

Connect a speaker or piezoelectric-buzzer to the defined port of RP2040:
```console
#define AUDIO_PORT 8
```
Connect the other part of speaker to GND. See the definition in "config.h".  

Use appropriate resistor to speaker/piezoelectric-buzzer. When connecting a speaker, ensure that the combined resistance of the additional resistor and the speaker is more than 220 ohms. When connecting a piezo buzzer, connect a 10K ohm resistor in parallel.
