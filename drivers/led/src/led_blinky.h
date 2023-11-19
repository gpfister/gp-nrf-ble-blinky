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

#ifndef __LED_BLINKY_H__
#define __LED_BLINKY_H__

#if CONFIG_LED_DRIVER_BLINKY

#include <zephyr/drivers/gpio.h>

/* Define led nodes ***********************************************************/

#define BLINKY_LED_NODE DT_ALIAS(ledblinky)
static const struct gpio_dt_spec g_blinky_led = GPIO_DT_SPEC_GET(BLINKY_LED_NODE, gpios);

/* Declarations ***************************************************************/

int configure_blinky_led();

#endif /* CONFIG_LED_DRIVER_BLINKY */

#endif /** __LED_BLINKY_H__ */