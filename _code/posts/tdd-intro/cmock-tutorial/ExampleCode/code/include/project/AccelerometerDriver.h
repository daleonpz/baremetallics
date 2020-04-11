/**
 ****************************************************************************************
 *
 * @file accelerometer_i2c.h
 *
 * @brief I2C driver for Si7060  
 *
 * Copyright (C) 2015-2016 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef _ACCELEROMETER_DRIVER_H
#define _ACCELEROMETER_DRIVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <time.h>
#include <osal.h>
// #include <hw_i2c.h>
// #include <resmgmt.h>
// #include "common.h"

#include "ad_i2c.h"
#include <platform_devices.h>
#include "def.h"

// #include "hw_led.h"
// #include "hw_breath.h"
// #include "sys_power_mgr.h"

void i2c_acc_do_measurement(void);
void i2c_acc_init(void);

STATIC uint8_t GetDataReadyFlag(i2c_device dev);
STATIC uint16_t UpdateAccelerometerValue(i2c_device dev);
STATIC uint16_t ConcatenateBytes(uint8_t MostSignificantByte, uint8_t LessSignificantByte);

#endif /* _ACCELEROMETER_DRIVER_H_ */
