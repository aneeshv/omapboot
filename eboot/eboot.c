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

#include <aboot.h>
#include <common.h>
#include <io.h>

#include <common_proc.h>
#include <fastboot.h>
#include <omap_rom.h>
#include <usbboot_common.h>
#include <alloc.h>
#include <user_params.h>

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

#ifdef TWO_STAGE_OMAPBOOT
static int run_sboot(struct bootloader_ops *boot_ops)
{
	int ret = 0;
	struct fastboot_ptentry *pte;
	void (*Sboot)(struct bootloader_ops *boot_ops);

	int sector_sz = boot_ops->storage_ops->get_sector_size();

	int num_sectors = SECOND_STAGE_OBJECT_SIZE/sector_sz;

	u32 addr = CONFIG_ADDR_DOWNLOAD;

	/* look for sboot in bootloader ptn, load and jump */
	ret = load_ptbl(boot_ops->storage_ops, 1);
	if (ret != 0) {
		printf("unable to load the partition table\n");
		return ret;
	}

	pte = fastboot_flash_find_ptn("bootloader");
	if (!pte) {
		printf("eboot: cannot find '%s' partition\n");
		return -1;
	}

	ret = boot_ops->storage_ops->read(pte->start, num_sectors,
								(void *) addr);
	if (ret != 0) {
		printf("mmc read failed\n");
		return ret;
	}

	Sboot = (void (*)(u32, int, void *))(addr);
	Sboot(boot_ops);

	return -1;
}
#endif

void do_eboot(struct bootloader_ops *boot_ops)
{
	int err;

	if (boot_ops->board_ops->board_user_fastboot_request)
		if (boot_ops->board_ops->board_user_fastboot_request())
			goto fastboot;

	do_booti(boot_ops, "storage", NULL);
	return;

fastboot:
	err = usb_open(&boot_ops->usb, INIT_USB);
	if (err) {
		printf("Opening USB failed\n");
		return;
	}
	do_fastboot(boot_ops);

	return ret;
}

void eboot(struct bootloader_ops *boot_ops)
{
#ifdef TWO_STAGE_OMAPBOOT
#if DO_MEMORY_TEST_DURING_FIRST_STAGE_IN_EBOOT
	memtest((void *)0x82000000, 8*1024*1024);
	memtest((void *)0xA0208000, 8*1024*1024);
#endif
	run_sboot(boot_ops);
#else
	do_eboot(boot_ops);
#endif
}
