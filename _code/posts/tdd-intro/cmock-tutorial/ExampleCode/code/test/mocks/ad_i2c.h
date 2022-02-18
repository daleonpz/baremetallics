#ifndef _AD_I2C_H
#define _AD_I2C_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint16_t i2c_device;

i2c_device ad_i2c_open(i2c_device dev);
void ad_i2c_transact(i2c_device dev, const uint8_t *reg, size_t reg_size, uint8_t *res, size_t res_size );
void ad_i2c_close(i2c_device dev);


#endif  /* _AD_I2C_H*/
