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
 * descriptors.h
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
 *  Header file for descriptors.c.
 */
 
#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

/* Includes: */

#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/USB/Class/HIDClass.h>


/* Type Defines: */

/** Interface IN Endpoint numbers. */
typedef enum USB_if_endpoint_e_ {
    IF_EP_FIRST = 1,
    IF_EP_1     = IF_EP_FIRST,
    IF_EP_2,
    IF_EP_3,
    IF_EP_4,
    IF_EP_NUM   = IF_EP_4,
} USB_if_endpoint_e;


/* Macros: */

/** Size in bytes of each interface HID reporting IN endpoint. */
#define IF_EPSIZE 8


/* Function Prototypes: */

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif /* _DESCRIPTORS_H_ */
