#include "unity.h"
#include "cmock.h"
#include "mock_ad_i2c.h"
#include "mock_osal.h"
#include "mock_platform_devices.h"
#include "AccelerometerDriver.h"

// Just to satisfy the linker, never used in testing
uint16_t CurrentAccelerometerValue = 0;

void setUp(void)
{
}

void tearDown()
{
}

void test_ReadDataReadyRegister(void)
{
    const uint8_t dummyInput;

    uint16_t dev = 0 ;
    uint8_t  StatusRegister = 0xFF;
    uint8_t ReadyFlag;

    ad_i2c_open_ExpectAndReturn(LSM303AH_ACC, dev);

    ad_i2c_transact_Expect(dev, &dummyInput, sizeof(dummyInput), 
            &StatusRegister, sizeof(StatusRegister));
    ad_i2c_transact_IgnoreArg_reg();
    ad_i2c_transact_IgnoreArg_res();
    ad_i2c_transact_ReturnThruPtr_res(&StatusRegister);

    ad_i2c_close_Expect(dev);
    ad_i2c_close_Ignore();

    ReadyFlag = GetDataReadyFlag(dev);

    TEST_ASSERT_EQUAL_HEX8(0x01, ReadyFlag );
}

void test_ConcatenateBytes(void)
{
    uint8_t MostSignificantByte = 0xDE;
    uint8_t LessSignificantByte = 0xAD;

    uint16_t Result = 0;

    Result = ConcatenateBytes( MostSignificantByte, LessSignificantByte);

    TEST_ASSERT_EQUAL_HEX16( 0xDEAD, Result);
}
