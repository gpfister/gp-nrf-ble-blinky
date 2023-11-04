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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

// #include <ble_controller/ble_controller.h>
// #include <led_controller/led_controller.h>

#include "controllers/ble_controller.h"
#include "controllers/led_controller.h"

/* Logger *********************************************************************/

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

/* Declarations ***************************************************************/

void printInfo();

/* Main ***********************************************************************/

int main(void)
{
    // Print informations about the app
    printInfo();

    // Define variables
    int ret;

    // Initialise the leds
    ret = led_controller_init();
    if (ret < 0) {
        return -1;
    }

    // Initializing BLE
    ret = ble_controller_init();
    if (ret < 0) {
        return -2;
    }

    // Main loop
    while (1) {
        ret = led_controller_run_sequence();
        if (ret < 0) {
            LOG_WRN("Exiting main loop because of an error");
            break;
        }
    }

    k_msleep(1000);
    return 0;
}

/* Informations ***************************************************************/

void printInfo()
{
    uint32_t ver = sys_kernel_version_get();
    LOG_INF("Welcome to BLE Blinky Peripheral");
    LOG_INF("Copyright (c) 2023, Greg PFISTER. MIT License.");
    LOG_INF("Firmware version: %s", APP_VERSION_STRING);
    LOG_INF("Target board: %s", CONFIG_BOARD);
    LOG_INF("Zephy Kernel version: %i.%i.%i", SYS_KERNEL_VER_MAJOR(ver), SYS_KERNEL_VER_MINOR(ver),
            SYS_KERNEL_VER_PATCHLEVEL(ver));
}