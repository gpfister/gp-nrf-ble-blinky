//
// gp-nrf52840-ble-blinky
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

#include "ble_controller.h"
#include "led_controller.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/rand32.h>

/* Logger *********************************************************************/

LOG_MODULE_REGISTER(led_controller, CONFIG_LOG_DEFAULT_LEVEL);

/* Declarations ***************************************************************/

int led_controller_set_blinking_led();
int led_controller_set_bt_status_led();
void led_controller_set_random_led_sequence();

/* Define led nodes ***********************************************************/

#define LED_CONTROLLER_LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led_controller_blinking_ld = GPIO_DT_SPEC_GET(LED_CONTROLLER_LED0_NODE, gpios);
#define LED_CONTROLLER_LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led_controller_bt_status_led = GPIO_DT_SPEC_GET(LED_CONTROLLER_LED1_NODE, gpios);
// #define LED2_NODE DT_ALIAS(led2)
// #define LED3_NODE DT_ALIAS(led3)

/* Sequence *******************************************************************/

uint16_t g_led_sequences[8][19] = {
    // Random (replaced at runtime)
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 0},
    // short
    {100, 100, 2000, 0},
    // Long
    {500, 500, 2000, 0},
    // Very long
    {1000, 1000, 2000, 0},
    // short/long/very long
    {100, 100, 500, 500, 1000, 1000, 2000, 0},
    // S.O.S.
    {100, 100, 100, 100, 100, 100, 500, 500, 500, 500, 500, 500, 100, 100, 100, 100, 100, 2000, 0},
    // Wave
    {250, 10, 500, 100, 1000, 100, 2000, 100, 100, 100, 2000, 100, 500, 100, 100, 2000, 0},
    // Stabilizing
    {250, 100, 3000, 100, 500, 100, 2000, 100, 1000, 100, 1500, 2000, 0},
};
uint8_t g_led_controller_selected_led_sequence = 0;

/* Led status *****************************************************************/

uint8_t g_led_controller_led_status = 0;

/* Semaphores *****************************************************************/

K_SEM_DEFINE(g_led_controller_led_sequence_sem, 1, 1);

/* Led initialization *********************************************************/

int led_controller_init()
{
    int ret;

    LOG_INF("Intializing led...");

    // Blinking led
    ret = led_controller_set_blinking_led();
    if (ret < 0) {
        return -1;
    }

    // BLE Status led
    ret = led_controller_set_bt_status_led();
    if (ret < 0) {
        return -1;
    }

    LOG_INF("Led initialization done!");
    return 0;
}

int led_controller_set_blinking_led()
{
    int ret;

    ret = gpio_is_ready_dt(&led_controller_blinking_ld);
    if (ret < 0) {
        LOG_ERR("Unable to init blinking led (error code %i)", ret);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_controller_blinking_ld, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Unable to configure blinking led (error code %i)", ret);
        return -2;
    }

    ret = gpio_pin_set_dt(&led_controller_blinking_ld, 0);
    if (ret < 0) {
        LOG_ERR("Unable to set blinking led to LOW (error code %i)", ret);
        return -3;
    }

    // Set initial random led sequence
    led_controller_set_random_led_sequence();
    k_sem_give(&g_led_controller_led_sequence_sem);

    return 0;
}

int led_controller_set_bt_status_led()
{
    int ret;

    ret = gpio_is_ready_dt(&led_controller_bt_status_led);
    if (ret < 0) {
        LOG_ERR("Unable to init BLE status led (error code %i)", ret);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_controller_bt_status_led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Unable to configure BLE status led (error code %i)", ret);
        return -2;
    }

    ret = gpio_pin_set_dt(&led_controller_bt_status_led, 0);
    if (ret < 0) {
        LOG_ERR("Unable to set BLE status led to LOW (error code %i)", ret);
        return -3;
    }

    return 0;
}

/* Run led sequence ***********************************************************/

int led_controller_run_sequence()
{
    // Blocking while processing the led sequence
    k_sem_take(&g_led_controller_led_sequence_sem, K_FOREVER);

    LOG_DBG("Running sequence %i", g_led_controller_selected_led_sequence);

    int ret;
    int i = 0;
    while (g_led_sequences[g_led_controller_selected_led_sequence][i] > 0) {
        // Set the led status
        g_led_controller_led_status = (g_led_controller_led_status + 1) % 2;
        ret = gpio_pin_set_dt(&led_controller_blinking_ld, g_led_controller_led_status);
        if (ret < 0) {
            LOG_ERR("Unable to set blinking led to %s (error code %i)",
                    g_led_controller_led_status > 0 ? "HIGH" : "LOW", ret);
            return -1;
        } else {
            LOG_DBG("Set led to %s", g_led_controller_led_status > 0 ? "HIGH" : "LOW");
        }

        // Update BLE Blinky Led Status value
        ble_controller_update_blinky_led_status_value(&g_led_controller_led_status);

        // Sleep
        LOG_DBG("Wait for %ims", g_led_sequences[g_led_controller_selected_led_sequence][i]);
        k_msleep(g_led_sequences[g_led_controller_selected_led_sequence][i]);
        i++;
    }

    // Releasing the led sequence
    k_sem_give(&g_led_controller_led_sequence_sem);

    return 0;
}

/* Led status getter **********************************************************/

const uint8_t *led_controller_get_led_status() { return (const uint8_t *)&g_led_controller_led_status; }

/* Led sequence getter/setter *************************************************/

const uint8_t *led_controller_get_selected_led_sequence()
{
    return (const uint8_t *)&g_led_controller_selected_led_sequence;
}

void led_controller_set_selected_led_sequence(const uint8_t led_sequence)
{
    if (led_sequence > 7) {
        LOG_ERR("Invalid led sequence value: %i > 7", led_sequence);
        return;
    }
    k_sem_take(&g_led_controller_led_sequence_sem, K_FOREVER);
    if (led_sequence == 0) {
        led_controller_set_random_led_sequence();
    }
    LOG_INF("Switching led sequence from %i to %i", g_led_controller_selected_led_sequence, led_sequence);
    g_led_controller_selected_led_sequence = led_sequence;
    k_sem_give(&g_led_controller_led_sequence_sem);
}

/* Control BLE status led *****************************************************/

void led_controller_set_bt_connected()
{
    int ret;

    ret = gpio_pin_set_dt(&led_controller_bt_status_led, 1);
    if (ret < 0) {
        LOG_ERR("Unable to set BLE status led to HIGH (error code %i)", ret);
    }
}

void led_controller_unset_bt_connected()
{
    int ret;

    ret = gpio_pin_set_dt(&led_controller_bt_status_led, 0);
    if (ret < 0) {
        LOG_ERR("Unable to set BLE status led to LOW (error code %i)", ret);
    }
}

/* Utilities ******************************************************************/

size_t led_controller_led_sequence_length(const uint16_t *led_sequence)
{
    size_t i = 0;

    if (led_sequence == NULL) {
        return 0;
    }

    while (led_sequence[i] != 0) {
        i++;
    }

    return i;
}

void led_controller_set_random_led_sequence()
{
    // Set the randome sequence
    LOG_DBG("Generating random led sequence...");
    for (uint8_t i = 0; i < 16; i++) {
        uint8_t rand;
        sys_rand_get(&rand, sizeof(uint8_t));
        g_led_sequences[0][i] = i % 2 == 0 ? (rand % 30 + 1) * 100 : 1000;
        LOG_DBG("#%i -> %ims", i + 1, g_led_sequences[0][i]);
    }
    LOG_DBG("#17 -> %ims", g_led_sequences[0][16]);
}