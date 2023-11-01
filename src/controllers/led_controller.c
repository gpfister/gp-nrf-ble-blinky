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

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led[4] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios)};

__uint8_t gSequence[8] = {0, 1, 2, 3, 3, 2, 1, 0};
__uint16_t gTimeInterval = 1000;

int led_controller_init() {
  int ret;

  printk("[Led Controller] Intializing led...");
  for (int i = 0; i < 4; i++) {
    printk(" ");

    if (!gpio_is_ready_dt(&led[i])) {
      return -1;
    }

    ret = gpio_pin_configure_dt(&led[i], GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
      return -2;
    }

    ret = gpio_pin_toggle_dt(&led[i]);
    if (ret < 0) {
      return -3;
    }

    printk("#%i", i);
  }
  printk(" done!\n");

  return 0;
}

int led_controller_run_sequnce() {
  int ret;
  for (int i = 0; i < 8; i++) {
    printk("[Led Controller] Toggling led #%i\n", gSequence[i]);
    ret = gpio_pin_toggle_dt(&led[gSequence[i]]);
    if (ret < 0) {
      return -1;
    }
    k_msleep(gTimeInterval);
  }
  return 0;
}

void led_controller_set_timeInterval(__uint16_t timeInterval) {
  gTimeInterval = timeInterval;
  printk("[Led Controller] Setting time interval to %ims\n", timeInterval);
}

void led_controller_set_sequence(__uint8_t sequence[8]) {
  printk("[Led Controller] Setting sequence to");
  for (int i = 0; i < 8; i++) {
    printk(" ");
    gSequence[i] = sequence[i];
    printk("#%i", sequence[i]);
  }
  printk("\n");
}