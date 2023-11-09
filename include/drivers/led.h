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

#ifndef __LED_H__
#define __LED_H__

#include <zephyr/kernel.h>

typedef struct led_controller_cb {
    void (*on_blinky_led_selected_sequence_changed)(const uint8_t *led_sequence);
    void (*on_blinky_led_status_changed)(const uint8_t *led_status);
} led_controller_cb;

int led_controller_init(struct led_controller_cb *cb);

int led_controller_run_sequence();

const uint8_t *led_controller_get_led_status();
const uint8_t *led_controller_get_selected_led_sequence();
void led_controller_set_selected_led_sequence(const uint8_t *led_sequence);

void led_controller_set_bt_connected();
void led_controller_unset_bt_connected();

#endif /* __LED_H__ */