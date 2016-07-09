# MultiplayerArduinoUSBJoystick
Turn an Arduino Mega 2560 into a composite USB device, managing 4-player 8-button joysticks.

This project was originally cloned from overpro/calltherain's ArduinoUSBJoystick project.
http://forums.eagle.ru/showthread.php?t=117011
https://github.com/calltherain/ArduinoUSBJoystick

That project was itself originated from Darran Hunt/harlequin-tech's arduino-usb firmwares.
http://hunt.net.nz/users/darran/weblog/15f92/
https://github.com/harlequin-tech/arduino-usb

This code is dependent upon the LUFA (Lightweight USB Framework for AVRs) Library, by Dean Camera. That code must be retrieved from the LUFA website separately, as it's not included in this project.
http://www.fourwalledcubicle.com/LUFA.php

Thank you to all these individuals whose contributions helped me make this project happen.

Workspace Notes
The firmware code needs to be built with LUFA, and so the makefiles expect to find LUFA in a particular place. For simplicity's sake, here's the directory structure that I used.

$WORKSPACE/arduino_usb/
$WORKSPACE/arduino_usb/firmwares/
$WORKSPACE/arduino_usb/firmwares/multiplayer_joystick
$WORKSPACE/LUFA/
$WORKSPACE/LUFA/LUFA/

Makefile paths are all relative to the multiplayer_joystick directory.

The LUFA package I used had its own LUFA sub-directory. Newer versions of the library may require small adaptations in the firmware code.

Building
You will need the avr-gcc toolchain and avr-libc.

cd $WORKSPACE/arduino_usb/firmwares/multiplayer_joystick
make
ls multiplayer_joystick.hex

Decent instructions for programming hex files to the board were provided by overpro, which can be found at his forum link above. I chose to go with the Flip tool, myself.
http://www.atmel.com/tools/flip.aspx
