//
// gp-nrf-ble-blinky
// Copyright (c) 2023, Greg PFISTER. MIT Licennse
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>

#include <drivers/ble.h>

/* Logger *********************************************************************/

LOG_MODULE_REGISTER(BLE_DRIVER, CONFIG_BLE_DRIVER_LOG_LEVEL);

/* Internal declarations ******************************************************/

void on_ready_cb(int err);
void on_connected_cb(struct bt_conn *conn, uint8_t err);
void on_disconnected_cb(struct bt_conn *conn, uint8_t reason);
ssize_t on_read_blinky_led_sequence_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                                                      uint16_t len, uint16_t offset);
ssize_t on_read_blinky_led_status_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                                                    uint16_t len, uint16_t offset);
ssize_t on_write_blinky_led_sequence_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                                       const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
void on_blinky_led_sequence_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value);
void on_blinky_led_status_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value);
void on_blinky_led_sequence_data_sent_cb(struct bt_conn *conn, void *user_data);
void on_blinky_led_status_data_sent_cb(struct bt_conn *conn, void *user_data);

/* Advertizing data ***********************************************************/

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
static const struct bt_data g_advertizing_data[] = {
    // Flags: BLE, no classic
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    // Device name
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* Scanning data **************************************************************/

#define UUID_ADVERTISED_SERVICE_VAL BT_UUID_128_ENCODE(0xfefb5d60, 0x994e, 0x4d19, 0xbe43, 0xd7849a807198)
#define UUID_ADVERTISED_SERVICE     BT_UUID_DECLARE_128(UUID_ADVERTISED_SERVICE_VAL)
static const struct bt_data g_scanning_data[] = {
    // Advertised service
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, UUID_ADVERTISED_SERVICE_VAL),
};

/* Service and characteristics UUID *******************************************/

#define UUID_BLINKY_SERVICE_VAL BT_UUID_128_ENCODE(0x826c9400, 0x8f2f, 0x4dc5, 0x8319, 0xd07b584cf83)
#define UUID_BLINKY_SERVICE     BT_UUID_DECLARE_128(UUID_BLINKY_SERVICE_VAL)
#define UUID_BLINKY_LED_SEQUENCE_READ_CHARACTERISTIC_VAL                                                               \
    BT_UUID_128_ENCODE(0x7656d6e9, 0x46da, 0x425a, 0x8c9e, 0x4fa2becdf619)
#define UUID_BLINKY_LED_SEQUENCE_READ_CHARACTERISTIC                                                                   \
    BT_UUID_DECLARE_128(UUID_BLINKY_LED_SEQUENCE_READ_CHARACTERISTIC_VAL)
#define UUID_BLINKY_LED_SEQUENCE_WRITE_CHARACTERISTIC_VAL                                                              \
    BT_UUID_128_ENCODE(0x3ed3db80, 0x4fdb, 0x461b, 0xad850, 0xcf0764566488)
#define UUID_BLINKY_LED_SEQUENCE_WRITE_CHARACTERISTIC                                                                  \
    BT_UUID_DECLARE_128(UUID_BLINKY_LED_SEQUENCE_WRITE_CHARACTERISTIC_VAL)
#define UUID_BLINKY_CHARACTERISTIC_LED_STATUS_VAL BT_UUID_128_ENCODE(0x24b35ad0, 0xd0f0, 0x4811, 0xbdfb, 0x16d4451a514f)
#define UUID_BLINKY_CHARACTERISTIC_LED_STATUS     BT_UUID_DECLARE_128(UUID_BLINKY_CHARACTERISTIC_LED_STATUS_VAL)
BT_GATT_SERVICE_DEFINE(g_blinky_service, BT_GATT_PRIMARY_SERVICE(UUID_BLINKY_SERVICE),
                       BT_GATT_CHARACTERISTIC(UUID_BLINKY_LED_SEQUENCE_READ_CHARACTERISTIC,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
                                              on_read_blinky_led_sequence_characteristic_cb, NULL, NULL),
                       BT_GATT_CCC(on_blinky_led_sequence_ccc_changed_cb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(UUID_BLINKY_LED_SEQUENCE_WRITE_CHARACTERISTIC,
                                              BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                                              NULL, on_write_blinky_led_sequence_characteristic_cb, NULL),
                       BT_GATT_CHARACTERISTIC(UUID_BLINKY_CHARACTERISTIC_LED_STATUS,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
                                              on_read_blinky_led_status_characteristic_cb, NULL, NULL),
                       BT_GATT_CCC(on_blinky_led_status_ccc_changed_cb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

/* Notifications **************************************************************/

typedef enum _ble_controller_notification_status {
    BLE_CONTROLLER_NOTIFICATION_STATUS_ENABLED,
    BLE_CONTROLLER_NOTIFICATION_STATUS_DISABLED,
} ble_controller_notification_status;
static ble_controller_notification_status g_blinky_led_sequence_notifications_status =
    BLE_CONTROLLER_NOTIFICATION_STATUS_DISABLED;
static ble_controller_notification_status g_blinky_led_status_notifications_status =
    BLE_CONTROLLER_NOTIFICATION_STATUS_DISABLED;

/* BLE connection *************************************************************/

// Connection callbacks
static struct bt_conn_cb g_connection_callback = {
    .connected = on_connected_cb,
    .disconnected = on_disconnected_cb,
};

// Semaphores
static K_SEM_DEFINE(g_ble_init_ok, 1, 1);

// Current connect
static struct bt_conn *g_current_conn;

/* Controller callbacks *******************************************************/

static ble_controller_cb g_cb = {
    .on_connect = NULL,
    .on_disconnect = NULL,
    .on_read_blinky_led_selected_sequence = NULL,
    .on_write_blinky_led_selected_sequence = NULL,
    .on_read_blinky_led_status = NULL,
};

/* BLE initialization *********************************************************/

/**
 * Initializes the BLE connection
 */
int ble_controller_init(struct ble_controller_cb *cb)
{
    int ret;

    // Callbacks
    g_cb = *cb;

    // Register callbacks
    bt_conn_cb_register(&g_connection_callback);

    // Unale BLE
    LOG_INF("Intializing BLE controller...");
    ret = bt_enable(on_ready_cb);
    if (ret < 0) {
        LOG_ERR("Unable to initialize BLE controller (error code %i)", ret);
        return -1;
    }

    // Wait for BLE to be initialized
    k_sem_take(&g_ble_init_ok, K_FOREVER);

    // Advertize
    ret = bt_le_adv_start(BT_LE_ADV_CONN, g_advertizing_data, ARRAY_SIZE(g_advertizing_data), g_scanning_data,
                          ARRAY_SIZE(g_scanning_data));
    if (ret < 0) {
        LOG_ERR("Unable to start advertizing (error code %i)", ret);
        return -2;
    }

    LOG_INF("BLE controller initialization done!");
    return 0;
}

/* Callbacks ******************************************************************/

/**
 * Callback when the BLE stack is ready
 */
void on_ready_cb(int err)
{
    if (err) {
        LOG_ERR("Bluetooth could not be enabled (error code: %i)", err);
    } else {
        LOG_INF("Bluetooth is ready");
    }

    k_sem_give(&g_ble_init_ok);
}

/**
 * When a central connects to the device
 */
void on_connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Unable to connect (error code %i)", err);
        return;
    }

    // Reference current connection
    g_current_conn = bt_conn_ref(conn);
    LOG_INF("Device connected %p", (void *)g_current_conn);

    // Set led indicating BLE central is connected
    if (g_cb.on_connect) {
        g_cb.on_connect();
    }
}

/**
 * When the central disconnects from the device
 */
void on_disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Device disconnected (reason code %i)", reason);

    // If there was an active connect, unreference it
    if (g_current_conn) {
        bt_conn_unref(g_current_conn);
        g_current_conn = NULL;
    }

    // Unset led BLE indicating BLE central is connected
    if (g_cb.on_disconnect) {
        g_cb.on_disconnect();
    }
}

/**
 * Read the led sequence value
 */
ssize_t on_read_blinky_led_sequence_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                                                      uint16_t len, uint16_t offset)
{
    LOG_DBG("Reading led sequence...");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, g_cb.on_read_blinky_led_selected_sequence(),
                             sizeof(uint8_t));
}

ssize_t on_write_blinky_led_sequence_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                                       const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    LOG_DBG("Received Blinky Led Sequence Data on handle %d & conn %p", attr->handle, (void *)conn);

    if (len != sizeof(uint8_t)) {
        LOG_ERR("Invalid Blinky Led Sequence received (expected %i bytes, "
                "received %i bytes)",
                sizeof(uint8_t), len);
    } else {
        const uint8_t *value = buf;
        if (g_cb.on_write_blinky_led_selected_sequence) {
            g_cb.on_write_blinky_led_selected_sequence(value);
        }
        LOG_HEXDUMP_DBG(buf, len, "Blinky Led Sequence value received");
    }
    return len;
}

/**
 * Read the led status value
 */
ssize_t on_read_blinky_led_status_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                                                    uint16_t len, uint16_t offset)
{
    LOG_DBG("Reading led status...");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, g_cb.on_read_blinky_led_status(), 1 * sizeof(uint8_t));
}

/**
 * Handle notification status changed
 */
void on_blinky_led_sequence_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_DBG("Blinky led sequence characteristics notifications %s", notif_enabled ? "enabled" : "disabled");
    g_blinky_led_sequence_notifications_status =
        notif_enabled ? BLE_CONTROLLER_NOTIFICATION_STATUS_ENABLED : BLE_CONTROLLER_NOTIFICATION_STATUS_DISABLED;
}

/**
 * Handle notification status changed
 */
void on_blinky_led_status_ccc_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_DBG("Blinky led sequence status notifications %s", notif_enabled ? "enabled" : "disabled");
    g_blinky_led_status_notifications_status =
        notif_enabled ? BLE_CONTROLLER_NOTIFICATION_STATUS_ENABLED : BLE_CONTROLLER_NOTIFICATION_STATUS_DISABLED;
}

/**
 *
 */
void on_blinky_led_sequence_data_sent_cb(struct bt_conn *conn, void *user_data)
{
    ARG_UNUSED(user_data);
    LOG_DBG("Blinky Led Sequence data sent on connection %p", (void *)conn);
}

/**
 *
 */
void on_blinky_led_status_data_sent_cb(struct bt_conn *conn, void *user_data)
{
    ARG_UNUSED(user_data);
    LOG_DBG("Blinky Led Status data sent on connection %p", (void *)conn);
}

/* Update characteristic values ***********************************************/

void ble_controller_update_blinky_led_sequence_value(const uint8_t *led_sequence)
{
    int err;
    if (g_blinky_led_sequence_notifications_status == BLE_CONTROLLER_NOTIFICATION_STATUS_ENABLED) {
        struct bt_gatt_notify_params params = {0};
        const struct bt_gatt_attr *attr = &g_blinky_service.attrs[2];

        params.attr = attr;
        params.data = led_sequence;
        params.len = sizeof(uint8_t);
        params.func = on_blinky_led_sequence_data_sent_cb;

        err = bt_gatt_notify_cb(g_current_conn, &params);
        if (err) {
            LOG_ERR("Unable to push Blinky Led Sequence update (error code %i)", err);
            return;
        }
    }
}

void ble_controller_update_blinky_led_status_value(const uint8_t *led_status)
{
    int err;
    if (g_blinky_led_status_notifications_status == BLE_CONTROLLER_NOTIFICATION_STATUS_ENABLED) {
        struct bt_gatt_notify_params params = {0};
        const struct bt_gatt_attr *attr = &g_blinky_service.attrs[6];

        params.attr = attr;
        params.data = led_status;
        params.len = sizeof(uint8_t);
        params.func = on_blinky_led_status_data_sent_cb;

        err = bt_gatt_notify_cb(g_current_conn, &params);
        if (err) {
            LOG_ERR("Unable to push Blinky Led Status update (error code %i)", err);
            return;
        }
    }
}