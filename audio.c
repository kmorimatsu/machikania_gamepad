/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Katsumi Morimatsu
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

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "config.h"

static int g_pwm_slice;
static int g_pwm_channel;

void init_audio(void){
	// Set slice and channel
	g_pwm_slice=pwm_gpio_to_slice_num(AUDIO_PORT);
	g_pwm_channel=pwm_gpio_to_channel(AUDIO_PORT);	
	// Initialization (see music.c of MachiKania source code)
	uint32_t sys_clk_hz = clock_get_hz(clk_sys);
	// Allocate GPIO to the PWM
	gpio_set_function(AUDIO_PORT, GPIO_FUNC_PWM);
	// Set clock divier for fastest frequency
	pwm_set_clkdiv(g_pwm_slice, 0.25 * (float)(sys_clk_hz/SOUND_PWM_MUL/225280));
	// 2048 cycles PWM (an example for 440 Hz)
	pwm_set_wrap(g_pwm_slice, 2047);
	// Set duty to 50%
	pwm_set_chan_level(g_pwm_slice, g_pwm_channel, 1024);
	// Disable
	pwm_set_enabled(g_pwm_slice, false);
	// Reset counter
	pwm_set_counter(g_pwm_slice, 0);
}

void set_sound(unsigned short val){
	static unsigned short s_pwm_wrap;
	if (val<0xffff) {
		if (val!=s_pwm_wrap) {
			pwm_set_enabled(g_pwm_slice, false);
			pwm_set_wrap(g_pwm_slice, val*SOUND_PWM_MUL-1);
			pwm_set_chan_level(g_pwm_slice, g_pwm_channel, val*SOUND_PWM_MUL/2);
			pwm_set_counter(g_pwm_slice, 0);
		}
		pwm_set_enabled(g_pwm_slice, true);
	} else {
		pwm_set_enabled(g_pwm_slice, false);
	}
	s_pwm_wrap=val;
}
