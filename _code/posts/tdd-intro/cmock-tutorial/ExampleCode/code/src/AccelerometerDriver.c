/**
 ****************************************************************************************
 *
 * @file accelerometer_i2c.c
 *
 * @brief I2C driver for LSM303AH
 *
 * Copyright (C) 2015-2016 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <time.h>
// #include <osal.h>
// #include <platform_devices.h>
// #include <hw_i2c.h>
// #include <resmgmt.h>
// #include "common.h"
// #include "accelerometer_i2c.h"
// 
// #include "sys_power_mgr.h"
// #include "hw_led.h"
// #include "hw_breath.h"
// 
//
#include "AccelerometerDriver.h"

static const uint8_t LSM303_CTRL1_A      = 0x20;    // control reg 1
static const uint8_t LSM303_CTRL2_A      = 0x21;    // control reg 2
static const uint8_t LSM303_STATUS_A     = 0x27;    // status reg
static const uint8_t LSM303_WHO_AM_I_A   = 0x0F;    // ID register
static const uint8_t LSM303_ID_ACC       = 0x43;

/*  Axis output registers HSB/LSB     */
static const uint8_t LSM303_OUTX_L_A    =  0x28;
static const uint8_t LSM303_OUTX_H_A    =  0x29;
static const uint8_t LSM303_OUTY_L_A    =  0x2A;
static const uint8_t LSM303_OUTY_H_A    =  0x2B;
static const uint8_t LSM303_OUTZ_L_A    =  0x2C;
static const uint8_t LSM303_OUTZ_H_A    =  0x2D;

#define NOTIF_DO_MEASUREMENT            (1 << 1)
static OS_TASK handle = NULL;

// shared value between BLE service and I2C temperature task
extern __RETAINED_RW uint16_t CurrentAccelerometerValue;

#define ReadI2CRegister( Device, RegisterToRead, ReturnValue) \
            ad_i2c_transact( (Device), &(RegisterToRead), sizeof (RegisterToRead), \
                    &(ReturnValue), sizeof (ReturnValue) )


STATIC uint8_t GetDataReadyFlag(i2c_device dev)
{
    uint8_t _r;
    dev = ad_i2c_open(LSM303AH_ACC);
    ReadI2CRegister(dev, LSM303_STATUS_A , _r);
    ad_i2c_close(dev);

    return (_r & 0x01);
}

STATIC uint16_t ConcatenateBytes(uint8_t MostSignificantByte, uint8_t LessSignificantByte)
{
    return ((uint16_t)MostSignificantByte<< 8) | LessSignificantByte;

}

STATIC uint16_t UpdateAccelerometerValue(i2c_device dev)
{ 
//     uint16_t _outx_a;
//     uint16_t AccelerometerValue;
//     uint8_t MostSignificantByte;
//     uint8_t LessSignificantByte;
// 
//     dev = ad_i2c_open(LSM303AH_ACC);
// 
//     ad_i2c_transact(i2c_dev, 
//             &LSM303_OUTX_H_A, sizeof(LSM303_OUTX_H_A),  
//             &MostSignificantByte, sizeof(MostSignificantByte)
//             );
// 
//     ad_i2c_transact(i2c_dev, 
//             &LSM303_OUTX_L_A, sizeof(LSM303_OUTX_L_A), 
//             &LessSignificantByte, sizeof(LessSignificantByte)  
//             );
// 
//     ad_i2c_close(dev);
// 
//     _outx_a = ConcatenateBytes(MostSignificantByte, LessSignificantByte);
//     //     _outx_a = ((uint16_t)MostSignificantByte<< 8) | LessSignificantByte;
// 
//     AccelerometerValue = _outx_a>>4; // 1/16 is almost 1/0.061.. only for test
// 
//     CurrentAccelerometerValue = AccelerometerValue;
// 
//     return AccelerometerValue;
}


static void read_acc_i2c(void)
{
//     uint8_t _ret;
//     uint8_t _drdy; // data ready status
//     uint8_t _outx_l_a, _outx_h_a;
// 
//     uint16_t _outx_a;
// 
//     i2c_device i2c_dev;
// 
//     /* GetDataReadyFlag  */
// //     i2c_dev = ad_i2c_open(LSM303AH_ACC);
// //     ad_i2c_transact(i2c_dev, 
// //             &LSM303_STATUS_A, sizeof(LSM303_STATUS_A), 
// //             &_ret, sizeof(_ret) 
// //             );
// // 
// //     _drdy = _ret & 0x01;
// //
//     _drdy = GetDataReadyFlag(i2c_dev);
// 
//     /* if DataReady? then */
//     if( _drdy ){
//         /* Update Accelerometer Value  */
//         ad_i2c_transact(i2c_dev, 
//                 &LSM303_OUTX_L_A, sizeof(LSM303_OUTX_L_A), 
//                 &_outx_l_a, sizeof(_outx_l_a)  
//                 );
// 
//         ad_i2c_transact(i2c_dev, 
//                 &LSM303_OUTX_H_A, sizeof(LSM303_OUTX_H_A),  
//                 &_outx_h_a, sizeof(_outx_h_a)
//                 );
// 
//         _outx_a = ((uint16_t)_outx_h_a  << 8) | _outx_l_a;
// 
//         acc_int_val = _outx_a>>4; // 1/16 is almost 1/0.061.. only for test
//     }
// 
//     ad_i2c_close(i2c_dev);
}

static void i2c_acc_task(void *param)
{
//     for (;;) {
//         OS_BASE_TYPE ret;
//         uint32_t notif;
// 
//         ret = OS_TASK_NOTIFY_WAIT(0, (uint32_t) -1, &notif, OS_TASK_NOTIFY_FOREVER);
//         OS_ASSERT(ret == OS_OK);
// 
//         if (notif & NOTIF_DO_MEASUREMENT) {
//             read_acc_i2c();
//         }
//     }
}

void i2c_acc_do_measurement(void)
{
//     OS_TASK_NOTIFY(handle, NOTIF_DO_MEASUREMENT, OS_NOTIFY_SET_BITS);
}
// 
void i2c_acc_init(void)
{
//     // Configure sensor in Low Power at 100Hz
//     static const uint8_t conf_reg = 0xC0;
//     static const uint8_t rst_reg= 0x40; 
//     uint8_t dev_id = 0x00; 
// 
// 
//     /* IsValidAccelerometerID */ 
//     i2c_device i2c_dev;
//     i2c_dev = ad_i2c_open(LSM303AH_ACC);
//     ad_i2c_transact(i2c_dev, &LSM303_WHO_AM_I_A, sizeof(LSM303_WHO_AM_I_A),
//             &dev_id, sizeof(dev_id));
// 
//     if (dev_id != LSM303_ID_ACC) 
//     {        
//         ad_i2c_close(i2c_dev);
//         while(1);
//     }
// 
//     /* End IsValidAccelerometerID */
// 
//     /* ConfigAccelerometer */
//     // Concatenation REG_ADDR + REG_VAL
//     const uint8_t conf[] = {LSM303_CTRL1_A, conf_reg};
//     const uint8_t reset[] = {LSM303_CTRL2_A, rst_reg};
// 
//     /* dev_id is just a dummy var, used to generate a I2C writing 
//        frame. Without dev_id and the concatenation the API
//        does not generate the I2C writting pattern */
//     ad_i2c_transact(i2c_dev, reset, sizeof(reset), &dev_id, sizeof(dev_id));
// 
//     ad_i2c_transact(i2c_dev, conf, sizeof(conf), &dev_id, sizeof(dev_id));
//     ad_i2c_transact(i2c_dev, &LSM303_CTRL1_A, sizeof(LSM303_CTRL1_A), &dev_id, sizeof(dev_id));
// 
//     ad_i2c_close(i2c_dev);
// 
//     /* End ConfigAccelerometer */
// 
//     OS_TASK_CREATE("acc_sensor", i2c_acc_task, NULL, 400, OS_TASK_PRIORITY_NORMAL, handle);
}
