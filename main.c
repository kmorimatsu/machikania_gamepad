/*
 * The source code in this file was modified by Katsumi.
 * The license of modified file remain MIT license.
/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "hardware/gpio.h"

/*
	Configuration
*/

#define BUTTON_UP_PORT    12
#define BUTTON_LEFT_PORT  13
#define BUTTON_RIGHT_PORT 14
#define BUTTON_DOWN_PORT  15
#define BUTTON_START_PORT 1
#define BUTTON_FIRE_PORT  5

#define BUTTON_UP_KEY    HID_KEY_ARROW_UP
#define BUTTON_LEFT_KEY  HID_KEY_ARROW_LEFT
#define BUTTON_RIGHT_KEY HID_KEY_ARROW_RIGHT
#define BUTTON_DOWN_KEY  HID_KEY_ARROW_DOWN
#define BUTTON_START_KEY HID_KEY_F4
#define BUTTON_FIRE_KEY  HID_KEY_F1

#define NUM_BUTTONS 6
static const unsigned char button_ports[NUM_BUTTONS]={
	BUTTON_UP_PORT,
	BUTTON_LEFT_PORT,
	BUTTON_RIGHT_PORT,
	BUTTON_DOWN_PORT,
	BUTTON_START_PORT,
	BUTTON_FIRE_PORT,
};
static const unsigned char button_keys[NUM_BUTTONS]={
	BUTTON_UP_KEY,
	BUTTON_LEFT_KEY,
	BUTTON_RIGHT_KEY,
	BUTTON_DOWN_KEY,
	BUTTON_START_KEY,
	BUTTON_FIRE_KEY,
};

// Local global variable(s)
static int g_init_keys=0;


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
	// Initialize input buttons
	for(int i=0;i<NUM_BUTTONS; i++){
		gpio_init(button_ports[i]);
		gpio_set_dir(button_ports[i],GPIO_IN);
		gpio_pull_up(button_ports[i]);
	}

	board_init();
	tusb_init();

	while (1)
	{
		tud_task(); // tinyusb device task

		hid_task();
		
		sleep_ms(10);
	}
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void){
	// Send initialization keys for 1 seconds 
	g_init_keys=100;
}

// Invoked when device is unmounted
void tud_umount_cb(void){ }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void){ }

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
	// Poll every 10ms
	const uint32_t interval_ms = 10;
	static uint32_t start_ms = 0;

	if ( board_millis() - start_ms < interval_ms) return; // not enough time
	start_ms += interval_ms;

	// Remote wakeup
	if ( tud_suspended() && (false) )
	{
		// Wake up host if we are in suspend mode
		// and REMOTE_WAKEUP feature is enabled by host
		tud_remote_wakeup();
	}
	else
	{
		// skip if hid is not ready yet
		if ( !tud_hid_ready() ) return;
		
		// use to avoid send multiple consecutive zero report for keyboard
		static bool has_keyboard_key = false;

		// Send "MACGP" for 1 second after connection
		if (0<g_init_keys) {
			g_init_keys--;
			uint8_t keycode[6] = { HID_KEY_M, HID_KEY_A, HID_KEY_C, HID_KEY_G, HID_KEY_P,0 };
			tud_hid_keyboard_report(0 /*REPORT_ID_KEYBOARD*/, 0, keycode);
			has_keyboard_key = true;
			return;
		}

		// Check the button input
		int num_send=0;
		uint8_t keycode[6] = { 0,0,0,0,0,0 };
		for(int i=0;i<NUM_BUTTONS;i++){
			if (!gpio_get(button_ports[i])) {
				keycode[num_send++]=button_keys[i];
				if (6<=num_send) break;
			}
		}
		// Send key(s) if required
		if (0<num_send) {
			// Send key(s)
			tud_hid_keyboard_report(0 /*REPORT_ID_KEYBOARD*/, 0, keycode);
			has_keyboard_key = true;
		} else {
			// send empty key report if previously has key pressed
			if (has_keyboard_key) tud_hid_keyboard_report(0 /*REPORT_ID_KEYBOARD*/, 0, NULL);
			has_keyboard_key = false;
		
		}
		
	}
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) { }

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) { }

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) { }

