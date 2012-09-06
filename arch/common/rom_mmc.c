/*
* Copyright (C) 2012 Texas Instruments, Inc.
* All rights reserved.
* Author: Olivier Deprez <o-deprez@ti.com>
* Contributor: Christina Warren <cawarren@ti.com>
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

#include <aboot/aboot.h>
#include <aboot/io.h>

#include <common/usbboot_common.h>
#include <common/alloc.h>
#include <common/omap_rom.h>
#include <common/mmc.h>

#include <libc/string.h>

#if defined CONFIG_IS_OMAP4
#include <omap4/hw.h>
#elif defined CONFIG_IS_OMAP5
#include <omap5/hw.h>
#endif

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

/* MMC/SD driver specific data */
struct mmc_storage_data {
	struct storage_specific_functions mmc_functions;
	u8 storage_device;
	struct mmc mmc;
	struct mmc_devicedata dd;
	int *dd_ptr;
	int (*rom_hal_mmchs_writedata)(u32 moduleid, u32 *buf);
	int (*rom_hal_mmchs_sendcommand)(u32 moduleid,
			u32 cmd, u32 arg, u32 *resp);
};

#define MMC_SECTOR_SZ 512

static struct mmc_storage_data mmcd;

static const u32 rom_hal_mmchs_writedata_addr[] = {
	0,           /*OMAP_REV_INVALID*/
	(0x25c2c|1), /*OMAP_4430_ES1_DOT_0*/
	(0x2ddd8|1), /*OMAP_4430_ES2_DOT_0*/
	(0x2ddd8|1), /*OMAP_4430_ES2_DOT_1*/
	(0x2df58|1), /*OMAP_4430_ES2_DOT_2*/
	(0x2df58|1), /*OMAP_4430_ES2_DOT_3*/
	(0x36028|1), /*OMAP_4460_ES1_DOT_0*/
	(0x36028|1),  /*OMAP_4460_ES1_DOT_1*/
	(0x36028|1),  /*4470 placeholder*/
	(0x3ee18|1),  /*OMAP_5430_ES1_DOT_0*/
	(0x3ee18|1)  /*OMAP_5432_ES1_DOT_0*/
};

static const u32 rom_hal_mmchs_sendcommand_addr[] = {
	0,           /*OMAP_REV_INVALID*/
	(0x25aa8|1), /*OMAP_4430_ES1_DOT_0*/
	(0x2dc54|1), /*OMAP_4430_ES2_DOT_0*/
	(0x2dc54|1), /*OMAP_4430_ES2_DOT_1*/
	(0x2ddd4|1), /*OMAP_4430_ES2_DOT_2*/
	(0x2ddd4|1), /*OMAP_4430_ES2_DOT_3*/
	(0x35ea4|1), /*OMAP_4460_ES1_DOT_0*/
	(0x35ea4|1),  /*OMAP_4460_ES1_DOT_1*/
	(0x35ea4|1),  /*4470 placeholder*/
	(0x3ec8c|1),  /*OMAP_5430_ES1_DOT_0*/
	(0x3ec8c|1)  /*OMAP_5432_ES1_DOT_0*/
};

static void mmc_reg_write(u32 reg_offset, u32 val)
{
	u32 reg_addr;
	reg_addr = (mmcd.storage_device == DEVICE_SDCARD) ?
		(reg_offset + OMAP_HSMMC1_BASE) :
		(reg_offset + OMAP_HSMMC2_BASE);
	*(volatile u32 *)reg_addr = val;
}

static u32 mmc_reg_read(u32 reg_offset)
{
	u32 reg_addr;
	reg_addr = (mmcd.storage_device == DEVICE_SDCARD) ?
		(reg_offset + OMAP_HSMMC1_BASE) :
		(reg_offset + OMAP_HSMMC2_BASE);
	return *(volatile u32 *)reg_addr;
}

static int mmc_configure(struct mmc *mmc, u32 id, u32 value)
{
	int n;
	struct mmc_config config;
	config.configid = id;
	config.value = value;

	n = mmc->io->configure(&mmc->dread, &config);
	if (n) {
		printf("mmc_configure failed\n");
		return n;
	}

	return 0;
}

static int mmc_init(u8 device)
{
	struct mem_device *md;
	struct mmc_devicedata *dd =  NULL;
	u16 options;
	int n;

	if (!((device == DEVICE_SDCARD) || (device == DEVICE_EMMC))) {
		printf("unsupported mmc device\n");
		return -1;
	} else
		mmcd.storage_device = device;

	n = rom_get_mem_driver(&mmcd.mmc.io, mmcd.storage_device);
	if (n) {
		printf("rom_get_mem_driver failed\n");
		return n;
	}

	/* clear memory device descriptor */
	md = &mmcd.mmc.dread;
	memset(md, 0, sizeof(struct mem_device));

	/*initialize device data buffer*/
	if (mmcd.dd_ptr == NULL) {
		mmcd.dd_ptr = (void *) zalloc_memory(SIZEOF_MMC_DEVICE_DATA);
		if (mmcd.dd_ptr == NULL) {
			printf("mmc device data memory allocation failed\n");
			return -1;
		} else
			dd = (void *) mmcd.dd_ptr;
	} else {
		/* use previously allocated memory and zero out the buffer */
		dd = (void *) mmcd.dd_ptr;
		memset(dd, 0, SIZEOF_MMC_DEVICE_DATA);
	}

	options			= 1;
	md->initialized		= 0;
	md->device_type		= mmcd.storage_device;
	md->xip_device		= 0;
	md->search_size		= 0;
	md->base_address	= 0;
	md->hs_toc_mask		= 0;
	md->gp_toc_mask		= 0;
	md->boot_options	= &options;
	md->device_data		= dd;

	n = mmcd.mmc.io->init(md);
	if (n) {
		printf("mmc->io->init failed\n");
		return n;
	}

	/* Copy what we care about in dd for later */
	memcpy(&mmcd.dd, dd, sizeof(struct mmc_devicedata));

	/* force raw mode operation (if ever a filesystem was detected at
	init)*/
	dd->mode = MMCSD_MODE_RAW;

	switch (dd->type) {

	/* Configure SD card to 4b @ 19.2 MHz */
	case MMCSD_TYPE_SD:

		/*configure larger bus width*/
		n = mmc_configure(&mmcd.mmc, MMCSD_CONFIGID_SETBUSWIDTH,
				MMCSD_4BIT_BUS_WIDTH_SUPPORTED);
		if (n) {
			printf("mmc_configure: failed to configure "
							"buswidth\n");
			return n;
		}

		/*configure higher clock*/
		n = mmc_configure(&mmcd.mmc, MMCSD_CONFIGID_SETCLOCK,
				MMCSD_CLOCK_DIVIDER_19_2MHZ);
		if (n) {
			printf("mmc_configure: failed to set clock\n");
			return n;
			}

		break;


	/* Configure eMMC width and divider clock:
	*  SDR mode for omap4 and DDR mode for omap5
	*/

	case MMCSD_TYPE_MMC:

		/* configure SDR/DDR mode and bus width */
		n = mmc_configure(&mmcd.mmc, MMCSD_CONFIGID,
				MMCSD_WIDTH_SUPPORTED);
		if (n) {
			printf("mmc_configure: failed to set DDR "
							"mode\n");
			return n;
		}

		/* configure clock divider*/
		n = mmc_configure(&mmcd.mmc, MMCSD_CONFIGID_SETCLOCK,
					MMCSD_CLOCK_DIVIDER);
		if (n) {
			printf("mmc_configure: failed to set clock\n");
			return n;
		}

		break;

	default:
		printf("Unsupported mmcsd device!\n");
		break;
	}

	return 0;
}

static int mmc_read(u64 start_sec, u64 sectors, void *data)
{
	struct read_desc rd;
	int n;

	if ((start_sec > 0xFFFFFFFF) ||
		(sectors > 0xFFFFFFFF)) {
		printf("mmc_read failed. start_sec or sectors too large.\n");
		return -1;
	}

	rd.sector_start	= (u32)start_sec;
	rd.sector_count	= (u32)sectors;
	rd.destination	= data;

	n = mmcd.mmc.io->read(&mmcd.mmc.dread, &rd);
	if (n) {
		printf("mmc_read failed\n");
		return n;
	}

	return 0;
}

static int mmc_write(u64 start_sec, u64 sectors, void *data)
{
	struct mmc_devicedata *dd;
	struct mem_device *md;
	u32 arg;
	u32 resp[4];
	int n;
	u32 blkreg;

	if ((start_sec > 0xFFFFFFFF) ||
		(sectors > 0xFFFFFFFF)) {
		printf("mmc_write failed. start_sec or sectors too large.\n");
		return -1;
	}

	/* Get the device data structure */
	md = &mmcd.mmc.dread;
	dd = md->device_data;

	if (dd->addressing == MMCSD_ADDRESSING_SECTOR)
		/* In case of sector addressing,
		the address given is the sector nb */
		arg = (u32)start_sec;
	else
		/* In case of byte addressing,
		the address given is start sector * MMCSD_SECTOR_SIZE */
		arg = (u32)start_sec << MMCSD_SECTOR_SIZE_SHIFT;

	blkreg = mmc_reg_read(OMAP_HSMMC_BLK_OFFSET);
	blkreg &= ~HSMMC_BLK_NBLK_MASK;
	mmc_reg_write(OMAP_HSMMC_BLK_OFFSET, (blkreg |
			(u32)sectors<<HSMMC_BLK_NBLK_SHIFT));

	/* Send the CMD25 write command */
	n = mmcd.rom_hal_mmchs_sendcommand(dd->moduleid,
			MMCSD_CMD25 | MMCHS_MMCHS_CMD_BCE_ENABLE |
			MMCHS_MMCHS_CMD_ACEN_ENABLECMD12 |
			MMCHS_MMCHS_CMD_MSBS_MULTIBLK, arg, resp);
	if (n) {
		printf("mmc_sendcommand failed\n");
		return n;
	}

	/* Write the data */
	n = mmcd.rom_hal_mmchs_writedata(dd->moduleid, data);
	if (n) {
		printf("mmc_writedata failed\n");
		return n;
	}

	return 0;
}

static int mmc_erase (u64 start_sec, u64 sectors)
{
	struct mmc_devicedata *dd;
	struct mem_device *md;
	u32 start_sector, end_sector;
	u32 resp[4];
	int n;
	u32 reg;
	u32 cmd_start, cmd_end, arg;

	if ((start_sec > 0xFFFFFFFF) ||
		(sectors > 0xFFFFFFFF)) {
		printf("mmc_erase failed. start_sec or sectors too large.\n");
		return -1;
	}

	start_sector = (u32) start_sec;
	end_sector = (u32) (start_sec + sectors - 1);

	/* Get the device data structure */
	md = &mmcd.mmc.dread;
	dd = md->device_data;

	if (dd->addressing != MMCSD_ADDRESSING_SECTOR) {
		start_sector = start_sector << MMCSD_SECTOR_SIZE_SHIFT;
		end_sector = end_sector << MMCSD_SECTOR_SIZE_SHIFT;
	}

	if (mmcd.storage_device == DEVICE_SDCARD) {
		cmd_start = MMCSD_CMD32;
		cmd_end = MMCSD_CMD33;
		arg = 0;
	} else {
		cmd_start = MMCSD_CMD35;
		cmd_end = MMCSD_CMD36;
		arg = 0x00000001;
	}

	n = mmcd.rom_hal_mmchs_sendcommand(dd->moduleid,
				cmd_start, start_sector, resp);
	if (n) {
		printf("mmc_sendcommand failed\n");
		return n;
	}

	n = mmcd.rom_hal_mmchs_sendcommand(dd->moduleid,
				cmd_end, end_sector, resp);
	if (n) {
		printf("mmc_sendcommand failed\n");
		return n;
	}

	n = mmcd.rom_hal_mmchs_sendcommand(dd->moduleid,
				MMCSD_CMD38, arg, resp);
	if (n) {
		printf("mmc_sendcommand failed\n");
		return n;
	}
	while (1) {
		reg = mmc_reg_read(OMAP_HSMMC_PSTATE_OFFSET);
		DBG("reg = 0x%08x\n", reg);
		if (reg & MMCHS_WAIT_CARD_BUSY_DEASSERT) {
			ldelay(100000000);
			continue;
		}
		break;
	}
#ifdef DEBUG
	u8 data[500];
	mmc_read(start_sec, 1, (void *)data);
	for (n = 0; n < 500; n++)
		printf ("%02x ", data[n]);
	printf("\n");
#endif
	return 0;
}

static int get_mmc_sector_size(void)
{
	return MMC_SECTOR_SZ;
}

static u64 get_mmc_total_sectors(void)
{
	u64 total_sectors = (u64)mmcd.dd.size;
	return total_sectors;
}


struct storage_specific_functions *init_rom_mmc_funcs(u8 device)
{
	if (!((device == DEVICE_SDCARD) || (device == DEVICE_EMMC))) {
		printf("unsupported mmc device\n");
		return NULL;
	}

	if (boot_ops->proc_ops->proc_get_proc_id) {
		mmcd.rom_hal_mmchs_writedata =
			API(&rom_hal_mmchs_writedata_addr
			[boot_ops->proc_ops->proc_get_proc_id()]);

		mmcd.rom_hal_mmchs_sendcommand =
			API(&rom_hal_mmchs_sendcommand_addr
			[boot_ops->proc_ops->proc_get_proc_id()]);
	} else
		return NULL;

	mmcd.mmc_functions.init = mmc_init;
	mmcd.mmc_functions.get_sector_size = get_mmc_sector_size;
	mmcd.mmc_functions.read = mmc_read;
	mmcd.mmc_functions.write = mmc_write;
	mmcd.mmc_functions.get_total_sectors = get_mmc_total_sectors;
	mmcd.mmc_functions.erase = mmc_erase;

	mmcd.dd_ptr = NULL;

	return &mmcd.mmc_functions;
}
