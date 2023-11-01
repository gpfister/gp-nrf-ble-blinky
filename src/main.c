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

#include <stdlib.h>

#include "controllers/ble_controller.h"
#include "controllers/led_controller.h"

int main(void) {
  // Define variables
  int ret;
  __uint16_t timeInterval = 250;
  __uint8_t sequence[8] = {0, 0, 1, 1, 2, 2, 3, 3};

  // Initialise the leds
  ret = led_controller_init();
  if (ret < 0) {
    return -1;
  }

  // Set time interval and sequence
  led_controller_set_timeInterval(timeInterval);
  led_controller_set_sequence(sequence);

  // Initializing BLE
  ret = ble_controller_init();
  if (ret < 0) {
    return -2;
  }

  // Main loop
  while (1) {
    ret = led_controller_run_sequnce();
    if (ret < 0) {
      return -3;
    }
  }
  return 0;
}
