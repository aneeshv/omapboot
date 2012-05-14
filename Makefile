##
## Copyright (C) 2010 The Android Open Source Project
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in
##    the documentation and/or other materials provided with the 
##    distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
## COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
## BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
## OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
## AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
## OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.

VERSION = 1
PATCHLEVEL = 1
SUBLEVEL = 0
ORGANIZATION="Texas Instruments Inc"
VERSION_FILE = include/version.h
ABOOT_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

what_to_build:: all

-include local.mk

ifeq ("$(origin V)", "command line")
  VERBOSE = $(V)
endif
ifndef VERBOSE
  VERBOSE = 0
endif
QUIET := $(if $(VERBOSE:1=),@)

#if TOOLCHAIN is defined, override CROSS_COMPILE, else use CROSS_COMPILE
ifneq ("$(TOOLCHAIN)", "")
	CROSS_COMPILE := $(TOOLCHAIN)
else
	CROSS_COMPILE ?= arm-eabi-
endif

BOARD ?= panda
ARCH ?= arm
MACH ?= omap4
EXTRAOPTS ?= -m32

TARGET_CC := $(CROSS_COMPILE)gcc
TARGET_LD := $(CROSS_COMPILE)ld
TARGET_OBJCOPY := $(CROSS_COMPILE)objcopy
TARGET_OBJDUMP := $(CROSS_COMPILE)objdump

$(shell cat arch/$(MACH)/configs/config_$(BOARD).h > include/config.h)
TARGET_FLAGS := -g -Os  -Wall
TARGET_FLAGS += -fno-builtin -ffreestanding
TARGET_FLAGS += -I. -Iinclude
TARGET_FLAGS += -include include/config.h

# Newer GCC require +sec postfix to build secure instructions
plus_sec := $(shell set -e;						\
		if (echo ".arch_extension sec" |			\
		$(TARGET_CC) -c -x assembler -)>/dev/null 2>/dev/null;	\
		then echo "+sec";					\
		else echo "";						\
		fi)
TARGET_AFLAGS += $(TARGET_FLAGS) -Wa,-march=armv7-a$(plus_sec)
TARGET_CFLAGS += $(TARGET_FLAGS) -march=armv7-a

TARGET_LIBGCC := $(shell $(TARGET_CC) $(TARGET_CFLAGS) -print-libgcc-file-name)

HOST_CFLAGS := -g -O2 -Wall $(EXTRAOPTS)
HOST_CFLAGS += -Itools
HOST_CFLAGS += -include include/config.h

OUT := out/$(BOARD)
OUT_HOST_OBJ := $(OUT)/host-obj
OUT_TARGET_OBJ := $(OUT)/target-obj

ALL :=

# Build the usbboot host tool
include build/rules.mk
include tools/host_usbboot.mk

# Build the target with it's dependencies
include arch/$(MACH)/board/$(BOARD).mk

COMMON_OBJS := 	crc32.o \
		libc/utils.o \
		fastboot.o \
		fastboot_mmc.o \
		libc/printf.o \
		libc/raise.o \
		libc/string.o \
		trusted.o \
		boot.o \
		misc.o \

M_NAME := aboot
M_LDS :=  arch/$(MACH)/$(M_NAME).lds
M_BASE := $(ABOOT_TEXT_BASE)
M_OBJS := arch/common/start_reloc.o
M_OBJS += $(OMAP_COMMON_OBJS)
M_OBJS += $(COMMON_OBJS)
M_OBJS += $(PROC_COMMON_OBJS)
M_OBJS += $(BOARD_OBJS)
M_OBJS += aboot.o
M_OBJS += booti.o
M_OBJS += device_tree.o
M_LIBS := $(TARGET_LIBGCC)
include build/target-executable.mk

M_NAME := iboot
M_BASE := $(IBOOT_TEXT_BASE)
M_OBJS := iboot/start_reloc.o
M_OBJS += $(OMAP_COMMON_OBJS)
M_OBJS += $(COMMON_OBJS)
M_OBJS += $(PROC_COMMON_OBJS)
M_OBJS += $(BOARD_OBJS)
M_OBJS += booti.o
M_OBJS += device_tree.o
M_OBJS += iboot/iboot.o
M_OBJS += pmic.o
M_LIBS := $(TARGET_LIBGCC)
include build/target-executable.mk

M_NAME := eboot
M_LDS :=  arch/$(MACH)/$(M_NAME).lds
M_BASE := $(EBOOT_TEXT_BASE)
M_OBJS := eboot/start_reloc.o
M_OBJS += $(OMAP_COMMON_OBJS)
M_OBJS += $(COMMON_OBJS)
M_OBJS += $(PROC_COMMON_OBJS)
M_OBJS += $(BOARD_OBJS)
M_OBJS += booti.o
M_OBJS += eboot/eboot.o
M_OBJS += pmic.o
M_OBJS += device_tree.o
M_LIBS := $(TARGET_LIBGCC)
include build/target-executable.mk

# Override this with non 0 value to make MAKEALL continue build ignoring
# Failures - default behavior is to quit on detecting error
MAKEALL_LENIENT ?= 0

.EXPORT_ALL_VARIABLES:

$(OUT)/eboot.ift: $(OUT)/eboot.bin $(OUT)/mkheader
	@echo generate $@
	$(QUIET)./$(OUT)/mkheader $(EBOOT_TEXT_BASE) `wc -c $(OUT)/eboot.bin` add_gp_hdr > $@
	$(QUIET)cat $(OUT)/eboot.bin >> $@
	@echo Renaming eboot.ift to MLO ...Done!
	$(QUIET)cp $(OUT)/eboot.ift $(OUT)/MLO

$(OUT)/iboot.ift: $(OUT)/iboot.bin $(OUT)/mkheader
	@echo generate $@
	$(QUIET)./$(OUT)/mkheader $(IBOOT_TEXT_BASE) `wc -c $(OUT)/iboot.bin` no_gp_hdr > $@
	$(QUIET)cat $(OUT)/iboot.bin >> $@

$(OUT)/aboot.ift: $(OUT)/aboot.bin $(OUT)/mkheader
	@echo generate $@
	$(QUIET)./$(OUT)/mkheader $(ABOOT_TEXT_BASE) `wc -c $(OUT)/aboot.bin` no_gp_hdr > $@
	$(QUIET)cat $(OUT)/aboot.bin >> $@

ALL += $(OUT)/aboot.ift $(OUT)/iboot.ift $(OUT)/eboot.ift

$(OUT_HOST_OBJ)/2ndstage.o: $(OUT)/aboot.bin $(OUT)/bin2c $(OUT)/mkheader
	@echo generate $@
	$(QUIET)./$(OUT)/mkheader $(ABOOT_TEXT_BASE) `wc -c $(OUT)/aboot.bin` no_gp_hdr > $@
	$(QUIET)cat $(OUT)/aboot.bin >> $@
	$(QUIET)./$(OUT)/bin2c aboot < $@ > $(OUT)/2ndstage.c
	$(QUIET)gcc -c $(EXTRAOPTS) -o $@ $(OUT)/2ndstage.c

$(OUT_HOST_OBJ)/secondstage.o: $(OUT)/iboot.bin $(OUT)/bin2c $(OUT)/mkheader
	@echo generate $@
	$(QUIET)./$(OUT)/mkheader $(IBOOT_TEXT_BASE) `wc -c $(OUT)/iboot.bin` no_gp_hdr > $@
	$(QUIET)cat $(OUT)/iboot.bin >> $@
	$(QUIET)./$(OUT)/bin2c iboot < $@ > $(OUT)/secondstage.c
	$(QUIET)gcc -c $(EXTRAOPTS) -o $@ $(OUT)/secondstage.c

_clean_generic::
	$(QUIET)rm -f include/config.h
	$(QUIET)rm -f include/version.h
	$(QUIET)rm -rf $(OUT)

clean::
	@echo clean
	$(QUIET)$(MAKE) _clean_generic

distclean::
	@echo distclean
	$(QUIET)$(MAKE) _clean_generic
	$(QUIET)find . -iname "*~" -o -iname "*.d" -o -iname "*.rej" -o \
		-iname "*.orig" |\
		xargs rm -f tags

.PHONY:	tags

all:: version $(ALL)

version:
	$(QUIET)echo -n "#define ABOOT_VERSION \""$(ORGANIZATION)" Bootloader " > $(VERSION_FILE); \
	echo -n "$(ABOOT_VERSION)" >> $(VERSION_FILE); \
	echo -n $(shell $(CONFIG_SHELL) build/getgitinfo \
		 $(TOPDIR)) >> $(VERSION_FILE); \
		echo "\"" >> $(VERSION_FILE)

tags:
	@echo "Generating Tags"
	$(QUIET)find . -type f -iname \*.h -o -iname \*.c -o -iname \*.S | \
		xargs ctags

MAKEALL:
	@echo "checking available build platforms"
	$(QUIET)TMP_FILE="/tmp/make-all.$$$$.tmp";			\
		TMP_FILE2="/tmp/make-all-res.$$$$.tmp";			\
		ls arch/*/configs/*.h|cut -d '.' -f1 |			\
		sed -e "s/arch\//MACH=/g" |				\
		sed -e "s/\/configs\/config_/ BOARD=/g">"$$TMP_FILE" &&	\
		while read config; do 					\
		echo "Building $$config"; $(MAKE) $$config;R="$$?";	\
		if [ $$R -eq 0 ]; then					\
			echo "$$config : BUILD OK" >> "$$TMP_FILE2";	\
		else							\
			echo "$$config : BUILD FAIL">> "$$TMP_FILE2";	\
			if [ $(MAKEALL_LENIENT) -eq 0 ]; then		\
				cat "$$TMP_FILE2";			\
				echo "Further Builds stopped!";		\
				echo "use MAKEALL_LENIENT=1 to go on";	\
				rm -f "$$TMP_FILE" "$$TMP_FILE2";	\
				exit 1;					\
			fi;						\
		fi;							\
		done <"$$TMP_FILE";					\
		echo "Build with configs:";cat "$$TMP_FILE2";		\
		rm -f "$$TMP_FILE" "$$TMP_FILE2"

# we generate .d as a side-effect of compiling. override generic rule:
%.d:
-include $(DEPS)
