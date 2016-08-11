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
 * descriptors.c
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
 * USB Device Descriptors, for library use when in USB device mode.
 * Descriptors are special computer-readable structures which the host
 * requests upon device enumeration, to determine the device's capabilities
 * and functions.  
 */

#include "descriptors.h"

/** HID class report descriptor.
 *
 * This is a special descriptor constructed with values from the USBIF HID
 * class specification to describe the reports and capabilities of the HID
 * device. This descriptor is parsed by the host and its contents used to
 * determine what data (and in what encoding) the device will send, and what
 * it may be sent back from the host. Refer to the HID specification for
 * more details on HID report descriptors.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM Joystick_report_format[] =
{
    0x05, 0x01,          /* Usage Page (Generic Desktop)                     */
    0x09, 0x04,          /* Usage (Joystick)                                 */

    0xa1, 0x01,          /* Collection (Application)                         */
    0x09, 0x01,          /*   Usage (Pointer)                                */

    /* 2-axis joystick, treated as a D-pad. */
    0xa1, 0x00,          /*   Collection (Physical)                          */
    0x05, 0x01,          /*     Usage Page (Generic Desktop)                 */
    0x09, 0x30,          /*     Usage (X)                                    */
    0x09, 0x31,          /*     Usage (Y)                                    */
    0x15, 0x00,          /*     Logical Minimum (0)                          */
    0x25, 0x64,          /*     Logical Maximum (100)                        */
    0x75, 0x08,          /*     Report Size (8)                              */
    0x95, 0x02,          /*     Report Count (2)                             */
    0x81, 0x82,          /*     Input (Data, Variable, Absolute, Volatile)   */
    0xC0,                /*   End Collection                                 */

    /* 8 game buttons. */
    0x05, 0x09,          /*   Usage Page (Button)                            */
    0x19, 0x01,          /*   Usage Minimum (1)                              */
    0x29, 0x08,          /*   Usage Maximum (8)                              */
    0x15, 0x00,          /*   Logical Minimum (0)                            */
    0x25, 0x01,          /*   Logical Maximum (1)                            */
    0x75, 0x01,          /*   Report Size (1)                                */
    0x95, 0x08,          /*   Report Count (8)                               */
    0x81, 0x02,          /*   Input (Data, Variable, Absolute)               */

    0xC0                 /* End Collection                                   */
};

/** USB Vendor and Product Ids. */
#define USB_VID_TEST_VID        0x03EB
#define USB_PID_JOYSTICK_DEMO   0x2043

/** USB descriptor string enums. */
typedef enum USB_descriptor_strings_e_ {
    USB_STR_LANGUAGE     = 0,
    USB_STR_MANUFACTURER,
    USB_STR_PRODUCT,
    USB_STR_SERIAL_NO,
    USB_STR_IF_0,
    USB_STR_IF_1,
    USB_STR_IF_2,
    USB_STR_IF_3,
    USB_STR_NUM,
} USB_descriptor_strings_e;

/** HID descriptor structure. */
typedef struct USB_HID_Interface_t_ {
    USB_Descriptor_Interface_t Interface;
    USB_HID_Descriptor_HID_t   HID;
    USB_Descriptor_Endpoint_t  Endpoint;
} USB_HID_Interface_t;

/** Device configuration descriptor structure.
 *
 * This must be defined in the application code, as the configuration
 * descriptor contains several sub-descriptors which vary between devices,
 * and which describe the device's usage to the host.
 */
typedef struct USB_Descriptor_Configuration_t_ {
    USB_Descriptor_Configuration_Header_t Config;
    USB_HID_Interface_t                   HID_Interface[HID_IF_NUM];
} USB_Descriptor_Configuration_t;

/** Device descriptor.
 *
 * This descriptor, located in FLASH memory (PROGMEM), describes the overall
 * device characteristics, including the supported USB version, control
 * endpoint size and the number of device configurations. The descriptor is
 * read out by the USB host when the enumeration process begins.
 */
const USB_Descriptor_Device_t PROGMEM Device_Descriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t),
                               .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(2, 0, 0),
    .Class                  = 0x00,
    .SubClass               = 0x00,
    .Protocol               = 0x00,
    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
    .VendorID               = USB_VID_TEST_VID,
    .ProductID              = USB_PID_JOYSTICK_DEMO,
    .ReleaseNumber          = 0x0004,
    .ManufacturerStrIndex   = USB_STR_MANUFACTURER,
    .ProductStrIndex        = USB_STR_PRODUCT,
    .SerialNumStrIndex      = USB_STR_SERIAL_NO,
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure.
 *
 * This descriptor, located in FLASH memory (PROGMEM), describes the usage
 * of the device in one of its supported configurations, including information
 * about any device interfaces and endpoints. The descriptor is read out by
 * the USB host during the enumeration process when selecting a configuration
 * so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM Configuration_Descriptor =
{
    .Config =
    {
        .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t),
                                   .Type = DTYPE_Configuration},

        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces        = HID_IF_NUM,
        .ConfigurationNumber    = 1,
        .ConfigurationStrIndex  = NO_DESCRIPTOR,
        .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED),
        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
    },
        
    .HID_Interface[0] =
    {
        .Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t),
                                       .Type = DTYPE_Interface},

            .InterfaceNumber        = 0x00,
            .AlternateSetting       = 0x00,
            .TotalEndpoints         = 1,
            .Class                  = 0x03,
            .SubClass               = 0x00,
            .Protocol               = HID_CSCP_NonBootProtocol,
            .InterfaceStrIndex      = USB_STR_IF_0
        },

        .HID =
        {
            .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                                       .Type = HID_DTYPE_HID},

            .HIDSpec                = VERSION_BCD(1, 11, 0),
            .CountryCode            = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType          = HID_DTYPE_Report,
            .HIDReportLength        = sizeof(Joystick_report_format)
        },

        .Endpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                       .Type = DTYPE_Endpoint},

            .EndpointAddress        = (ENDPOINT_DIR_IN | IF_EP_1),
            .Attributes             = (EP_TYPE_INTERRUPT |
                                       ENDPOINT_ATTR_NO_SYNC |
                                       ENDPOINT_USAGE_DATA),
            .EndpointSize           = IF_EPSIZE,
            .PollingIntervalMS      = 0x02
        },
    },

    .HID_Interface[1] =
    {
        .Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t),
                                       .Type = DTYPE_Interface},

            .InterfaceNumber        = 0x01,
            .AlternateSetting       = 0x00,
            .TotalEndpoints         = 1,
            .Class                  = 0x03,
            .SubClass               = 0x00,
            .Protocol               = HID_CSCP_NonBootProtocol,
            .InterfaceStrIndex      = USB_STR_IF_1
        },

        .HID =
        {
            .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                                       .Type = HID_DTYPE_HID},

            .HIDSpec                = VERSION_BCD(1, 11, 0),
            .CountryCode            = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType          = HID_DTYPE_Report,
            .HIDReportLength        = sizeof(Joystick_report_format)
        },

        .Endpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                       .Type = DTYPE_Endpoint},

            .EndpointAddress        = (ENDPOINT_DIR_IN | IF_EP_2),
            .Attributes             = (EP_TYPE_INTERRUPT |
                                       ENDPOINT_ATTR_NO_SYNC |
                                       ENDPOINT_USAGE_DATA),
            .EndpointSize           = IF_EPSIZE,
            .PollingIntervalMS      = 0x02
        },
    },

    .HID_Interface[2] =
    {
        .Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t),
                                       .Type = DTYPE_Interface},

            .InterfaceNumber        = 0x02,
            .AlternateSetting       = 0x00,
            .TotalEndpoints         = 1,
            .Class                  = 0x03,
            .SubClass               = 0x00,
            .Protocol               = HID_CSCP_NonBootProtocol,
            .InterfaceStrIndex      = USB_STR_IF_2
        },

        .HID =
        {
            .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                                       .Type = HID_DTYPE_HID},

            .HIDSpec                = VERSION_BCD(1, 11, 0),
            .CountryCode            = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType          = HID_DTYPE_Report,
            .HIDReportLength        = sizeof(Joystick_report_format)
        },

        .Endpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                       .Type = DTYPE_Endpoint},

            .EndpointAddress        = (ENDPOINT_DIR_IN | IF_EP_3),
            .Attributes             = (EP_TYPE_INTERRUPT |
                                       ENDPOINT_ATTR_NO_SYNC |
                                       ENDPOINT_USAGE_DATA),
            .EndpointSize           = IF_EPSIZE,
            .PollingIntervalMS      = 0x02
        },
    },

    .HID_Interface[3] =
    {
        .Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t),
                                       .Type = DTYPE_Interface},

            .InterfaceNumber        = 0x03,
            .AlternateSetting       = 0x00,
            .TotalEndpoints         = 1,
            .Class                  = 0x03,
            .SubClass               = 0x00,
            .Protocol               = HID_CSCP_NonBootProtocol,
            .InterfaceStrIndex      = USB_STR_IF_3
        },

        .HID =
        {
            .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t),
                                       .Type = HID_DTYPE_HID},

            .HIDSpec                = VERSION_BCD(1, 11, 0),
            .CountryCode            = 0x00,
            .TotalReportDescriptors = 1,
            .HIDReportType          = HID_DTYPE_Report,
            .HIDReportLength        = sizeof(Joystick_report_format)
        },

        .Endpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                       .Type = DTYPE_Endpoint},

            .EndpointAddress        = (ENDPOINT_DIR_IN | IF_EP_4),
            .Attributes             = (EP_TYPE_INTERRUPT |
                                       ENDPOINT_ATTR_NO_SYNC |
                                       ENDPOINT_USAGE_DATA),
            .EndpointSize           = IF_EPSIZE,
            .PollingIntervalMS      = 0x02
        },
    },
};

/** Language descriptor structure.
 *
 * This descriptor, located in FLASH memory, is returned when the host
 * requests the string descriptor with index 0 (the first index). It is
 * actually an array of 16-bit integers, which indicate via the language
 * ID table available at USB.org what languages the device supports for
 * its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM Language_String =
{
    .Header                 = {.Size = USB_STRING_LEN(1),
                               .Type = DTYPE_String},
        
    .UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string.
 *
 * This is a Unicode string containing the manufacturer's details in human
 * readable form, and is read out upon request by the host when the
 * appropriate string ID is requested, listed in the Device Descriptor.
 */
const USB_Descriptor_String_t PROGMEM Manufacturer_String =
{
    .Header                 = {.Size = USB_STRING_LEN(7),
                               .Type = DTYPE_String},
        
    .UnicodeString          = L"Arduino"
};

/** Product descriptor string.
 *
 * This is a Unicode string containing the product's details in human
 * readable form, and is read out upon request by the host when the
 * appropriate string ID is requested, listed in the Device Descriptor.
 *
 * Note: This is the name the device will take in Windows.
 */
const USB_Descriptor_String_t PROGMEM Product_String =
{
    .Header                 = {.Size = USB_STRING_LEN(20),
                               .Type = DTYPE_String},
        
    .UnicodeString          = L"Multiplayer Joystick"
};

/** Serial Number string. */
const USB_Descriptor_String_t PROGMEM Serial_Number_String =
{
    .Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},
        
    .UnicodeString          = L"1337D00D"
};

/** Interface descriptor strings. */
const USB_Descriptor_String_t Interface_Descriptor0 PROGMEM =
{
    .Header                 = {.Size = USB_STRING_LEN(8), .Type = DTYPE_String},
    .UnicodeString          = L"Player 1"
};

const USB_Descriptor_String_t Interface_Descriptor1 PROGMEM =
{
    .Header                 = {.Size = USB_STRING_LEN(8),
                               .Type = DTYPE_String},
    .UnicodeString          = L"Player 2"
};

const USB_Descriptor_String_t Interface_Descriptor2 PROGMEM =
{
    .Header                 = {.Size = USB_STRING_LEN(8),
                               .Type = DTYPE_String},
    .UnicodeString          = L"Player 3"
};

const USB_Descriptor_String_t Interface_Descriptor3 PROGMEM =
{
    .Header                 = {.Size = USB_STRING_LEN(8),
                               .Type = DTYPE_String},
    .UnicodeString          = L"Player 4"
};

/** Descriptor retrieval API.
 *
 * This function is called by the library when in device mode, and must
 * be overridden (see library "USB Descriptors" documentation) by the
 * application code so that the address and size of a requested descriptor
 * can be given to the USB library. When the device receives a Get Descriptor
 * request on the control endpoint, this function is called so that the
 * descriptor details can be passed back and the appropriate descriptor sent
 * back to the USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t descriptor_type   = (wValue >> 8);
    const uint8_t descriptor_number = (wValue & 0xFF);

    void*    address = NULL;
    uint16_t size    = NO_DESCRIPTOR;

    switch (descriptor_type) {
    case DTYPE_Device:
        address = (void*)&Device_Descriptor;
        size    = sizeof(USB_Descriptor_Device_t);
        break;
    case DTYPE_Configuration: 
        address = (void*)&Configuration_Descriptor;
        size    = sizeof(USB_Descriptor_Configuration_t);
        break;
    case DTYPE_String:
        switch (descriptor_number) {
        case USB_STR_LANGUAGE:
            address = (void*)&Language_String;
            size    = pgm_read_byte(&Language_String.Header.Size);
            break;
        case USB_STR_MANUFACTURER:
            address = (void*)&Manufacturer_String;
            size    = pgm_read_byte(&Manufacturer_String.Header.Size);
            break;
        case USB_STR_PRODUCT:
            address = (void*)&Product_String;
            size    = pgm_read_byte(&Product_String.Header.Size);
            break;
        case USB_STR_SERIAL_NO:
            address = (void*)&Serial_Number_String;
            size = pgm_read_byte(&Serial_Number_String.Header.Size);
            break;
        case USB_STR_IF_0:
            address = (void*)&Interface_Descriptor0;
            size = pgm_read_byte(&Interface_Descriptor0.Header.Size);
            break;
        case USB_STR_IF_1:
            address = (void*)&Interface_Descriptor1;
            size = pgm_read_byte(&Interface_Descriptor1.Header.Size);
            break;
        case USB_STR_IF_2:
            address = (void*)&Interface_Descriptor2;
            size = pgm_read_byte(&Interface_Descriptor2.Header.Size);
            break;
        case USB_STR_IF_3:
            address = (void*)&Interface_Descriptor3;
            size = pgm_read_byte(&Interface_Descriptor3.Header.Size);
            break;
        default:
            break;
        }
        break;
    case HID_DTYPE_HID:
        if (wIndex < HID_IF_NUM) {
            address = (void*)&Configuration_Descriptor.HID_Interface[wIndex];
            size    = sizeof(USB_HID_Descriptor_HID_t);
        } else {
            address = (void*)&Configuration_Descriptor.HID_Interface[0];
            size    = sizeof(USB_HID_Descriptor_HID_t);
        }

        break;
    case HID_DTYPE_Report:
        /* All reports are the same size, so no need to use wIndex. */
        address = (void*)&Joystick_report_format;
        size    = sizeof(Joystick_report_format);
        break;
    default:
        break;
    }

    *DescriptorAddress = address;
    return size;
}
