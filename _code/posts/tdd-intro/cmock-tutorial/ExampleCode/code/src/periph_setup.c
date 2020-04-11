/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup for application
 *
 * Copyright (C) 2015-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <hw_gpio.h>
#include <platform_devices.h>
#include "common.h"


/* GPIO pins configuration array, see periph_setup() for details */
static const gpio_config gpio_cfg[] = {
        /* I2C */
        // The system is set to [Master], so it outputs the clock signal
        HW_GPIO_PINCONFIG(HW_GPIO_PORT_3, HW_GPIO_PIN_1, OUTPUT_PUSH_PULL, I2C_SCL, true),

        // Bidirectional signal both for sending and receiving data
        HW_GPIO_PINCONFIG(HW_GPIO_PORT_3, HW_GPIO_PIN_2, INPUT_PULLUP,  I2C_SDA, true),
        HW_GPIO_PINCONFIG_END 
};

void i2c_init(void)
{
        /*
         * Initialize I2C controller in master mode with standard communication speed (100 kb/s) and
         * transfer in 7-bit addressing mode.
         */
        static const i2c_config cfg = {
                .speed = HW_I2C_SPEED_STANDARD,
                .mode = HW_I2C_MODE_SLAVE,
                .addr_mode = HW_I2C_ADDRESSING_7B,
        };

        hw_i2c_init(HW_I2C1, &cfg);

        srand(OS_GET_TICK_COUNT());
}


void periph_setup(void)
{
        /*
         * In most cases application will configure a lot of pins, e.g. during initialization. In
         * such case it's usually convenient to use some predefined configuration of pins. This can
         * be described using array of \p gpio_config structures where each element describes single
         * pin configuration and it can be easily created using \p HW_GPIO_PINCONFIG macro. Since
         * array can have variable number of elements it should be terminated by special entry
         * which can be inserted using \p HW_GPIO_PINCONFIG_END macro. See \p gpio_cfg definition
         * above for an example.
         */
        hw_gpio_configure(gpio_cfg);

//         i2c_init();
}
