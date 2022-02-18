#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_bufops.h"
#include "ble_common.h"
#include "ble_gatt.h"
#include "ble_gatts.h"
#include "ble_storage.h"
#include "ble_uuid.h"
#include "sensors_service.h"

static const char temp_user_descriptor_val[]  = "Read temperature values";
static const char acc_user_descriptor_val[]  = "Read accelerometer values";

/* Service related variables */
typedef struct {
        ble_service_t svc;

        // User-defined callback functions
//        const temperature_service_cb_t *cb;
        const sensors_service_cb_t *cb;

        // Attribute handles of BLE service
        uint16_t temp_int_value_h;
        uint16_t acc_int_value_h;

} sensors_service_t;


/* This function is called upon read requests to characteristic attribue value */
static void read_temp_int_value(sensors_service_t *ss, const ble_evt_gatts_read_req_t *evt)
{
        /*
         * Check whether the application has defined a callback function
         * for handling the event.
         */
        if (!ss->cb || !ss->cb->temp_get_characteristic_value) {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
                return;
        }

        /* The application should provide the requested data to the peer device.  */
        ss->cb->temp_get_characteristic_value(&ss->svc, evt->conn_idx);

        // callback executed properly
}


/* This function is called upon read requests to characteristic attribue value */
static void read_acc_int_value(sensors_service_t *ss, const ble_evt_gatts_read_req_t *evt)
{
        /*
         * Check whether the application has defined a callback function
         * for handling the event.
         */
        if (!ss->cb || !ss->cb->acc_get_characteristic_value) {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
                return;
        }

        /* The application should provide the requested data to the peer device.  */
        ss->cb->acc_get_characteristic_value(&ss->svc, evt->conn_idx);

        // callback executed properly
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
 * This function should be called by the application as a response to read requests
 */
void temp_get_int_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status, const uint16_t *value)
{
        sensors_service_t *ss = (sensors_service_t *) svc;
        uint16_t pdu[1];

        pdu[0] = *value;

        /* This function should be used as a response for every read request */
        ble_gatts_read_cfm(conn_idx, ss->temp_int_value_h, ATT_ERROR_OK, sizeof(pdu), pdu);
}

void acc_get_int_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status, const uint16_t *value)
{
        sensors_service_t *ss = (sensors_service_t *) svc;
        uint16_t pdu[1];

        pdu[0] = *value;

        /* This function should be used as a response for every read request */
        ble_gatts_read_cfm(conn_idx, ss->acc_int_value_h, ATT_ERROR_OK, sizeof(pdu), pdu);
}

/* Handler for read requests, that is BLE_EVT_GATTS_READ_REQ */
static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        sensors_service_t *ss = (sensors_service_t *) svc;
        /*
         * Identify for which attribute handle the read request has been sent to
         * and call the appropriate function.
         */

        if (evt->handle == ss->temp_int_value_h) {
                read_temp_int_value(ss, evt);
        }
        else if (evt->handle == ss->acc_int_value_h) {
                read_acc_int_value(ss, evt); 
        }
        else {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
        }
}

/* Function to be called after a cleanup event */
static void cleanup(ble_service_t *svc)
{
        sensors_service_t *ss = (sensors_service_t *) svc;
        OS_FREE(ss);
}


/* Initialization function for My Custom Service (sensors).*/
ble_service_t *sensors_init(const sensors_service_cb_t *cb)
{
        sensors_service_t *ss;

        uint16_t num_attr;
        att_uuid_t uuid;

        uint16_t temp_char_user_descriptor_h;
        uint16_t acc_char_user_descriptor_h;

        /* Allocate memory for the sevice hanle */
        ss = (sensors_service_t *)OS_MALLOC(sizeof(*ss));
        memset(ss, 0, sizeof(*ss));


        /* Declare handlers for specific BLE events */
        ss->svc.read_req  = handle_read_req;
        ss->svc.cleanup   = cleanup;
        ss->cb = cb;


        /*
         * 0 --> Number of Included Services
         * 2 --> Number of Characteristic Declarations
         * 2 --> Number of Descriptors
         */
        num_attr = ble_gatts_get_num_attr(0, 2, 2);


        /* Service declaration */
        ble_uuid_from_string("00000000-1111-2222-2222-333333333333", &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_attr);


        /* Characteristic declaration for Temperature sensor*/
        ble_uuid_from_string("11111111-0000-0000-0000-111111111111", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ,  ATT_PERM_READ, 
                            1, GATTS_FLAG_CHAR_READ_REQ, NULL, &ss->temp_int_value_h);


       /* Define descriptor of type Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(temp_user_descriptor_val),
                                                              0, &temp_char_user_descriptor_h);

        /* Characteristic declaration for accelerometer */
        ble_uuid_from_string("22222222-0000-0000-0000-222222222222", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ,  ATT_PERM_READ, 
                            1, GATTS_FLAG_CHAR_READ_REQ, NULL, &ss->acc_int_value_h);


       /* Define descriptor of type Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(acc_user_descriptor_val),
                                                              0, &acc_char_user_descriptor_h);

        /*
         * Register all the attribute handles so that they can be updated
         * by the BLE manager automatically.
         */
        ble_gatts_register_service(&ss->svc.start_h, &ss->temp_int_value_h, &ss->acc_int_value_h,
                          &temp_char_user_descriptor_h, &acc_char_user_descriptor_h ,0);


        /* Calculate the last attribute handle of the BLE service */
        ss->svc.end_h = ss->svc.start_h + num_attr;

        /* Set default attribute values */
        const uint8_t initial_value = 0;
        ble_gatts_set_value(ss->temp_int_value_h, 1,  &initial_value);
        ble_gatts_set_value(ss->acc_int_value_h, 1, &initial_value);
        ble_gatts_set_value(temp_char_user_descriptor_h,  sizeof(temp_user_descriptor_val),
                                                               temp_user_descriptor_val);
        ble_gatts_set_value(acc_char_user_descriptor_h,  sizeof(acc_user_descriptor_val),
                                                               acc_user_descriptor_val);

        /* Register the BLE service in BLE framework */
        ble_service_add(&ss->svc);

        /* Return the service handle */
        return &ss->svc;

}
