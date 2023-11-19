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

#ifndef __BLE_H__
#define __BLE_H__

#include <zephyr/kernel.h>

typedef struct ble_controller_cb {
    void (*on_connect)(const void *conn);
    void (*on_disconnect)(const void *conn);
    const uint8_t *(*on_read_blinky_led_selected_sequence)(const void *conn);
    const uint8_t *(*on_read_blinky_led_status)(const void *conn);
    void (*on_write_blinky_led_selected_sequence)(const void *conn, const uint8_t *led_sequence);
} ble_controller_cb;

int ble_controller_init(struct ble_controller_cb *cb);

void ble_controller_update_blinky_led_sequence_value(const uint8_t *led_sequence);
void ble_controller_update_blinky_led_status_value(const uint8_t *led_status);

#endif /* __BLE_H__ */