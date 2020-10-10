# MIDI_KEYBOARD

Project for the exam "Sistemi Operativi", "Università degli Studi di Roma La Sapienza".

Author: **Matteo Biffoni**

Professor: **Giorgio Grisetti**\
Tutor: **Irvin Aloise**

## Overview

MIDI KEYBOARD is a piano-like instrument implemented with an AVR ATMega 2560 which allows to use the avr as a keyboard and the host as a sound-maker.

The avr hardware configuration includes:
* An AVR board (ATMega 2560)
* 7 hardware buttons to hold and release notes
* A bunch of wires to connect the buttons

(An image showing the avr configuration is attached ==> See `arduino_conf.png`)

Every time a button is pressed or released, the avr sends an appropriate note event through the serial and the host receiving it translates this event in an update of the sound production.

The "music" part is implemented with the open-source OpenAL APIs and consists of filling buffers with appropriate data then queueing and unqueueing them to update the sound production.

## Install

In order to compile required files, the *GNU Make* build system is used.

##### Prerequisites
* Open a terminal
* Run `sudo apt-get install build-essential`
* Run `sudo apt-get install git`
* Run ```sudo apt-get install libopenal1```
* Run ```sudo apt-get install libopenal-dev```

##### Download the repository:
* Open a terminal
* Go to the directory where MIDI_KEYBOARD should be stored (e.g. `cd path/to/the/directory`)
* Run `sudo git clone https://github.com/matteobiffoni/MIDI_KEYBOARD.git`

##### Compile:
* To compile both avr and host part, make sure to have the avr connected and run `make all`
* To compile only the host part, run `make host`
* To compile only the avr part, run `make avr`

##### Execution:
* If the avr part was compiled, the ATMega2560 should already be running its program
* To execute the host part, run `./target/host/midi_keyboard`

##### Cleaning:
* To clean all the `.o` files and the executables, run `make clean`

##### Notes:
* Make sure the host has something to reproduce sounds (such as speakers) and has its volume enabled
* If no avr is available, an OpenAL test using PC keyboard (but in a slightly different way: press a key to enable a note, press again to disable) is present at `./test/openal`

## Further informations
##### Note event structure
| Field   | Size (bits) | Description |
|:-------:|:-----------:|:-----------:|
|Padding   |      3      | Zeros needed to reach a byte|
|Parity    |      1      | A bit containing the parity checksum for the note event|
|Value     |      3      | Represents the pitch this note event refers to (more below)|
|State     |      1      | Represents the state of this event (more below)|
###### Note value enumeration
| Name | Value |
|:----:|:-----:|
|C|0x01|
|D|0x02|
|E|0x03|
|F|0x04|
|G|0x05|
|A|0x06|
|B|0x07|
###### Note state enumeration
| Name | Value |
|:----:|:-----:|
|Released|0x00|
|Pressed|0x01|
