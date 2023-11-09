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

#ifndef __CUSTOM_LIB_H___
#define __CUSTOM_LIB_H___

/**
 * @brief Return parameter if non-zero, or Kconfig-controlled default
 *
 * Function returns the provided value if non-zero, or a Kconfig-controlled
 * default value if the parameter is zero.  This trivial function is
 * provided in order to have a library interface example that is trivial
 * to test.
 *
 * @param return_value_if_nonzero Value to return if non-zero
 * @returns return_value_if_nonzero when the parameter is non-zero
 * @returns CONFIG_CUSTOM_LIB_GET_VALUE_DEFAULT if parameter is zero
 */
int custom_lib_get_value(int return_value_if_nonzero);

#endif /* __CUSTOM_LIB_H___ */
