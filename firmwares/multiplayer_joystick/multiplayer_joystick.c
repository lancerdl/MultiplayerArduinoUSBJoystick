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

/*-
 * Copyright (c) 2011 Darran Hunt (darran [at] hunt dot net dot nz)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * multiplayer_joystick.c
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
 * Project: ArduinoUSBJoystick https://github.com/calltherain/ArduinoUSBJoystick
 * No copyright provided.
 * License (unilicense) http://unlicense.org
 */

/** \file
 *
 * Main source file for the multiplayer-joystick project. This file contains
 * the main tasks of the project and is responsible for the initial
 * application hardware configuration.
 */

#include "multiplayer_joystick.h"


/** Synchronization */
#define SYNC_BYTE       0xFF
#define SYNC_BYTE_NUM   3
static uint8_t sync_bytes;

/** Circular buffer to hold data from the serial port before it is sent to
 * the host.
 */
static RingBuffer_t usart_to_usb_buffer;

/** Joystick report storage.
 *
 * The structure modeling the HID Joystick report for storing and sending to
 * the host PC. This mirrors the layout described to the host in the HID
 * report descriptor, in descriptors.c.
 */
typedef struct __attribute__((__packed__)) USB_joystick_report_data_t_ {
    int8_t axis[AXIS_NUM]; /* Array of joystick axes */
    uint8_t buttons;       /* Bit mask of the currently pressed buttons */
} USB_joystick_report_data_t;

#define JOYSTICK_REPORT_BUFFER_SIZE    (sizeof(USB_joystick_report_data_t) * \
                                        HID_IF_NUM)
static uint8_t joystick_report_buffer[JOYSTICK_REPORT_BUFFER_SIZE];
static uint8_t prev_joystick_report_buffer[JOYSTICK_REPORT_BUFFER_SIZE];

/** Endpoint state structure.
 *
 * A structure for recording the state of an HID interface endpoint.
 * It is used to store the endpoint number, and keep track of how long since
 * an endpoint has been last updated. In actuality however, all endpoints
 * will likely send their reports at the same time.
 *
 * The default timeout is 1000 milliseconds.
 */
#define IDLE_TIMEOUT_DEFAULT    0x03E8
typedef struct Endpoint_state_t_ {
    USB_if_endpoint_e ep_num;
    uint16_t          idle_timeout;
    uint16_t          idle_count;
} Endpoint_state_t;
static Endpoint_state_t Ep_state[HID_IF_NUM];


/** Configures the board hardware and chip peripherals. */
static void setup_hardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Hardware Initialization */
    Serial_Init(115200, true);
#ifdef LEDS_ENABLE
    LEDs_Init();
#endif
    USB_Init();

    UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));
}

/** Retrieve the part of the report buffer belonging to the given interface. */
static void select_report(int if_ix, uint8_t **report, uint8_t **prev_report)
{
    if (if_ix >= HID_IF_NUM) {
        /* Not a valid interface index. Return the first interface. */
        if_ix = 0;
    }

    *report =
        &joystick_report_buffer[sizeof(USB_joystick_report_data_t) * if_ix];
    *prev_report =
        &prev_joystick_report_buffer[sizeof(USB_joystick_report_data_t) *
                                     if_ix];
}

/** Per-interface report task. */
static void interface_report(void)
{
    uint16_t report_size = sizeof(USB_joystick_report_data_t);

    /* Device must be connected and configured for the task to run. */
    if (USB_DeviceState != DEVICE_STATE_Configured) {
        return;
    }

    /* Update reports for all interfaces. */
    for (int if_ix = 0; if_ix < HID_IF_NUM; if_ix++) {
        Endpoint_state_t *ep_ptr = &Ep_state[if_ix];
        bool idle_expiry = false;
        uint8_t *report, *prev_report;

        /* Select the Report Endpoint. */
        Endpoint_SelectEndpoint(ENDPOINT_DIR_IN | ep_ptr->ep_num);

        /* Check if idle timer defined by HOST has elapsed. */
        if ((ep_ptr->idle_timeout &&
             (ep_ptr->idle_count >= ep_ptr->idle_timeout)) ||
            (ep_ptr->idle_count >= IDLE_TIMEOUT_DEFAULT)) {
            /* The idle time has elapsed. */
            idle_expiry = true;
        }

        select_report(if_ix, &report, &prev_report);

        /* Send reports if the endpoint is ready and there's been an
         * idle-timeout, or a change in report contents.
         */
        if (Endpoint_IsINReady() &&
            (idle_expiry || (memcmp(prev_report, report, report_size) != 0))) {

            /* Write Joystick Report Data */
            Endpoint_Write_Stream_LE(report, report_size, NULL);

            /* Save the current buffer data for comparing in next round. */
            memcpy(prev_report, report, report_size);

            /* Reset the idle counter after a report is sent. */
            ep_ptr->idle_count = 0;
        }

        /* Finalize the stream transfer to send the last packet. */
        Endpoint_ClearIN();
    }
}

/** Main program entry point.
 *
 * This routine contains the overall program flow,
 * including initial setup of all components and the main program loop.
 */
int main(void)
{
    /* Initialize synchronization. */
    sync_bytes = 0;

    /* Initialize the report buffers. */
    memset(joystick_report_buffer, 0, JOYSTICK_REPORT_BUFFER_SIZE);
    memset(prev_joystick_report_buffer, 0, JOYSTICK_REPORT_BUFFER_SIZE);
    RingBuffer_InitBuffer(&usart_to_usb_buffer, joystick_report_buffer,
                          JOYSTICK_REPORT_BUFFER_SIZE);

    /* Reset endpoint states. */
    for (int if_ix = 0; if_ix < HID_IF_NUM; if_ix++) {
        Endpoint_state_t *ep_ptr = &Ep_state[if_ix];

        ep_ptr->ep_num = IF_EP_FIRST + if_ix;
        ep_ptr->idle_timeout = IDLE_TIMEOUT_DEFAULT;
        ep_ptr->idle_count = 0;
    }

    /* Demo setup. */
    setup_hardware();

    GlobalInterruptEnable();

    sei();

    for (;;) {
        interface_report();
        USB_USBTask();
    }
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    for (int if_ix = 0; if_ix < HID_IF_NUM; if_ix++) {
        Endpoint_ConfigureEndpoint(ENDPOINT_DIR_IN | Ep_state[if_ix].ep_num,
                                   EP_TYPE_INTERRUPT, IF_EPSIZE, 1);
    }

    USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Unhandled Control Request event. */
void EVENT_USB_Device_UnhandledControlRequest(void)
{
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    /* Advance all the endpoint idle counters. */
    for (int if_ix = 0; if_ix < HID_IF_NUM; if_ix++) {
        Ep_state[if_ix].idle_count++;
    }
}

/** Event handler for the USB device Control Request event. */
void EVENT_USB_Device_ControlRequest(void)
{
    uint8_t *report, *prev_report;
    uint8_t report_size;

    /* Handle HID Class specific requests */
    switch (USB_ControlRequest.bRequest) {
    case HID_REQ_GetReport:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST |
                                                 REQTYPE_CLASS |
                                                 REQREC_INTERFACE)) {
            Endpoint_ClearSETUP();

            /* Select the requested report. */
            report_size = sizeof(USB_joystick_report_data_t);
            select_report(USB_ControlRequest.wIndex, &report, &prev_report);

            /* Write the report to the control endpoint */
            Endpoint_Write_Control_Stream_LE(report, report_size);
            Endpoint_ClearOUT();
        }
        break;
    case HID_REQ_SetIdle:
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE |
                                                 REQTYPE_CLASS |
                                                 REQREC_INTERFACE)) {
            int if_ix;

            Endpoint_ClearSETUP();
            Endpoint_ClearStatusStage();

            if_ix = USB_ControlRequest.wIndex;
            if (if_ix < HID_IF_NUM) {
                /* Get idle period in MSB, must multiply by 4 to get the
                 * duration in milliseconds.
                 */
                Ep_state[if_ix].idle_timeout =
                    ((USB_ControlRequest.wValue & 0xFF00) >> 6);
            }
        }
        break;
    case HID_REQ_GetIdle:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST |
                                                 REQTYPE_CLASS |
                                                 REQREC_INTERFACE)) {
            int if_ix;

            Endpoint_ClearSETUP();

            if_ix = USB_ControlRequest.wIndex;
            if (if_ix < HID_IF_NUM) {
                /* Write the current idle duration to the host, must be
                 * divided by 4 before sent to host.
                 */
                Endpoint_Write_8(Ep_state[if_ix].idle_timeout >> 2);
            }

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }
        break;
    default:
        break;
    }
}

/** HID class driver callback.
 *
 * This function processes HID reports from the host.
 *
 * \param[in] HIDInterfaceInfo  Pointer to the HID class interface
 *                              configuration structure being referenced
 * \param[in] ReportID    Report ID of the received report from the host
 * \param[in] ReportType  The type of report that the host has sent, either
 *                        REPORT_ITEM_TYPE_Out or REPORT_ITEM_TYPE_Feature
 * \param[in] ReportData  Pointer to a buffer where the created report has
 *                        been stored
 * \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    /* Not used but must be present */
}

/** Interrupt Service Register
 *
 * Manage the reception of data from the serial port, placing received bytes
 * into a circular buffer for later transmission to the host.
 */
ISR(USART1_RX_vect, ISR_BLOCK)
{
    uint8_t rx_byte = UDR1;

    /*
     * Check for the sync signal.
     */
    if (rx_byte == SYNC_BYTE) {
        sync_bytes++;
    } else {
        if (sync_bytes) {
            /* One or more previous bytes have been sync bytes (or looked like
             * a sync byte). Handle the possible cases.
             */
            if (sync_bytes > 1) {
                /* Received all sync bytes. Reset usart_to_usb_buffer.
                 * We expect to receive 3 bytes, but 2 or more is sufficient.
                 */
                RingBuffer_InitBuffer(&usart_to_usb_buffer,
                                      joystick_report_buffer,
                                      JOYSTICK_REPORT_BUFFER_SIZE);
            } else if (sync_bytes == 1) {
                /* Broken sequence. The previous sync byte was actually a
                 * data byte. This can happen if all buttons are depressed on
                 * a joystick. Replay that byte and continue with the new one.
                 */
                RingBuffer_Insert(&usart_to_usb_buffer, SYNC_BYTE);
            }

            /* Reset for the next sync. */
            sync_bytes = 0;
        }

        /*
         * Store regular data.
         */
        RingBuffer_Insert(&usart_to_usb_buffer, rx_byte);
    }
}
