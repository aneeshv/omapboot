/*
* Copyright (C) 2012 Texas Instruments, Inc.
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
#include <smartio.h>

static struct smartio smartio_default[] = {

	/* CONTROL_SMART1IO_PADCONF_0 */
	{CONTROL_SMART1IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << HSI2_DS), (IM_60_OHM << HSI2_DS)},
	{CONTROL_SMART1IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << EMMC_DS), (IM_60_OHM << EMMC_DS)},
	{CONTROL_SMART1IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << C2C_PART2_DS), (IM_45_OHM << C2C_PART2_DS)},
	{CONTROL_SMART1IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << C2C_PART1_DS), (IM_45_OHM << C2C_PART1_DS)},

	/* CONTROL_SMART1IO_PADCONF_1 */
	{CONTROL_SMART1IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << MCSPI2_DS), (IM_60_OHM << MCSPI2_DS)},

	/* CONTROL_SMART1IO_PADCONF_2 */
	{CONTROL_SMART1IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << C2C_PART4_DS), (IM_45_OHM << C2C_PART4_DS)},
	{CONTROL_SMART1IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << C2C_PART3_DS), (IM_45_OHM << C2C_PART3_DS)},
	{CONTROL_SMART1IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << WLSDIO_DS), (IM_60_OHM << WLSDIO_DS)},

	/* CONTROL_SMART2IO_PADCONF_0 */
	{CONTROL_SMART2IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << EMMC_SC), (SR_FAST << EMMC_SC)},
	{CONTROL_SMART2IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << C2C_PART2_SC), (SR_NA << C2C_PART2_SC)},
	{CONTROL_SMART2IO_PADCONF_0,
	(SMARTIO_FIELD_MASK << C2C_PART1_SC), (SR_NA << C2C_PART1_SC)},

	/* CONTROL_SMART2IO_PADCONF_1 */
	{CONTROL_SMART2IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << MCSPI2_SC), (SR_FAST << MCSPI2_SC)},
	{CONTROL_SMART2IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << HSI2_SC), (SR_FAST << HSI2_SC)},

	/* CONTROL_SMART2IO_PADCONF_2 */
	{CONTROL_SMART2IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << C2C_PART4_SC), (SR_NA << C2C_PART4_SC)},
	{CONTROL_SMART2IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << C2C_PART3_SC), (SR_NA << C2C_PART3_SC)},
	{CONTROL_SMART2IO_PADCONF_2,
	(SMARTIO_FIELD_MASK << WLSDIO_SC), (SR_FAST << WLSDIO_SC)},

	/* CONTROL_SMART3IO_PADCONF_0 */

	/* CONTROL_SMART3IO_PADCONF_1 */
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB1_I), (IM_60_OHM << USBB1_I)},
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB1_SR), (SR_FAST << USBB1_SR)},
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB2_I), (IM_60_OHM << USBB2_I)},
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB2_SR), (SR_FAST << USBB2_SR)},
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB3_I), (IM_60_OHM << USBB3_I)},
	{CONTROL_SMART3IO_PADCONF_1,
	(SMARTIO_FIELD_MASK << USBB3_SR), (SR_FAST << USBB3_SR)},

	{0, 0, 0},
};

void configure_smartio(struct smartio *smartio_config)
{

	struct smartio *smartio;

	if (smartio_config != NULL) {
		for (smartio = &smartio_config[0]; smartio->base > 0;
								smartio++)
			set_modify(smartio->base, smartio->mask,
							smartio->value);
	} else {
		for (smartio = &smartio_default[0]; smartio->base > 0;
								smartio++)
			set_modify(smartio->base, smartio->mask,
							smartio->value);
	}
}
