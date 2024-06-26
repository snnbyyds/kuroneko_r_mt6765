/*
 * Copyright (C) 2018 MediaTek Inc.
 * Copyright (C) 2021 XiaoMi, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */
#include <linux/kernel.h>
#include "cam_cal_list.h"
#include "eeprom_i2c_common_driver.h"
#include "eeprom_i2c_custom_driver.h"
#include "kd_imgsensor.h"

struct stCAM_CAL_LIST_STRUCT g_camCalList[] = {
	/*Below is commom sensor */
	{HYNIX_HI1337_I_SENSOR_ID, 0xA8, Common_read_region},
	{HYNIX_HI1337_II_SENSOR_ID, 0xB0, Common_read_region},
	{HYNIX_HI1337_III_SENSOR_ID, 0xA8, Common_read_region},
	{GC_GC5035_I_SENSOR_ID, 0xA2, Common_read_region},
	{GC_GC5035_II_SENSOR_ID, 0xA2, Common_read_region},
	{GC_GC5035_III_SENSOR_ID, 0xA2, Common_read_region},
	/*  ADD before this line */
	{0, 0, 0}       /*end of list */
};

unsigned int cam_cal_get_sensor_list(
	struct stCAM_CAL_LIST_STRUCT **ppCamcalList)
{
	if (ppCamcalList == NULL)
		return 1;

	*ppCamcalList = &g_camCalList[0];
	return 0;
}


