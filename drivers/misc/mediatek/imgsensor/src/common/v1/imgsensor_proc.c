// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include "imgsensor.h"
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/string.h>


#include "imgsensor_proc.h"

char mtk_ccm_name[camera_info_size] = { 0 };
char mtk_i2c_dump[camera_info_size] = { 0 };




static int pdaf_type_info_read(struct seq_file *m, void *v)
{
#define bufsz 512

	unsigned int len = bufsz;
	char pdaf_type_info[bufsz];

	struct SENSOR_FUNCTION_STRUCT *psensor_func =
	    pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_MAIN].pfunc;

	memset(pdaf_type_info, 0, 512);

	if (psensor_func == NULL)
		return 0;


	psensor_func->SensorFeatureControl(
	    SENSOR_FEATURE_GET_PDAF_TYPE,
	    pdaf_type_info,
	    &len);

	seq_printf(m, "%s\n", pdaf_type_info);
	return 0;
};

static int proc_SensorType_open(struct inode *inode, struct file *file)
{
	return single_open(file, pdaf_type_info_read, NULL);
};

static ssize_t proc_SensorType_write(
	struct file *file,
	const char *buffer,
	size_t count,
	loff_t *data)
{
	char regBuf[64] = { '\0' };
	u32 u4CopyBufSize =
	    (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

	struct SENSOR_FUNCTION_STRUCT *psensor_func =
	    pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_MAIN].pfunc;

	if (copy_from_user(regBuf, buffer, u4CopyBufSize))
		return -EFAULT;

	if (psensor_func)
		psensor_func->SensorFeatureControl(
		    SENSOR_FEATURE_SET_PDAF_TYPE,
		    regBuf,
		    &u4CopyBufSize);

	return count;
};

/************************************************************************
 * CAMERA_HW_Reg_Debug()
 * Used for sensor register read/write by proc file
 ************************************************************************/
static ssize_t CAMERA_HW_Reg_Debug(
	struct file *file,
	const char *buffer,
	size_t count,
	loff_t *data)
{
	char regBuf[64] = { '\0' };
	int ret = 0;
	u32 u4CopyBufSize =
	    (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

	struct IMGSENSOR_SENSOR *psensor =
	    &pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_MAIN];

	MSDK_SENSOR_REG_INFO_STRUCT sensorReg;

	memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

	if (psensor == NULL || copy_from_user(regBuf, buffer, u4CopyBufSize))
		return -EFAULT;

	if (sscanf(
	    regBuf,
	    "%x %x",
	    &sensorReg.RegAddr,
	    &sensorReg.RegData) == 2) {

		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_SET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		imgsensor_sensor_feature_control(
		    psensor, SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "write addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		ret = snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
		if (ret == 0) {
			pr_info("Error! snprintf allocate 0");
			ret = IMGSENSOR_RETURN_ERROR;
			return ret;
		}

	} else if (kstrtouint(regBuf, 16, &sensorReg.RegAddr) == 0) {
		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "read addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
	}
	return count;
}


static ssize_t CAMERA_HW_Reg_Debug2(
	struct file *file,
	const char *buffer,
	size_t count,
	loff_t *data)
{
	char regBuf[64] = { '\0' };
	int ret = 0;
	u32 u4CopyBufSize =
	    (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

	struct IMGSENSOR_SENSOR *psensor =
	    &pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_SUB];

	MSDK_SENSOR_REG_INFO_STRUCT sensorReg;

	memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

	if (psensor == NULL || copy_from_user(regBuf, buffer, u4CopyBufSize))
		return -EFAULT;

	if (sscanf(
	    regBuf,
	    "%x %x",
	    &sensorReg.RegAddr,
	    &sensorReg.RegData) == 2) {
		imgsensor_sensor_feature_control(
		    psensor, SENSOR_FEATURE_SET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		imgsensor_sensor_feature_control(
		    psensor, SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "write addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		ret = snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
		if (ret == 0) {
			pr_info("Error! snprintf allocate 0");
			ret = IMGSENSOR_RETURN_ERROR;
			return ret;
		}

	} else if (kstrtouint(regBuf, 16, &sensorReg.RegAddr) == 0) {
		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "read addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr, sensorReg.RegData);
	}

	return count;
}

static ssize_t CAMERA_HW_Reg_Debug3(
	struct file *file,
	const char *buffer,
	size_t count,
	loff_t *data)
{
	char regBuf[64] = { '\0' };
	int ret = 0;
	u32 u4CopyBufSize =
	    (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

	struct IMGSENSOR_SENSOR *psensor =
	    &pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_MAIN2];

	MSDK_SENSOR_REG_INFO_STRUCT sensorReg;

	memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

	if (psensor == NULL || copy_from_user(regBuf, buffer, u4CopyBufSize))
		return -EFAULT;

	if (sscanf(
	    regBuf,
	    "%x %x",
	    &sensorReg.RegAddr,
	    &sensorReg.RegData) == 2) {

		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_SET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		imgsensor_sensor_feature_control(
		    psensor, SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "write addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		ret = snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
		if (ret == 0) {
			pr_info("Error! snprintf allocate 0");
			ret = IMGSENSOR_RETURN_ERROR;
			return ret;
		}

	} else if (kstrtouint(regBuf, 16, &sensorReg.RegAddr) == 0) {
		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "read addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
	}

	return count;
}

static ssize_t CAMERA_HW_Reg_Debug4(
	struct file *file,
	const char *buffer,
	size_t count,
	loff_t *data)
{
	char regBuf[64] = { '\0' };
	int ret = 0;
	u32 u4CopyBufSize =
	    (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

	struct IMGSENSOR_SENSOR *psensor =
	    &pgimgsensor->sensor[IMGSENSOR_SENSOR_IDX_SUB2];

	MSDK_SENSOR_REG_INFO_STRUCT sensorReg;

	memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

	if (psensor == NULL || copy_from_user(regBuf, buffer, u4CopyBufSize))
		return -EFAULT;

	if (sscanf(
	    regBuf,
	    "%x %x",
	    &sensorReg.RegAddr,
	    &sensorReg.RegData) == 2) {

		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_SET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));


		pr_no_debug(
		    "write addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		ret = snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
		if (ret == 0) {
			pr_info("Error! snprintf allocate 0");
			ret = IMGSENSOR_RETURN_ERROR;
			return ret;
		}

	} else if (kstrtouint(regBuf, 16, &sensorReg.RegAddr) == 0) {
		imgsensor_sensor_feature_control(
		    psensor,
		    SENSOR_FEATURE_GET_REGISTER,
		    (MUINT8 *) &sensorReg,
		    (MUINT32 *) sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

		pr_no_debug(
		    "read addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);

		snprintf(
		    mtk_i2c_dump,
		    sizeof(mtk_i2c_dump),
		    "addr = 0x%08x, data = 0x%08x\n",
		    sensorReg.RegAddr,
		    sensorReg.RegData);
	}

	return count;
}


/* Camera information */
static int subsys_camera_info_read(struct seq_file *m, void *v)
{
	pr_no_debug("%s %s\n", __func__, mtk_ccm_name);
	seq_printf(m, "%s\n", mtk_ccm_name);
	return 0;
};
static int subsys_camsensor_read(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", mtk_i2c_dump);
	return 0;
};

static int proc_camera_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, subsys_camera_info_read, NULL);
};

static int proc_camsensor_open(struct inode *inode, struct file *file)
{
	return single_open(file, subsys_camsensor_read, NULL);
};

static int imgsensor_proc_status_read(struct seq_file *m, void *v)
{
	char status_info[IMGSENSOR_STATUS_INFO_LENGTH];
	int ret = 0;

	ret = snprintf(status_info, sizeof(status_info),
			"ERR_L0, %x\n",
			*((uint32_t *)(&pgimgsensor->status)));
	if (ret == 0) {
		pr_info("Error! snprintf allocate 0");
		ret = IMGSENSOR_RETURN_ERROR;
		return ret;
	}
	seq_printf(m, "%s\n", status_info);
	return 0;
};

static int imgsensor_proc_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, imgsensor_proc_status_read, NULL);
};

static const struct file_operations fcamera_proc_fops_status = {
	.owner = THIS_MODULE,
	.open = imgsensor_proc_status_open,
	.read = seq_read,
};

static const struct file_operations fcamera_proc_fops1 = {
	.owner = THIS_MODULE,
	.open = proc_camera_info_open,
	.read = seq_read,
};

static const struct file_operations fcamera_proc_fops = {
	.owner = THIS_MODULE,
	.read = seq_read,
	.open = proc_camsensor_open,
	.write = CAMERA_HW_Reg_Debug
};

static const struct file_operations fcamera_proc_fops2 = {
	.owner = THIS_MODULE,
	.read = seq_read,
	.open = proc_camsensor_open,
	.write = CAMERA_HW_Reg_Debug2
};

static const struct file_operations fcamera_proc_fops3 = {
	.owner = THIS_MODULE,
	.read = seq_read,
	.open = proc_camsensor_open,
	.write = CAMERA_HW_Reg_Debug3
};

static const struct file_operations fcamera_proc_fops4 = {
	.owner = THIS_MODULE,
	.read = seq_read,
	.open = proc_camsensor_open,
	.write = CAMERA_HW_Reg_Debug4
};


static const struct file_operations fcamera_proc_fops_set_pdaf_type = {
	.owner = THIS_MODULE,
	.open = proc_SensorType_open,
	.read = seq_read,
	.write = proc_SensorType_write
};



enum IMGSENSOR_RETURN imgsensor_proc_init(void)
{
	memset(mtk_ccm_name, 0, camera_info_size);

	proc_create("driver/camsensor", 0664, NULL, &fcamera_proc_fops);
	proc_create("driver/camsensor2", 0664, NULL, &fcamera_proc_fops2);
	proc_create("driver/camsensor3", 0664, NULL, &fcamera_proc_fops3);
	proc_create("driver/camsensor4", 0664, NULL, &fcamera_proc_fops4);
	proc_create(
	    "driver/pdaf_type", 0664, NULL, &fcamera_proc_fops_set_pdaf_type);
	proc_create(PROC_SENSOR_STAT, 0664, NULL, &fcamera_proc_fops_status);

	/* Camera information */
	proc_create(PROC_CAMERA_INFO, 0664, NULL, &fcamera_proc_fops1);

	return IMGSENSOR_RETURN_SUCCESS;
}
