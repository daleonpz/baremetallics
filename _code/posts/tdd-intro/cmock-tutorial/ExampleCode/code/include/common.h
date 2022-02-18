/**
 ****************************************************************************************
 *
 * @file common.h
 *
 * @brief Common definitions for applications
 *
 * Copyright (C) 2015-2016 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef COMMON_H_
#define COMMON_H_ 

#include <stdbool.h>

/**
 * \brief Setup peripherals used in demo application
 *
 */
void periph_setup(void);

void i2c_temp_init(void);
void i2c_acc_init(void);


#endif /* COMMON_H_ */
