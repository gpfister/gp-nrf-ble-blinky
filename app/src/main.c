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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>
#include <drivers/ble.h>
#include <drivers/led.h>

/* Logger *********************************************************************/

LOG_MODULE_REGISTER(APP, CONFIG_LOG_DEFAULT_LEVEL);

/* Declarations ***************************************************************/

void printInfo();
void on_blinky_led_selected_sequence_changed_cb(const uint8_t *led_sequence);
void on_blinky_led_status_changed_cb(const uint8_t *led_status);
void on_ble_central_connected_cb(const void *conn);
void on_ble_central_disconnected_cb(const void *conn);
const uint8_t *on_ble_central_read_blinky_led_selected_sequence_cb(const void *conn);
const uint8_t *on_ble_central_read_blinky_led_status_cb(const void *conn);
void on_ble_central_write_blinky_led_selected_sequence_cb(const void *conn, const uint8_t *led_sequence);

// /* Main ***********************************************************************/

int main(void)
{
    // Print informations about the app
    printInfo();

    // Define variables
    int ret;

    // Initialise the leds
    struct led_controller_cb led_controller_cb = {
        .on_blinky_led_selected_sequence_changed = on_blinky_led_selected_sequence_changed_cb,
        .on_blinky_led_status_changed = on_blinky_led_status_changed_cb,
    };
    ret = led_controller_init(&led_controller_cb);
    if (ret < 0) {
        return -1;
    }

    // Initializing BLE
    struct ble_controller_cb ble_controller_cb = {
        .on_connect = on_ble_central_connected_cb,
        .on_disconnect = on_ble_central_disconnected_cb,
        .on_read_blinky_led_selected_sequence = on_ble_central_read_blinky_led_selected_sequence_cb,
        .on_read_blinky_led_status = on_ble_central_read_blinky_led_status_cb,
        .on_write_blinky_led_selected_sequence = on_ble_central_write_blinky_led_selected_sequence_cb,
    };
    ret = ble_controller_init(&ble_controller_cb);
    if (ret < 0) {
        return -2;
    }

    // Main loop
    while (1) {
        ret = led_controller_run_sequence();
        if (ret < 0) {
            LOG_WRN("Exiting main loop because of an error");
            break;
        }
    }

    k_msleep(1000);
    return 0;
}

/* Informations ***************************************************************/

void printInfo()
{
    uint32_t ver = sys_kernel_version_get();
    LOG_INF("Welcome to BLE Blinky Peripheral");
    LOG_INF("Copyright (c) 2023, Greg PFISTER. MIT License.");
    LOG_INF("Firmware version: %s", APP_VERSION_STRING);
    LOG_INF("Target board: %s", CONFIG_BOARD);
    LOG_INF("Zephy Kernel version: %i.%i.%i", SYS_KERNEL_VER_MAJOR(ver), SYS_KERNEL_VER_MINOR(ver),
            SYS_KERNEL_VER_PATCHLEVEL(ver));
}

/* Led Controller Callbacks ***************************************************/

void on_blinky_led_selected_sequence_changed_cb(const uint8_t *led_sequence)
{
#if CONFIG_LED_DRIVER_BLE
    ble_controller_update_blinky_led_sequence_value(led_sequence);
#endif /* CONFIG_LED_DRIVER_BLE */
}

void on_blinky_led_status_changed_cb(const uint8_t *led_status)
{
#if CONFIG_LED_DRIVER_BLE
    ble_controller_update_blinky_led_status_value(led_status);
#endif /* CONFIG_LED_DRIVER_BLE */
}

/* BLE Controller Callbacks ***************************************************/

void on_ble_central_connected_cb(const void *conn)
{
    LOG_INF("BLE - Central device %p connected", (void *)conn);
    led_controller_set_bt_connected();
}

void on_ble_central_disconnected_cb(const void *conn)
{
    LOG_INF("Central device %p disconnected", (void *)conn);
    led_controller_unset_bt_connected();
}

const uint8_t *on_ble_central_read_blinky_led_selected_sequence_cb(const void *conn)
{
    LOG_INF("BLE - Central %p is reading selected blinky led sequence value", (void *)conn);
    return led_controller_get_selected_led_sequence();
}

const uint8_t *on_ble_central_read_blinky_led_status_cb(const void *conn)
{
    LOG_INF("BLE - Central %p is reading selected blinky led status", (void *)conn);
    return led_controller_get_led_status();
}

void on_ble_central_write_blinky_led_selected_sequence_cb(const void *conn, const uint8_t *led_sequence)
{
    LOG_INF("BLE - Central %p is setting new selected sequence for blinkty led: %i", (void *)conn, *led_sequence);
    led_controller_set_selected_led_sequence(led_sequence);
}
