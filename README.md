# el-gato

## Sensors/Pins

A0 - Sonar sensor for measuring distance
A1 - Left Eye. 255=OFF, 0=DIM
A2 - Left Eye. 255=OFF, 0=BRIGHT
Digital 8 - Right Eye. HIGH=OFF, LOW=DIM
Digital 9 - Right Eye. HIGH=OFF, LOW=BRIGHT
SDReader - The reader for the SD card containing WAV files

## Global State

Several variables associated with playing the WAV files must be saved to global state. These take up a lot of memory, but I was unable to get them working in any other way.

There are also two enumerations to track the current state of the cat and the lights. The cat can be PURRING, MEOWING, or GREETING. The lights can be OFF, DIM, or BRIGHT.

## Setup

The setup method initializes the SDReader and its root directory. It turns lights off, plays a purring sound, and waits a few seconds to loop.

## Loop

Loop delays 2 seconds before starting again.

If the cat is PURRING or GREETING the lights alternate between DIM and OFF each loop.

The distance is chcked with the sonar sensor. If the object is far away, purring continues indefinitely. If the object is a little closer, meowing begins. Lights start bright and then solidly dim until the state changes. (TODO: This could get annoying. Contingency for an object being placed in meowing range?)

Once the object is close enough, we play the greeting. Lights flash while the greeting is being played. As long as an object stays in greeting or meowing range, random greetings continue to play every 10 seconds or so with lights flashing during speech. If nothing is in range for 10 seconds or so, go back to purring.



