/*
 * Copyright (C) 2012, Texas Instruments, Inc.
 * Texas Instruments, <www.ti.com>
 *
 * Copyright (C) 2012 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

unsigned char basic_header[512] = {
	/* CHSETTINGS(empty) + OPPLOW CHRAM */
	0xa0, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x43, 0x48, 0x53, 0x45,
	0x54, 0x54, 0x49, 0x4e, 0x47, 0x53, 0x00, 0x00,
	0x2c, 0x01, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x43, 0x48, 0x52, 0x41,
	0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc1, 0xc0, 0xc0, 0xc0,
#ifdef CONFIG_USE_CH_SETTINGS_CONFIG
	0x01, /* Valid CH Settings */
	0x00, /* Version */
	0x00, 0x00, /* Reserved */
	0xBD, 0x03, 0x00, 0x00, /* Flags */
	0x10, 0x01, 0x00, 0x00, /* CM_CLKSEL_CORE */
	0x00, 0x00, 0x00, 0x00, /* CM_BYPCLK_MPU */
	0x00, 0x00, 0x00, 0x00, /* CM_BYPCLK_IVA*/
	0x00, 0x00, 0x00, 0x01, /* CM_MPU_CLK_CTRL*/
	0x01, 0x00, 0x00, 0x00, /* CM_CLKSEL_USB */
	0x00, 0x00, 0x00, 0x00, /* CM_CLKMODE_DPLL_MPU */
	0x00, 0x00, 0x00, 0x00, /* CM_AUTOIDLE_DPLL_MPU */
	0x08, 0x77, 0x01, 0x00, /* CM_CLKSEL_DPLL_MPU */
	0x01, 0x00, 0x00, 0x00, /* CM_DIV_M2_DPLL_MPU */
	0x00, 0x00, 0x00, 0x00, /* CM_CLKMODE_DPLL_CORE */
	0x00, 0x00, 0x00, 0x00, /* CM_AUTOIDLE_DPLL_CORE */
	0x0b, 0x99, 0x02, 0x00, /* CM_CLKSEL_DPLL_CORE */
	0x02, 0x00, 0x00, 0x00, /* CM_DIV_M2_DPLL_CORE */
	0x05, 0x00, 0x00, 0x00, /* CM_DIV_M3_DPLL_CORE */
	0x08, 0x00, 0x00, 0x00, /* CM_DIV_H11_DPLL_CORE */
	0x04, 0x00, 0x00, 0x00, /* CM_DIV_H12_DPLL_CORE */
	0x62, 0x00, 0x00, 0x00, /* CM_DIV_H13_DPLL_CORE */
	0x05, 0x00, 0x00, 0x00, /* CM_DIV_H14_DPLL_CORE */
	0x05, 0x00, 0x00, 0x00, /* CM_DIV_H22_DPLL_CORE */
	0x07, 0x00, 0x00, 0x00, /* CM_DIV_H23_DPLL_CORE */
	0x05, 0x00, 0x00, 0x00, /* CM_CLKMODE_DPLL_PER */
	0x00, 0x00, 0x00, 0x00, /* CM_AUTOIDLE_DPLL_PER */
	0x00, 0x14, 0x00, 0x00, /* CM_CLKSEL_DPLL_PER */
	0x04, 0x00, 0x00, 0x00, /* CM_DIV_M2_DPLL_PER */
	0x03, 0x00, 0x00, 0x00, /* CM_DIV_M3_DPLL_PER */
	0x06, 0x00, 0x00, 0x00, /* CM_DIV_H11_DPLL_PER */
	0x04, 0x00, 0x00, 0x00, /* CM_DIV_H12_DPLL_PER */
	0x02, 0x00, 0x00, 0x00, /* CM_DIV_H14_DPLL_PER */
	0x00, 0x00, 0x00, 0x00, /* CM_CLKMODE_DPLL_USB */
	0x00, 0x00, 0x00, 0x00, /* CM_AUTOIDLE_DPLL_USB */
	0x07, 0x90, 0x01, 0x04, /* CM_CLKSEL_DPLL_USB */
	0x02, 0x00, 0x00, 0x00, /* CM_DIV_M2_DPLL_USB */
#else
	0x00, 0x00, 0x00, 0x00,
	0xef, 0xbe, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
#endif
	0xc2, 0xc0, 0xc0, 0xc0,
#if defined CONFIG_OMAP5UEVM
	0x01, /* Valid CH RAM */
	0x00, 0x00, 0x00,
	0x32, 0x1b, 0x85, 0x61, /* SD RAM Config */
	0x00, 0x00, 0x00, 0x00, /* SD RAM Config2 */
	0x71, 0x02, 0x00, 0x00, /* SD RAM Refresh */
	0x0d, 0x04, 0x00, 0x00, /* SD RAM Refresh Shadow */
	0xB3, 0x36, 0xCF, 0xcc, /* SD RAM Timing 1 */
	0xda, 0x7f, 0x8f, 0x30, /* SD RAM Timing 2 */
	0xa8, 0x88, 0x7f, 0x27, /* SD RAM Timing 3 */
	0xB3, 0x36, 0xCF, 0xcc, /* SD RAM Timing 1 Shadow */
	0xda, 0x7f, 0x8f, 0x30, /* SD RAM Timing 2 Shadow */
	0xa8, 0x88, 0x7f, 0x27, /* SD RAM Timing 3 Shadow */
#else
	0x00,
	0x00, 0x00, 0x00,
	0xba, 0x22, 0x80, 0x80, /* SD RAM Config */
	0x00, 0x00, 0x00, 0x00, /* SD RAM Config2 */
	0x71, 0x02, 0x00, 0x00, /* SD RAM Refresh */
	0x0d, 0x04, 0x00, 0x00, /* SD RAM Refresh Shadow */
	0x19, 0xb4, 0x86, 0x2a, /* SD RAM Timing 1 */
	0x4a, 0x09, 0x25, 0x10, /* SD RAM Timing 2 */
	0x2f, 0xa2, 0x6b, 0x02, /* SD RAM Timing 3 */
	0x19, 0xb4, 0x86, 0x2a, /* SD RAM Timing 1 Shadow */
	0x4a, 0x09, 0x25, 0x10, /* SD RAM Timing 2 Shadow */
	0x2f, 0xa2, 0x6b, 0x02, /* SD RAM Timing 3 Shadow */
#endif
	0x60, 0x60, 0x00, 0x00,
	0x60, 0x60, 0x00, 0x00,
	0x0d, 0x42, 0x18, 0x0e,
	0x00, 0x00, 0x00, 0x00,
	0x0d, 0x42, 0x18, 0x0e,
	0x80, 0x00, 0x02, 0x04, 0x52, 0x90, 0x82, 0x14,
	0xa4, 0x20, 0x05, 0x29, 0x48, 0x41, 0x0a, 0x00,
	0x40, 0x00, 0x01, 0x04, 0x10, 0x40, 0x00, 0x01,
	0x04, 0x10, 0x00, 0x00, 0x40, 0x00, 0x01, 0x04,
	0x10, 0x40, 0x00, 0x01, 0x04, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x08, 0x80,
	0x00, 0x00, 0x40, 0x00, 0x40, 0x20, 0x10, 0x08,
	0x01, 0x00, 0x00, 0x00, 0x50, 0x81, 0x0a, 0x54,
	0xa0, 0x02, 0x15, 0xa8, 0x40, 0x05, 0x2a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x06, 0x02, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x03, 0x74, 0x80, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
