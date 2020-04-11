/**
 ****************************************************************************************
 *
 * @file temperature_i2c.c
 *
 * @brief I2C driver for Si7060  
 *
 * Copyright (C) 2015-2016 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef _TEMPERATURE_DRIVER_H
#define _TEMPERATURE_DRIVER_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <platform_devices.h>
#include <osal.h>
#include "ad_i2c.h"
#include "def.h"

void DoMeasurementTemperature(void);
void InitTemperatureSensorDriver(void);
STATIC void     TemperatureDriverTask(void *param);
STATIC int16_t  ReadTemperatureFromI2C(void);
STATIC void     ReadSensorRegisters( uint8_t *RegisterWithMSB, uint8_t *RegisterWithLSB);
STATIC int16_t  ConvertTemperatureFromRegisters(uint8_t RegisterMostSignificantByte,
                            uint8_t RegisterLessSignificantByte);

// 
// #include <time.h>
// #include <hw_i2c.h>
// #include <resmgmt.h>
// #include "common.h"
// 
// #include "hw_led.h"
// #include "hw_breath.h"
// #include "sys_power_mgr.h"
#endif  /* _TEMPERATURE_DRIVER_H*/
