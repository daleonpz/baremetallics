/**
 ****************************************************************************************
 *
 * @file ble_peripheral_task.c
 *
 * @brief BLE peripheral task
 *
 * Copyright (C) 2015-2018 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "ble_peripheral_config.h"
#if CFG_DEBUG_SERVICE
#include "dlg_debug.h"
#endif /* CFG_DEBUG_SERVICE */
#include "bas.h"
#include "cts.h"
#include "dis.h"
#include "scps.h"
#include "sys_power_mgr.h"
#include "hw_breath.h"
#include "hw_gpio.h"
#include "hw_led.h"
#include "ad_i2c.h"
#include <platform_devices.h>

#include "common.h"

#include "sensors_service.h"

/*
 * Notification bits reservation
 * bit #0 is always assigned to BLE event queue notification
 */
#define TEMP_MEAS_TIMER_NOTIF       (1 << 2)
#define TEMP_SENSOR_NOTIF           (1 << 3)
#define ACC_MEAS_TIMER_NOTIF        (1 << 4)
#define ACC_SENSOR_NOTIF            (1 << 5)

/*
 * BLE peripheral advertising data
 */
static const uint8_t adv_data[] = {
        0x0E, GAP_DATA_TYPE_LOCAL_NAME,
        'D', 'i', 'a', 'l', 'o', 'g', ' ', 'C','u','s','t','o','m'
};

/* Task used by application */
static OS_TASK ble_peripheral_task_handle;

/* Timer used to read periodical measurements */
PRIVILEGED_DATA static OS_TIMER temp_meas_timer;
PRIVILEGED_DATA static OS_TIMER acc_meas_timer;

static void notif_timer_cb(OS_TIMER timer)
{
        uint32_t notif = OS_PTR_TO_UINT(OS_TIMER_GET_TIMER_ID(timer));

        OS_TASK_NOTIFY(ble_peripheral_task_handle, notif, OS_NOTIFY_SET_BITS);
}


static void set_alerting(bool new_alerting)
{
        PRIVILEGED_DATA static bool alerting = false;

        /*
         * Breath timer used for alerting does not work in sleep mode thus device cannot go to sleep
         * if it's alerting. Also pm_stay_alive() and pm_resume_sleep() has to be always called the
         * same number of times so this adds required logic to do this.
         */

        if (new_alerting == alerting) {
                return;
        }

        alerting = new_alerting;

        if (alerting) {
                pm_stay_alive();
        } else {
                pm_resume_sleep();
        }
}

/* Configure alerting for given level */
static void do_alert(uint8_t level)
{

        breath_config config = {
                .dc_min = 0,
                .dc_max = 255,
                .freq_div = 255,
                .polarity = HW_BREATH_PWM_POL_POS
        };

        switch (level) {
        case 1:
                set_alerting(true);
                config.dc_step = 96;
                break;
        case 2:
                set_alerting(true);
                config.dc_step = 32;
                break;
        default:
                set_alerting(false);
            /* Simply disable breath timer and return */
                hw_breath_disable();
                return;
        }

        /* Configure and enable breath timer */
        hw_breath_init(&config);
        hw_led_set_led1_src(HW_LED_SRC2_BREATH);
        hw_led_enable_led1(true);
        hw_breath_enable();
}

/*
 * Main code
 */
static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        /**
         * Manage connection information
         */
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
        // restart advertising so we can connect again
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void setup_timers(void)
{
        /* Create timer for Temperature Sensor (TS) to send periodic measurements 2 seg*/
        temp_meas_timer = OS_TIMER_CREATE("temp_meas", 2*1000, OS_TIMER_SUCCESS,
                                                        OS_UINT_TO_PTR(TEMP_MEAS_TIMER_NOTIF),
                                                                                notif_timer_cb);

        OS_ASSERT(temp_meas_timer);
        OS_TIMER_START(temp_meas_timer, OS_TIMER_FOREVER);

        /* Create timer for accelerometer (CS) to send periodic measurements 1 seg*/
        acc_meas_timer = OS_TIMER_CREATE("acc_meas", 1*1000, OS_TIMER_SUCCESS,
                                                        OS_UINT_TO_PTR(ACC_MEAS_TIMER_NOTIF),
                                                                                notif_timer_cb);
        OS_ASSERT(acc_meas_timer);
        OS_TIMER_START(acc_meas_timer, OS_TIMER_FOREVER);
}

/* LED D2 status flag */
__RETAINED_RW volatile bool pin_status_flag = 0;

/* Characteristic value */
__RETAINED_RW int16_t CurrentTemperatureValue = 0;
__RETAINED_RW uint16_t acc_int_val = 0;

/* Handle of custom BLE service */
__RETAINED_RW ble_service_t *ss = NULL;

/* Handler for read requests */
static void temp_get_int_val_cb(ble_service_t *svc, uint16_t conn_idx)
{
        uint16_t var_value = CurrentTemperatureValue;

        /* Send the requested data to the peer device.  */
        temp_get_int_value_cfm(svc, conn_idx, ATT_ERROR_OK, &var_value);
}

static void acc_get_int_val_cb(ble_service_t *svc, uint16_t conn_idx)
{
        uint16_t var_value = acc_int_val;

        /* Send the requested data to the peer device.  */
        acc_get_int_value_cfm(svc, conn_idx, ATT_ERROR_OK, &var_value);
}


/* Declare callback functions for specific BLE events */
static const sensors_service_cb_t ss_callbacks = {
        .temp_get_characteristic_value = temp_get_int_val_cb,
        .acc_get_characteristic_value = acc_get_int_val_cb,
};

void ble_peripheral_task(void *params)
{
        int8_t wdog_id;
        ble_service_t *svc;

        // in case services which do not use svc are all disabled, just surpress -Wunused-variable
        (void) svc;

        /* register ble_peripheral task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /* Store application task handle for task notifications */
        ble_peripheral_task_handle = OS_GET_CURRENT_TASK();

        srand(time(NULL));

        /* Start BLE device as peripheral */
        ble_peripheral_start();
     
        /* Register task to BLE framework to receive BLE event notifications */
        ble_register_app();

        /* Set device name, advertising response data and IO capabilities */
        ble_gap_device_name_set("Dialog Custom", ATT_PERM_READ);
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);

        /* Initialize the custom BLE service */
        ss = sensors_init(&ss_callbacks);
        
        /* Setup various timers */
        setup_timers();

        /* Initialize temperature sensor and create task*/
        InitTemperatureSensorDriver();
        i2c_acc_init();

        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* notified from BLE manager, can get event */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        if (ble_service_handle_event(hdr)) {
                                goto handled;
                        }

                        switch (hdr->evt_code) {
                        case BLE_EVT_GAP_CONNECTED:
                                //do_alert(1);
                                handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADV_COMPLETED:
                                handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                //do_alert(0);
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_REQ:
                        {
                                ble_evt_gap_pair_req_t *evt = (ble_evt_gap_pair_req_t *) hdr;
                                ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
                                break;
                        }
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

handled:
                        OS_FREE(hdr);

no_event:
                        // notify again if there are more events to process in queue
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }
                }
                
               
                if (notif & TEMP_MEAS_TIMER_NOTIF) {
                        DoMeasurementTemperature();
                }
                if (notif & ACC_MEAS_TIMER_NOTIF) {
                        i2c_acc_do_measurement();
                }

        }
}
