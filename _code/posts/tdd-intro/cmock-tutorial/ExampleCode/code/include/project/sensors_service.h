#include <stdint.h>
#include <ble_service.h>
#include "TemperatureDriver.h"
#include "AccelerometerDriver.h"

/* User-defined callback functions - Prototyping */
typedef void (* sensor_get_int_value_cb_t) (ble_service_t *svc, uint16_t conn_idx);


/* User-defined callback functions */
typedef struct {
        /* Handler for read requests - Triggered on application context */
        sensor_get_int_value_cb_t temp_get_characteristic_value;
        sensor_get_int_value_cb_t acc_get_characteristic_value;
} sensors_service_cb_t;


/*
 * \brief This function creates the custom BLE service and registers it in BLE framework
 *
 * \param [in] variable_value Default characteristic value
 * \param [in] cb             Application callback functions
 *
 * \return service handle
 *
 */
ble_service_t *sensors_init(const sensors_service_cb_t *cb);

/*
 * This function should be called by the application as a response to a read request
 *
 * \param[in] svc       service instance
 * \param[in] conn_idx  connection index
 * \param[in] status    ATT error
 * \param[in] value     attribute value
 */
void temp_get_int_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status, const uint16_t *value);
void acc_get_int_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status, const uint16_t *value);

