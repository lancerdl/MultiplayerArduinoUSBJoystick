/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
 * multiplayer_joystick.h
 * Copyright (c) 2016 Matthew Nikkanen (mjnikkan [at] gmail [dot] com)
 * Copyrights licensed under Unilicense.
 * See the accompanying LICENSE file for terms.
 *
 * Credits
 * This file uses additional Open Source components not mentioned above. You
 * can find the source code of their open source projects along with license
 * information below. We acknowledge and are grateful to these developers for
 * their contributions to open source.
 *
 * Project: arduino-usb https://github.com/harlequin-tech/arduino-usb
 * Copyright (c) 2011 Darran Hunt (darran [at] hunt dot net dot nz)
 *
 * Project: ArduinoUSBJoystick https://github.com/calltherain/ArduinoUSBJoystick
 * No copyright provided.
 * License (unilicense) http://unlicense.org
 */

/** \file
 *
 *  Header file for multiplayer_joystick.c.
 */

#ifndef _MULTIPLAYER_JOYSTICK_H_
#define _MULTIPLAYER_JOYSTICK_H_


/* Includes: */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "descriptors.h"

#include <LUFA/Drivers/Misc/RingBuffer.h>
#include <LUFA/Version.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/USB/Class/CDCClass.h>
#include <LUFA/Common/Common.h>


/** Index for each joystick axis. */
typedef enum Axis_e_ {
    AXIS_FIRST = 0,
    AXIS_X     = AXIS_FIRST,
    AXIS_Y,
    AXIS_NUM,
} Axis_e;


/* Macros: */

#ifdef LEDS_ENABLE
/** LED mask for the library LED driver, to indicate TX activity. */
#define LEDMASK_TX      LEDS_LED1

/** LED mask for the library LED driver, to indicate RX activity. */
#define LEDMASK_RX      LEDS_LED2

/** LED mask for the library LED driver, to indicate that an error has
 *  occurred in the USB interface.
 */
#define LEDMASK_ERROR   (LEDS_LED1 | LEDS_LED2)

/** LED mask for the library LED driver, to indicate that the USB interface
 *  is busy.
 */
#define LEDMASK_BUSY    (LEDS_LED1 | LEDS_LED2)		
#endif /* LEDS_ENABLE */


/* Function Prototypes: */

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_UnhandledControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
					 uint8_t* const ReportID,
					 const uint8_t ReportType,
					 void* ReportData,
					 uint16_t* const ReportSize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
					  const uint8_t ReportID, 
					  const uint8_t ReportType,
					  const void* ReportData,
					  const uint16_t ReportSize);

#endif /* _MULTIPLAYER_JOYSTICK_H_ */
