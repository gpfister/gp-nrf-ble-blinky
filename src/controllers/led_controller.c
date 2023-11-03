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

#include "led_controller.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "ble_controller.h"

/* Declarations ***************************************************************/

int led_controller_set_blinking_led();
int led_controller_set_bt_status_led();

/* Define led nodes ***********************************************************/

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led_blinking = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led_bt_status = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
// #define LED2_NODE DT_ALIAS(led2)
// #define LED3_NODE DT_ALIAS(led3)

/* Sequence *******************************************************************/

uint16_t g_led_sequence[8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

/* Led status *****************************************************************/

uint8_t g_led_status = 0;

/* Led initialization *********************************************************/

int led_controller_init() {
  int ret;

  printk("[Led Controller] Intializing led...\n");

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

  printk("[Led Controller] Led initialization done!\n");
  return 0;
}

int led_controller_set_blinking_led() {
  int ret;

  ret = gpio_is_ready_dt(&led_blinking);
  if (ret < 0) {
    printk("[Led Controller] Unable to init blinking led (error code %i)\n", ret);
    return -1;
  }

  ret = gpio_pin_configure_dt(&led_blinking, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    printk("[Led Controller] Unable to configure blinking led (error code %i)\n", ret);
    return -2;
  }

  ret = gpio_pin_set_dt(&led_blinking, 0);
  if (ret < 0) {
    printk("[Led Controller] Unable to set blinking led to LOW (error code %i)\n", ret);
    return -3;
  }

  return 0;
}

int led_controller_set_bt_status_led() {
  int ret;

  ret = gpio_is_ready_dt(&led_bt_status);
  if (ret < 0) {
    printk("[Led Controller] Unable to init BLE status led (error code %i)\n", ret);
    return -1;
  }

  ret = gpio_pin_configure_dt(&led_bt_status, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    printk("[Led Controller] Unable to configure BLE status led (error code %i)\n", ret);
    return -2;
  }

  ret = gpio_pin_set_dt(&led_bt_status, 0);
  if (ret < 0) {
    printk("[Led Controller] Unable to set BLE status led to LOW (error code %i)\n", ret);
    return -3;
  }

  return 0;
}

/* Run led sequence ***********************************************************/

int led_controller_run_sequnce() {
  int ret;
  for (int i = 0; i < 8; i++) {
    // Set the led status
    g_led_status = (g_led_status + 1) % 2;
    ret = gpio_pin_set_dt(&led_blinking, g_led_status);
    if (ret < 0) {
      printk("[Led Controller] Unable to set blinking led to %s (error code %i)\n", g_led_status > 0 ? "HIGH" : "LOW", ret);
      return -1;
    }

    // Update BLE Blinky Led Status value
    ble_controller_update_blinky_led_status_value(&g_led_status);

    // Sleep
    k_msleep(g_led_sequence[i]);
  }
  return 0;
}

/* Led status getter **********************************************************/

const uint8_t* led_controller_get_led_status() {
  return (const uint8_t*)&g_led_status;
}

/* Led sequence getter/setter *************************************************/

const uint16_t* led_controller_get_led_sequence() {
  return (const uint16_t*)g_led_sequence;
}

void led_controller_set_led_sequence(const uint16_t* led_sequence) {
  // Save value
  printk("[Led Controller] Setting sequence to");
  for (int i = 0; i < 8; i++) {
    printk(" ");
    g_led_sequence[i] = led_sequence[i];
    printk("#%ims", led_sequence[i]);
  }
  printk("\n");

  // Update BLE Blinky Led Sequence characteristic value
  ble_controller_update_blinky_led_sequence_value(g_led_sequence);
}

/* Control BLE status led *****************************************************/

void led_controller_set_bt_connected() {
  int ret;

  ret = gpio_pin_set_dt(&led_bt_status, 1);
  if (ret < 0) {
    printk("[Led Controller] Unable to set BLE status led to HIGH (error code %i)\n", ret);
  }
}

void led_controller_unset_bt_connected() {
  int ret;

  ret = gpio_pin_set_dt(&led_bt_status, 0);
  if (ret < 0) {
    printk("[Led Controller] Unable to set BLE status led to LOW (error code %i)\n", ret);
  }
}