#include "unity.h"
#include "cmock.h"
#include "mock_ad_i2c.h"
#include "mock_osal.h"
#include "mock_platform_devices.h"
#include "TemperatureDriver.h"
#include "FakeTemperature_i2c.h"

// Just to satisfy the linker, never used in testing
int16_t CurrentTemperatureValue = 0;

void setUp(void)
{
}

void tearDown()
{
}

void test_ReadTemperatureSensorRegister(void)
{
    uint16_t Temperature = 0;
    const uint8_t msByte, lsByte;

    uint16_t dev = 1;
    uint8_t e_m = 0xFF;
    uint8_t e_l = 0x80;
    uint8_t _ptr = 10;
  
    ad_i2c_open_ExpectAndReturn(1, dev);
   
    ad_i2c_transact_Expect(dev, &msByte, sizeof(msByte), &e_m, sizeof(e_m));
    ad_i2c_transact_IgnoreArg_reg();
    ad_i2c_transact_IgnoreArg_res();
    ad_i2c_transact_ReturnThruPtr_res(&e_m);
    
    ad_i2c_transact_Expect(dev, &msByte, sizeof(msByte), &e_l, sizeof(e_l));
    ad_i2c_transact_IgnoreArg_reg();
    ad_i2c_transact_IgnoreArg_res();
    ad_i2c_transact_ReturnThruPtr_res(&e_l);

    ad_i2c_close_Expect(dev);

    ReadSensorRegisters(&e_m, &e_l);
    
    TEST_ASSERT_EQUAL_HEX8(0x7F, e_m );
    TEST_ASSERT_EQUAL_HEX8(0x80, e_l);
}

void test_ConvertTemperatureFromRegisters(void)
{
    uint8_t RegisterMostSignificantByte = 50;
    uint8_t RegisterLessSignificantByte = 100;
    int16_t temperature;

    temperature = ConvertTemperatureFromRegisters(RegisterMostSignificantByte,
                        RegisterLessSignificantByte);

    TEST_ASSERT_EQUAL_HEX16(34, temperature);
}


void test_ConvertToUnderZeroTemperatureFromRegisters(void)
{
    uint8_t RegisterMostSignificantByte = 10;
    uint8_t RegisterLessSignificantByte = 50;
    int16_t temperature;

    temperature = ConvertTemperatureFromRegisters(RegisterMostSignificantByte,
                        RegisterLessSignificantByte);

    TEST_ASSERT_EQUAL_HEX16(-31, temperature);
}

void test_ConvertTemperatureFromRegistersWithExtremeValues(void)
{
    uint8_t RegisterMostSignificantByte = 0xFF;
    uint8_t RegisterLessSignificantByte = 0xFF;
    int16_t temperature;

    temperature = ConvertTemperatureFromRegisters(RegisterMostSignificantByte,
                        RegisterLessSignificantByte);

    TEST_ASSERT_EQUAL_HEX16(362, temperature);
}


