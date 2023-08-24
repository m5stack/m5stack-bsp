#include <string.h>
#include "aw88298.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"

#include "aw88298_config.h"

typedef struct {
    i2c_port_t port;
    uint16_t dev_addr;
} aw88298_dev_t;

/*
 * Clock coefficient structure
 */
struct _coeff_div {
    uint32_t mclk;        /* mclk frequency */
    uint32_t rate;        /* sample rate */
    uint8_t pre_div;      /* the pre divider with range from 1 to 8 */
    uint8_t pre_multi;    /* the pre multiplier with 0: 1x, 1: 2x, 2: 4x, 3: 8x selection */
    uint8_t adc_div;      /* adcclk divider */
    uint8_t dac_div;      /* dacclk divider */
    uint8_t fs_mode;      /* double speed or single speed, =0, ss, =1, ds */
    uint8_t lrck_h;       /* adclrck divider and daclrck divider */
    uint8_t lrck_l;
    uint8_t bclk_div;     /* sclk divider */
    uint8_t adc_osr;      /* adc osr */
    uint8_t dac_osr;      /* dac osr */
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
    /*!<mclk     rate   pre_div  mult  adc_div dac_div fs_mode lrch  lrcl  bckdiv osr */
    /* 8k */
    {12288000, 8000, 0x06, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 8000, 0x03, 0x01, 0x03, 0x03, 0x00, 0x05, 0xff, 0x18, 0x10, 0x10},
    {16384000, 8000, 0x08, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000, 8000, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 8000, 0x03, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000, 8000, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 8000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000, 8000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 8000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1024000, 8000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 11.025k */
    {11289600, 11025, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800, 11025, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400, 11025, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200, 11025, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 12k */
    {12288000, 12000, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 12000, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 12000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 16k */
    {12288000, 16000, 0x03, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 16000, 0x03, 0x01, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x10},
    {16384000, 16000, 0x04, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000, 16000, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 16000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000, 16000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 16000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000, 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 16000, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1024000, 16000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 22.05k */
    {11289600, 22050, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800, 22050, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400, 22050, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200, 22050, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {705600, 22050, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 24k */
    {12288000, 24000, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 24000, 0x03, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 24000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 24000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 32k */
    {12288000, 32000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 32000, 0x03, 0x02, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x10},
    {16384000, 32000, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000, 32000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000, 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 32000, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000, 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 32000, 0x03, 0x03, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
    {1024000, 32000, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 44.1k */
    {11289600, 44100, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400, 44100, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200, 44100, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 48k */
    {12288000, 48000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 48000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 48000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 48000, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 64k */
    {12288000, 64000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 64000, 0x03, 0x02, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {16384000, 64000, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000, 64000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 64000, 0x01, 0x02, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {4096000, 64000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 64000, 0x01, 0x03, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {2048000, 64000, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 64000, 0x01, 0x03, 0x01, 0x01, 0x01, 0x00, 0xbf, 0x03, 0x18, 0x18},
    {1024000, 64000, 0x01, 0x03, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 88.2k */
    {11289600, 88200, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800, 88200, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400, 88200, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200, 88200, 0x01, 0x03, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 96k */
    {12288000, 96000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 96000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000, 96000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000, 96000, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000, 96000, 0x01, 0x03, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
};

static const char *TAG = "aw88298";

static inline esp_err_t aw88298_write_reg(aw88298_handle_t dev, uint8_t reg_addr, uint8_t data)
{
    aw88298_dev_t *es = (aw88298_dev_t *) dev;
    const uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(es->port, es->dev_addr, write_buf, sizeof(write_buf), pdMS_TO_TICKS(1000));
}

static inline esp_err_t aw88298_read_reg(aw88298_handle_t dev, uint8_t reg_addr, uint8_t *reg_value)
{
    aw88298_dev_t *es = (aw88298_dev_t *) dev;
    return i2c_master_write_read_device(es->port, es->dev_addr, &reg_addr, 1, reg_value, 1, pdMS_TO_TICKS(1000));
}

/*
* look for the coefficient in coeff_div[] table
*/
static int get_coeff(uint32_t mclk, uint32_t rate)
{
    for (int i = 0; i < (sizeof(coeff_div) / sizeof(coeff_div[0])); i++) {
        if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk) {
            return i;
        }
    }

    return -1;
}

esp_err_t aw88298_sample_frequency_config(aw88298_handle_t dev, int mclk_frequency, int sample_frequency)
{
    uint8_t regv;

    /* Get clock coefficients from coefficient table */
    int coeff = get_coeff(mclk_frequency, sample_frequency);

    if (coeff < 0) {
        ESP_LOGE(TAG, "Unable to configure sample rate %dHz with %dHz MCLK", sample_frequency, mclk_frequency);
        return ESP_ERR_INVALID_ARG;
    }

    const struct _coeff_div *const selected_coeff = &coeff_div[coeff];

    /* register 0x02 */
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_CLK_MANAGER_REG02, &regv), TAG, "I2C read/write error");
    regv &= 0x07;
    regv |= (selected_coeff->pre_div - 1) << 5;
    regv |= selected_coeff->pre_multi << 3;
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG02, regv), TAG, "I2C read/write error");

    /* register 0x03 */
    const uint8_t reg03 = (selected_coeff->fs_mode << 6) | selected_coeff->adc_osr;
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG03, reg03), TAG, "I2C read/write error");

    /* register 0x04 */
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG04, selected_coeff->dac_osr), TAG, "I2C read/write error");

    /* register 0x05 */
    const uint8_t reg05 = ((selected_coeff->adc_div - 1) << 4) | (selected_coeff->dac_div - 1);
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG05, reg05), TAG, "I2C read/write error");

    /* register 0x06 */
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_CLK_MANAGER_REG06, &regv), TAG, "I2C read/write error");
    regv &= 0xE0;

    if (selected_coeff->bclk_div < 19) {
        regv |= (selected_coeff->bclk_div - 1) << 0;
    } else {
        regv |= (selected_coeff->bclk_div) << 0;
    }

    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG06, regv), TAG, "I2C read/write error");

    /* register 0x07 */
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_CLK_MANAGER_REG07, &regv), TAG, "I2C read/write error");
    regv &= 0xC0;
    regv |= selected_coeff->lrck_h << 0;
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG07, regv), TAG, "I2C read/write error");

    /* register 0x08 */
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG08, selected_coeff->lrck_l), TAG, "I2C read/write error");

    return ESP_OK;
}

static esp_err_t aw88298_clock_config(aw88298_handle_t dev, const aw88298_clock_config_t *const clk_cfg, aw88298_resolution_t res)
{
    uint8_t reg06;
    uint8_t reg01 = 0x3F; // Enable all clocks
    int mclk_hz;

    /* Select clock source for internal MCLK and determine its frequency */
    if (clk_cfg->mclk_from_mclk_pin) {
        mclk_hz = clk_cfg->mclk_frequency;
    } else {
        mclk_hz = clk_cfg->sample_frequency * (int)res * 2;
        reg01 |= BIT(7); // Select BCLK (a.k.a. SCK) pin
    }

    if (clk_cfg->mclk_inverted) {
        reg01 |= BIT(6); // Invert MCLK pin
    }
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG01, reg01), TAG, "I2C read/write error");

    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_CLK_MANAGER_REG06, &reg06), TAG, "I2C read/write error");
    if (clk_cfg->sclk_inverted) {
        reg06 |= BIT(5);
    } else {
        reg06 &= ~BIT(5);
    }
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_CLK_MANAGER_REG06, reg06), TAG, "I2C read/write error");

    /* Configure clock dividers */
    return aw88298_sample_frequency_config(dev, mclk_hz, clk_cfg->sample_frequency);
}

static esp_err_t aw88298_resolution_config(const aw88298_resolution_t res, uint8_t *reg)
{
    switch (res) {
    case aw88298_RESOLUTION_16:
        *reg |= (3 << 2);
        break;
    case aw88298_RESOLUTION_18:
        *reg |= (2 << 2);
        break;
    case aw88298_RESOLUTION_20:
        *reg |= (1 << 2);
        break;
    case aw88298_RESOLUTION_24:
        *reg |= (0 << 2);
        break;
    case aw88298_RESOLUTION_32:
        *reg |= (4 << 2);
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t aw88298_fmt_config(aw88298_handle_t dev, const aw88298_resolution_t res_in, const aw88298_resolution_t res_out)
{
    uint8_t reg09 = 0; // SDP In
    uint8_t reg0a = 0; // SDP Out

    ESP_LOGI(TAG, "aw88298 in Slave mode and I2S format");
    uint8_t reg00;
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_RESET_REG00, &reg00), TAG, "I2C read/write error");
    reg00 &= 0xBF;
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_RESET_REG00, reg00), TAG, "I2C read/write error"); // Slave serial port - default

    /* Setup SDP In and Out resolution */
    aw88298_resolution_config(res_in, &reg09);
    aw88298_resolution_config(res_out, &reg0a);

    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SDPIN_REG09, reg09), TAG, "I2C read/write error");
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SDPOUT_REG0A, reg0a), TAG, "I2C read/write error");

    return ESP_OK;
}

esp_err_t aw88298_microphone_config(aw88298_handle_t dev, bool digital_mic)
{
    uint8_t reg14 = 0x1A; // enable analog MIC and max PGA gain

    /* PDM digital microphone enable or disable */
    if (digital_mic) {
        reg14 |= BIT(6);
    }
    aw88298_write_reg(dev, aw88298_ADC_REG17, 0xC8); // Set ADC gain @todo move this to ADC config section

    return aw88298_write_reg(dev, aw88298_SYSTEM_REG14, reg14);
}

esp_err_t aw88298_init(aw88298_handle_t dev, const aw88298_clock_config_t *const clk_cfg, const aw88298_resolution_t res_in, const aw88298_resolution_t res_out)
{
    ESP_RETURN_ON_FALSE(
        (clk_cfg->sample_frequency >= 8000) && (clk_cfg->sample_frequency <= 96000),
        ESP_ERR_INVALID_ARG, TAG, "aw88298 init needs frequency in interval [8000; 96000] Hz"
    );
    if (!clk_cfg->mclk_from_mclk_pin) {
        ESP_RETURN_ON_FALSE(res_out == res_in, ESP_ERR_INVALID_ARG, TAG, "Resolution IN/OUT must be equal if MCLK is taken from SCK pin");
    }


    /* Reset aw88298 to its default */
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_RESET_REG00, 0x1F), TAG, "I2C read/write error");
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_RESET_REG00, 0x00), TAG, "I2C read/write error");
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_RESET_REG00, 0x80), TAG, "I2C read/write error"); // Power-on command

    /* Setup clock: source, polarity and clock dividers */
    ESP_RETURN_ON_ERROR(aw88298_clock_config(dev, clk_cfg, res_out), TAG, "");

    /* Setup audio format (fmt): master/slave, resolution, I2S */
    ESP_RETURN_ON_ERROR(aw88298_fmt_config(dev, res_in, res_out), TAG, "");

    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SYSTEM_REG0D, 0x01), TAG, "I2C read/write error"); // Power up analog circuitry - NOT default
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SYSTEM_REG0E, 0x02), TAG, "I2C read/write error"); // Enable analog PGA, enable ADC modulator - NOT default
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SYSTEM_REG12, 0x00), TAG, "I2C read/write error"); // power-up DAC - NOT default
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_SYSTEM_REG13, 0x10), TAG, "I2C read/write error"); // Enable output to HP drive - NOT default
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_ADC_REG1C, 0x6A), TAG, "I2C read/write error"); // ADC Equalizer bypass, cancel DC offset in digital domain
    ESP_RETURN_ON_ERROR(aw88298_write_reg(dev, aw88298_DAC_REG37, 0x08), TAG, "I2C read/write error"); // Bypass DAC equalizer - NOT default

    return ESP_OK;
}

void aw88298_delete(aw88298_handle_t dev)
{
    free(dev);
}

esp_err_t aw88298_voice_volume_set(aw88298_handle_t dev, int volume, int *volume_set)
{
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    int reg32;
    if (volume == 0) {
        reg32 = 0;
    } else {
        reg32 = ((volume) * 256 / 100) - 1;
    }

    // provide user with real volume set
    if (volume_set != NULL) {
        *volume_set = volume;
    }
    return aw88298_write_reg(dev, aw88298_DAC_REG32, reg32);
}

esp_err_t aw88298_voice_volume_get(aw88298_handle_t dev, int *volume)
{
    uint8_t reg32;
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_DAC_REG32, &reg32), TAG, "I2C read/write error");

    if (reg32 == 0) {
        *volume = 0;
    } else {
        *volume = ((reg32 * 100) / 256) + 1;
    }
    return ESP_OK;
}

esp_err_t aw88298_voice_mute(aw88298_handle_t dev, bool mute)
{
    uint8_t reg31;
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_DAC_REG31, &reg31), TAG, "I2C read/write error");

    if (mute) {
        reg31 |= BIT(6) | BIT(5);
    } else {
        reg31 &= ~(BIT(6) | BIT(5));
    }

    return aw88298_write_reg(dev, aw88298_DAC_REG31, reg31);
}

esp_err_t aw88298_microphone_gain_set(aw88298_handle_t dev, aw88298_mic_gain_t gain_db)
{
    return aw88298_write_reg(dev, aw88298_ADC_REG16, gain_db); // ADC gain scale up
}

esp_err_t aw88298_voice_fade(aw88298_handle_t dev, const aw88298_fade_t fade)
{
    uint8_t reg37;
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_DAC_REG37, &reg37), TAG, "I2C read/write error");
    reg37 &= 0x0F;
    reg37 |= (fade << 4);
    return aw88298_write_reg(dev, aw88298_DAC_REG37, reg37);
}

esp_err_t aw88298_microphone_fade(aw88298_handle_t dev, const aw88298_fade_t fade)
{
    uint8_t reg15;
    ESP_RETURN_ON_ERROR(aw88298_read_reg(dev, aw88298_ADC_REG15, &reg15), TAG, "I2C read/write error");
    reg15 &= 0x0F;
    reg15 |= (fade << 4);
    return aw88298_write_reg(dev, aw88298_ADC_REG15, reg15);
}

void aw88298_register_dump(aw88298_handle_t dev)
{
    for (int reg = 0; reg < 0x4A; reg++) {
        uint8_t value;
        ESP_ERROR_CHECK(aw88298_read_reg(dev, reg, &value));
        printf("REG:%02x: %02x", reg, value);
    }
}

aw88298_handle_t aw88298_create(const i2c_port_t port, const uint16_t dev_addr)
{
    aw88298_dev_t *sensor = (aw88298_dev_t *) calloc(1, sizeof(aw88298_dev_t));
    sensor->port = port;
    sensor->dev_addr = dev_addr;
    return (aw88298_handle_t) sensor;
}