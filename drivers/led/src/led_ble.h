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

#ifndef __LED_BLE_H__
#define __LED_BLE_H__

#if CONFIG_LED_DRIVER_BLE

#include <zephyr/drivers/gpio.h>

/* Define led nodes ***********************************************************/

#define BLE_LED_NODE DT_ALIAS(ledble)
static const struct gpio_dt_spec g_ble_led = GPIO_DT_SPEC_GET(BLE_LED_NODE, gpios);

/* Declarations ***************************************************************/

int configure_ble_led();

#endif /* CONFIG_LED_DRIVER_BLE */

#endif /** __LED_BLE_H__ */