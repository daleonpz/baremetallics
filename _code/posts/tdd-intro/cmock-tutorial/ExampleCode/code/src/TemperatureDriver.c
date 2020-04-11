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
#include "TemperatureDriver.h"

static const uint8_t SI7060_ID           = 0xC0    ;// chipID | revID
static const uint8_t SI7060_DSPSIGM      = 0xC1    ;// most significant bits temperature conversion
static const uint8_t SI7060_DSPSIGL      = 0xC2    ;// least significant bits temperature conversion

static OS_TASK handle = NULL;

#define NOTIF_DO_MEASUREMENT            (1 << 1)

extern __RETAINED_RW int16_t CurrentTemperatureValue;

STATIC int16_t ConvertTemperatureFromRegisters(uint8_t RegisterMostSignificantByte,
                uint8_t RegisterLessSignificantByte){

    int16_t Temperature = 0;
    
    Temperature =  55 + ( 256* (int16_t)(RegisterMostSignificantByte)  +
                            (int16_t)(RegisterLessSignificantByte) -
                            16384 
                            ) /160;
    return Temperature;
}

STATIC void ReadSensorRegisters( uint8_t *RegisterWithMSB, uint8_t *RegisterWithLSB)
{
    uint8_t _ret;
    i2c_device i2c_dev;
    i2c_dev = ad_i2c_open(SI7060);

    ad_i2c_transact(i2c_dev, &SI7060_DSPSIGM, sizeof(SI7060_DSPSIGM), &_ret, sizeof(_ret) );
    *RegisterWithMSB = (_ret&0x7F); // [6:0]bits are the conversion result
    
    ad_i2c_transact(i2c_dev, &SI7060_DSPSIGL, sizeof(SI7060_DSPSIGL), &_ret, sizeof(_ret) );
    *RegisterWithLSB = _ret;

    ad_i2c_close(i2c_dev);
}

STATIC int16_t ReadTemperatureFromI2C(void)
{
    uint8_t _dspsigm, _dspsigl;
    int16_t  Temperature;

    ReadSensorRegisters( &_dspsigm, &_dspsigl); 
    Temperature = ConvertTemperatureFromRegisters(_dspsigm, _dspsigl);
    
    CurrentTemperatureValue = Temperature;    
    return Temperature; 
}

STATIC void TemperatureDriverTask(void *param)
{
    for (;;) {
        OS_BASE_TYPE ret;
        uint32_t notif;

        ret = OS_TASK_NOTIFY_WAIT(0, (uint32_t) -1, &notif, OS_TASK_NOTIFY_FOREVER);
        OS_ASSERT(ret == OS_OK);

        if (notif & NOTIF_DO_MEASUREMENT) {
            ReadTemperatureFromI2C();
        }
    }
}

void DoMeasurementTemperature(void)
{
    OS_TASK_NOTIFY(handle, NOTIF_DO_MEASUREMENT, OS_NOTIFY_SET_BITS);
}

void InitTemperatureSensorDriver(void)
{
    /*
     * Set Si7060 with initial values which are at the same time default values of configuration
     * register of the sensor after power up.
     */

    OS_TASK_CREATE("temp_sensor", TemperatureDriverTask, NULL, 400, OS_TASK_PRIORITY_NORMAL, handle);
}
