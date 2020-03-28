#include "unity.h"
#include "LedDriver.h"

void setUp(void) { }

void tearDown(void) { }

void test_square(void)
{
    int16_t x = 1000;
    int32_t y = 0;
    y = square_x(x);
    TEST_ASSERT_EQUAL_INT32(1000000,y);

}

void test_square32(void)
{
    int16_t x = 1000;
    int32_t y = 0;
    y = square_x(x);
    TEST_ASSERT_EQUAL_INT32(1000000,y);
}
