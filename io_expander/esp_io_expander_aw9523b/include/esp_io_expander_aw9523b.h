#pragma once

#include <stdint.h>

#include "driver/i2c.h"
#include "esp_err.h"

#include "esp_io_expander.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new aw9523b IO expander driver
 *
 * @note The I2C communication should be initialized before use this function
 *
 * @param i2c_num: I2C port num
 * @param i2c_address: I2C address of chip
 * @param handle: IO expander handle
 *
 * @return
 *      - ESP_OK: Success, otherwise returns ESP_ERR_xxx
 */
esp_err_t esp_io_expander_new_i2c_aw9523b(i2c_port_t i2c_num, uint32_t i2c_address, esp_io_expander_handle_t *handle);

/**
 * @brief I2C address of the aw9523b
 *
 * The 8-bit address format is as follows:
 *
 *                (Slave Address)
 *     ┌─────────────────┷─────────────────┐
 *  ┌─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐
 *  |  1  |  0  |  1  |  1  |  0  | A1  | A0  | R/W |
 *  └─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘
 *     └────────┯──────────────┘     └──┯──┘
 *           (Fixed)              (Hareware Selectable)
 *
 * And the 7-bit slave address is the most important data for users.
 * For example, if a chip's A0,A1 are connected to GND, it's 7-bit slave address is 0111000b(0x38).
 * Then users can use `ESP_IO_EXPANDER_I2C_AW9523B_ADDRESS_000` to init it.
 */
#define ESP_IO_EXPANDER_I2C_AW9523B_ADDRESS_000    (0x58)
#define ESP_IO_EXPANDER_I2C_AW9523B_ADDRESS_001    (0x59)
#define ESP_IO_EXPANDER_I2C_AW9523B_ADDRESS_010    (0x5A)
#define ESP_IO_EXPANDER_I2C_AW9523B_ADDRESS_011    (0x5B)


#ifdef __cplusplus
}
#endif