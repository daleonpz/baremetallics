#include "build/temp/_test_LedDriver.c"
#include "include/LedDriver.h"
#include "/var/lib/gems/2.5.0/gems/ceedling-0.29.1/vendor/unity/src/unity.h"


void setUp(void) { }



void tearDown(void) { }



void test_square(void)

{

    int16_t x = 1000;

    int32_t y = 0;

    y = square_x(x);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((1000000)), (UNITY_INT)(UNITY_INT32)((y)), (

   ((void *)0)

   ), (UNITY_UINT)(13), UNITY_DISPLAY_STYLE_INT32);



}



void test_square32(void)

{

    int16_t x = 1000;

    int32_t y = 0;

    y = square_x(x);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((1000000)), (UNITY_INT)(UNITY_INT32)((y)), (

   ((void *)0)

   ), (UNITY_UINT)(22), UNITY_DISPLAY_STYLE_INT32);

}
