#include "FakeTemperature_i2c.h"

void  FakeReadSensorRegisters( uint8_t *RegisterWithMSB, uint8_t *RegisterWithLSB)
{
    *RegisterWithMSB = 0xFF;
    *RegisterWithLSB = 0xAB;
}


