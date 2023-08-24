#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "driver/i2c.h"
#include "esp_bit_defs.h"
#include "esp_check.h"
#include "esp_log.h"

#include "esp_io_expander.h"
#include "esp_io_expander_aw9523b.h"
#include "esp_io_expander_aw9523b_reg.h"

/* Timeout of each I2C communication */
#define I2C_TIMEOUT_MS          (10)

#define IO_COUNT                (8)

/* Default register value on power-up */
#define DIR_REG_DEFAULT_VAL     (0xff)
#define OUT_REG_DEFAULT_VAL     (0xff)

/**
 * @brief Device Structure Type
 *
 */
typedef struct {
    esp_io_expander_t base;
    i2c_port_t i2c_num;
    uint32_t i2c_address;
    struct {
        uint8_t direction;
        uint8_t output;
    } regs;
} esp_io_expander_aw9523b_t;

static char *TAG = "aw9523b";

static esp_err_t read_input_reg(esp_io_expander_handle_t handle, uint32_t *value);
static esp_err_t write_output_reg(esp_io_expander_handle_t handle, uint32_t value);
static esp_err_t read_output_reg(esp_io_expander_handle_t handle, uint32_t *value);
static esp_err_t write_direction_reg(esp_io_expander_handle_t handle, uint32_t value);
static esp_err_t read_direction_reg(esp_io_expander_handle_t handle, uint32_t *value);
static esp_err_t reset(esp_io_expander_t *handle);
static esp_err_t del(esp_io_expander_t *handle);

esp_err_t esp_io_expander_new_i2c_aw9523b(i2c_port_t i2c_num, uint32_t i2c_address, esp_io_expander_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(i2c_num < I2C_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "Invalid i2c num");
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "Invalid handle");

    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)calloc(1, sizeof(esp_io_expander_aw9523b_t));
    ESP_RETURN_ON_FALSE(aw9523b, ESP_ERR_NO_MEM, TAG, "Malloc failed");

    aw9523b->base.config.io_count = IO_COUNT;
    aw9523b->base.config.flags.dir_out_bit_zero = 1;
    aw9523b->i2c_num = i2c_num;
    aw9523b->i2c_address = i2c_address;
    aw9523b->base.read_input_reg = read_input_reg;
    aw9523b->base.write_output_reg = write_output_reg;
    aw9523b->base.read_output_reg = read_output_reg;
    aw9523b->base.write_direction_reg = write_direction_reg;
    aw9523b->base.read_direction_reg = read_direction_reg;
    aw9523b->base.del = del;
    aw9523b->base.reset = reset;

    esp_err_t ret = ESP_OK;
    /* Reset configuration and register status */
    ESP_GOTO_ON_ERROR(reset(&aw9523b->base), err, TAG, "Reset failed");

    *handle = &aw9523b->base;
    return ESP_OK;
err:
    free(aw9523b);
    return ret;
}

static esp_err_t read_input_reg(esp_io_expander_handle_t handle, uint32_t *value)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);

    uint8_t temp = 0;
    // *INDENT-OFF*
    ESP_RETURN_ON_ERROR(
        i2c_master_read_from_device(aw9523b->i2c_num, aw9523b->i2c_address, &temp, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)),
        TAG, "Read input reg failed");
    // *INDENT-ON*
    *value = temp;
    return ESP_OK;
}

static esp_err_t write_output_reg(esp_io_expander_handle_t handle, uint32_t value)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);
    value &= 0xff;

    uint8_t data = (uint8_t)value;
    ESP_RETURN_ON_ERROR(
        i2c_master_write_to_device(aw9523b->i2c_num, aw9523b->i2c_address, &data, 1, pdMS_TO_TICKS(I2C_TIMEOUT_MS)),
        TAG, "Write output reg failed");
    aw9523b->regs.output = value;
    return ESP_OK;
}

static esp_err_t read_output_reg(esp_io_expander_handle_t handle, uint32_t *value)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);

    *value = aw9523b->regs.output;
    return ESP_OK;
}

static esp_err_t write_direction_reg(esp_io_expander_handle_t handle, uint32_t value)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);
    value &= 0xff;
    aw9523b->regs.direction = value;
    return ESP_OK;
}

static esp_err_t read_direction_reg(esp_io_expander_handle_t handle, uint32_t *value)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);

    *value = aw9523b->regs.direction;
    return ESP_OK;
}

static esp_err_t reset(esp_io_expander_t *handle)
{
    ESP_RETURN_ON_ERROR(write_output_reg(handle, OUT_REG_DEFAULT_VAL), TAG, "Write output reg failed");
    return ESP_OK;
}

static esp_err_t del(esp_io_expander_t *handle)
{
    esp_io_expander_aw9523b_t *aw9523b = (esp_io_expander_aw9523b_t *)__containerof(handle, esp_io_expander_aw9523b_t, base);

    free(aw9523b);
    return ESP_OK;
}