SW implementation startet with a home-brew cooperative multitasking.
This works fine, but some parts of the SW (e.g. LCD routines) are fare away from beeing "cooperative".
The latest version is stored in the zip file; still not fully implemented!

So I started a different concept. 
The idea is to use the already running FreeRTOS of the ESP32 module board to change the complete SW from cooperative to preemptive scheduling.
I start with an almost empty project and step by step I will move the already coded functions to the preemptive task scheduling.

Current state is:

FreeRTOS-Tasks
- TskMyIODrive.ino; Input driver modle is running and tested. 
- TskMyKTY.ino;     KTY module is running, results are wrong! => DEBUGGING required
- TskMyRelays.ino;  Relay control module, under construction

Other modules
- MyConfig.ino;     Configuration module as a standard arduino function (carry over from last version)
- MyEncoder.ino;    Encoder module ist running; completely interrupt driven; no FreeRTOS task!
