#ifndef _FAKE_TEMPERATURE_I2C_H_ 
#define _FAKE_TEMPERATURE_I2C_H_

#include <stdint.h>

void  FakeReadSensorRegisters( uint8_t *RegisterWithMSB, uint8_t *RegisterWithLSB);


#endif 
