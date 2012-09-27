/*
 * Copyright (C) 2012, Texas Instruments, Inc.
 * Texas Instruments, <www.ti.com>
 * Author: Olivier Deprez <o-deprez@ti.com>
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
#include <io.h>
#include <usbboot_common.h>
#include <alloc.h>
#include <omap_rom.h>
#include <sata.h>
#include <string.h>
#include <hw.h>

/* TODO: Query real device size and sector size
 * 32GB in number of sectors
 * If we write 0x4000000, for some reason, the partitions
 * are not being recognized.
 * But if we take the 0x3b70000 (which is what we get when
 * we query EMMC of size 32GB), then all partitions are
 * being recognized.
 */
#define SATA_SIZE_IN_SECTORS	0x3b70000
#define SATA_SECTOR_SZ		512

#define SATA_RW_SECTOR_LIMIT	256

struct sata {
	struct mem_device dread;
	struct mem_driver *io;
};

struct sata_devicedata {
	u32 mode;
	u32 copy;
	u32 size;
	u32 rsv[509];
};

/* SATA driver specific data */
struct sata_storage_data {
	u8 storage_device;
	struct storage_specific_functions sata_functions;
	struct sata sata_rom_data;
	struct sata_devicedata *dd;
	u64 sector_size;
	void (*rom_satall_buildreadsectorcmd)
			(u32 cmdslot, u32 lba, u32 cnt, u32 adr);
	int sata_initialized;
};

struct sata_storage_data satad;

const u32 rom_satall_buildreadsectorcmd_addr[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	(0x40070|1), /*OMAP_5430_ES1_DOT_0*/
	(0x40070|1)  /*OMAP_5432_ES1_DOT_0*/
};

/* TODO: Query the device to get the device size, sector size */
static u32 sata_get_size()
{
	return SATA_SIZE_IN_SECTORS;
}

static u64 sata_get_sector_size(void)
{
	return (u64) SATA_SECTOR_SZ;
}

static void sata_reg_write(u32 reg_offset, u32 val)
{
	u32 reg_addr;
	reg_addr = reg_offset + OMAP_SATA_BASE;
	writel(val, reg_addr);
}

static u32 sata_reg_read(u32 reg_offset)
{
	u32 reg_addr;
	reg_addr = reg_offset + OMAP_SATA_BASE;
	return readl(reg_addr);
}

static int sata_init(u8 device)
{
	struct mem_device *md;
	u16 options;
	int n;

	if (device != DEVICE_SATA) {
		printf("unsupported device\n");
		return -1;
	} else
		satad.storage_device = DEVICE_SATA;

	/* Avoid calling rom sata init function again if already initialized */
	if (satad.sata_initialized == 1)
		return 0;

	n = rom_get_mem_driver(&satad.sata_rom_data.io, satad.storage_device);
	if (n) {
		printf("rom_get_mem_driver failed\n");
		return n;
	}

	/* clear memory device descriptor */
	md = &satad.sata_rom_data.dread;
	memset(md, 0, sizeof(struct mem_device));

	/* Initialize device data buffer */
	satad.dd = (struct sata_devicedata *)
			zalloc_memory(sizeof(struct sata_devicedata));
	if (!satad.dd) {
		printf("Unable to alloc memery for sata device\n");
		return -1;
	}

	options			= 1;
	md->initialized		= 0;
	md->device_type		= satad.storage_device;
	md->xip_device		= 0;
	md->search_size		= 0;
	md->base_address	= 0;
	md->hs_toc_mask		= 0;
	md->gp_toc_mask		= 0;
	md->boot_options	= &options;
	md->device_data		= satad.dd;

	n = satad.sata_rom_data.io->init(md);
	if (n) {
		printf("sata->io->init failed\n");
		return n;
	}

	satad.dd->size = sata_get_size();
	satad.sector_size = sata_get_sector_size();

	/* Update sata initialized flag */
	satad.sata_initialized = 1;

	return 0;
}

static int sata_read(u64 start_sec, u64 sectors, void *data)
{
	struct read_desc rd;
	int n;
	u32 curr_count = 0;
	u32 start = (u32) start_sec;
	u32 to_read = (u32) sectors;
	u32 datap = (u32)data;

	if ((start_sec > 0xFFFFFFFF) ||
		(sectors > 0xFFFFFFFF)) {
		printf("sata_read failed. start_sec or sectors too large.\n");
		return -1;
	}

	while (to_read) {
		curr_count = (to_read > SATA_RW_SECTOR_LIMIT) ?
				SATA_RW_SECTOR_LIMIT : to_read;
		rd.sector_start	= start;
		rd.sector_count	= curr_count;
		rd.destination	= (void *)datap;

		n = satad.sata_rom_data.io->read(&satad.sata_rom_data.dread,
						&rd);
		if (n) {
			printf("sata_read failed\n");
			return n;
		}
		to_read -= curr_count;
		start += curr_count;
		datap += satad.sector_size * curr_count;
	}

	return 0;
}

static int sata_write(u64 start_sec, u64 sectors, void *data)
{
	/* TODO: Below are pointers into Embedded RAM
	 * to the command list and command table
	 * We only need to do these tricks as OMAP5 1.0 does
	 * not have native ROM support for sata write.
	 * This will be fixed in OMAP5 2.0 silicon.
	 * Also noticed that there is limitation of max
	 * 8192 sectors write with this API.
	 * Beyond that, we are stuck in the while loop
	 * for the OMAP_SATA_PXCI register status to change
	 */
	u32 * const cmdlist = (u32 *)0x4031d000;
	u32 * const cmdtbl = (u32 *)0x4031d800;
	u32 start = (u32)start_sec;
	u32 curr_count = 0;
	u32 to_write = (u32)sectors;
	u32 datap = (u32)data;

	if ((start_sec > 0xFFFFFFFF) ||
		(sectors > 0xFFFFFFFF)) {
		printf("sata_write failed. start_sec or sectors too large.\n");
		return -1;
	}

	while (to_write) {
		curr_count = (to_write > SATA_RW_SECTOR_LIMIT) ?
				SATA_RW_SECTOR_LIMIT : to_write;
		satad.rom_satall_buildreadsectorcmd(0, start,
						curr_count, datap);
		/* Set cmdlist[0].dw0.w bit */
		cmdlist[0] |= 1<<6;

		/* Clear cmd field and replace by ATA write */
		cmdtbl[0] &= ~0xFF0000;
		cmdtbl[0] |= 0x30<<16;

		/* Enable command list processing */
		sata_reg_write(OMAP_SATA_PXCMD_OFFSET,
			sata_reg_read(OMAP_SATA_PXCMD_OFFSET) | 1);

		/* Issue write command */
		sata_reg_write(OMAP_SATA_PXCI_OFFSET,
			sata_reg_read(OMAP_SATA_PXCI_OFFSET) | 1);

		/* Wait command completion */
		while (sata_reg_read(OMAP_SATA_PXCI_OFFSET) & 1)
			;
		/* TODO: check write result in controller */
		to_write -= curr_count;
		start += curr_count;
		datap += satad.sector_size * curr_count;
	}

	return 0;
}

static int get_sata_sector_size(void)
{
	return satad.sector_size;
}

static u64 get_sata_total_sectors(void)
{
	u64 total_sectors = (u64)satad.dd->size;
	return total_sectors;
}

struct storage_specific_functions *init_rom_sata_funcs(int proc_id, u8 device)
{
	if (device != DEVICE_SATA) {
		printf("Invalid Device\n");

		return NULL;
	}

	if (proc_id) {
		satad.rom_satall_buildreadsectorcmd =
			API(&rom_satall_buildreadsectorcmd_addr[proc_id]);
	} else
		return NULL;

	satad.sata_functions.init = sata_init;
	satad.sata_functions.get_sector_size = get_sata_sector_size;
	satad.sata_functions.read = sata_read;
	satad.sata_functions.write = sata_write;
	satad.sata_functions.get_total_sectors = get_sata_total_sectors;

	return &satad.sata_functions;
}
