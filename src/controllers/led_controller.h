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

#ifndef __LED_CONTROLLER_H__
#define __LED_CONTROLLER_H__

#include <zephyr/kernel.h>

int led_controller_init();

int led_controller_run_sequnce();

const uint8_t *led_controller_get_led_status();
const uint16_t *led_controller_get_led_sequence();
void led_controller_set_led_sequence(uint16_t sequence[8]);

void led_controller_set_bt_connected();
void led_controller_unset_bt_connected();

#endif