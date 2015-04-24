# UAVTelemetryReceiver
The UAV Telemetry receiver is a device which provides PWM control signals to the FlightController receiver and transmits informations from the UAVRemoteController to the FlightController unit.

## Features
This device has these main functions:

1. Receive GPS and KEYB data from a remote controller and translate into:
	1A. PWM signals to the FCU (pitch, yaw, roll, throttle, flightmode)
	2A. UAVTalk objects for realtime waypoint navigation
2. Read UAV status/alarm flags and retransmits to the RemoteController

## Contents
- /docs/ Sketches of the system
- /Project_mbed_stm32f10x_src/: uVision project with mbed and mbd-rtos sources

## Changelog

> 24.04.2015-001 branch: master --------------
 - Project creation from UAVRemoteController project
 NEXT STEPS:
 - Modify Cubemx project to select new GPIO and provide a PDF pinout file
 - Modify main.cpp to choose new selected GPIOs
 - Remove unused tasks and add new ones (UAVTalkManager, ServoController see mbed servo library)
 - Provide a serial terminal for testing and debugging (Terminal module see mbed for libraries)
 
