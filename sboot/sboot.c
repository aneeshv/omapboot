/*
* Copyright (C) 2012, Texas Instruments, Inc.
* Texas Instruments, <www.ti.com>
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

#include <aboot.h>
#include <common.h>
#include <io.h>

#include <common_proc.h>
#include <fastboot.h>
#include <omap_rom.h>
#include <usbboot_common.h>
#include <alloc.h>

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

u32 public_rom_base;

#ifdef BOARD_PROCESS_CMDLINE
static void process_cmdline(struct bootloader_ops *boot_ops,
				int bootdevice)
{
	/* Insert here command prompt processing */
	printf("Omapboot> ");
	ldelay(0x10000000);
	printf(". ");
	ldelay(0x10000000);
	printf(". ");
	ldelay(0x10000000);
	printf(".\n");
}
#endif

void sboot(struct bootloader_ops *boot_ops)
{
	int ret = 0;
	char buf[DEV_STR_LENGTH];
	int bootdevice = boot_ops->boot_device;

	if (boot_ops->proc_ops->proc_get_api_base)
		public_rom_base = boot_ops->proc_ops->proc_get_api_base();

	init_memory_alloc();

	dev_to_devstr(, buf);
	printf("Second Stage Boot: boot device: %s\n", buf);
	if (boot_ops->board_ops->board_user_fastboot_request) {
		if (boot_ops->board_ops->board_user_fastboot_request())
			goto fastboot;
	}

#ifdef BOARD_PROCESS_CMDLINE
	do_cmdline(boot_ops, bootdevice);
#endif

	if (bootdevice == DEVICE_USB)
		do_iboot(boot_ops);
	else
		do_eboot(boot_ops);

	/* If we reach here boot has failed */
	printf("boot failed\n");
	while (1)
		;
}
