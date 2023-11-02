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

/* Define led nodes */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* Declarations */
int led_controller_set_blinking_led();
int led_controller_set_bt_status_led();

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led_blinking = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_bt_status = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

/* Sequence */
uint16_t g_sequence[8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

/* Time interval */
uint16_t g_time_interval = 1000;

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

int led_controller_run_sequnce() {
  int ret;
  for (int i = 0; i < 8; i++) {
    ret = gpio_pin_toggle_dt(&led_blinking);
    if (ret < 0) {
      printk("[Led Controller] Unable to toggle blinking led (error code %i)\n", ret);
      return -1;
    }
    k_msleep(g_sequence[i]);
  }
  return 0;
}

void led_controller_set_timeInterval(uint16_t timeInterval) {
  g_time_interval = timeInterval;
  printk("[Led Controller] Setting time interval to %ims\n", timeInterval);
}

void led_controller_set_sequence(uint16_t sequence[8]) {
  printk("[Led Controller] Setting sequence to");
  for (int i = 0; i < 8; i++) {
    printk(" ");
    g_sequence[i] = sequence[i];
    printk("#%ims", sequence[i]);
  }
  printk("\n");
}

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