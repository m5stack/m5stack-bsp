#pragma once

/**
 * @brief I2C register map of the aw88298
 *
 * The 16-bit register format is as follows:
 *
 *                                       (Slave Address)
 *     ┌───────────────────────────────────────┷───────────────────────────────────────────┐
 *  ┌─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐─────┐
 *  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |
 *  └─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘─────┘
 *
 * 
 *   Caution: This driver implements only the I2C interface to AW88298 which is used for configuration of the device. User is responsible for initialization of I2S port and start I2S transaction to stream audio in/from the AW88298.
 */

#include "esp_types.h"
#include "esp_err.h"

#include "driver/i2c.h"

/* AW88298 address: CE pin low - 0x18, CE pin high - 0x19 */
#define AW88298_ADDRRES_0 0x18u
#define AW88298_ADDRESS_1 0x19u

#ifdef __cplusplus
extern "C" {
#endif

typedef void *aw88298_handle_t;

typedef enum {
    AW88298_MIC_GAIN_MIN = -1,
    AW88298_MIC_GAIN_0DB,
    AW88298_MIC_GAIN_6DB,
    AW88298_MIC_GAIN_12DB,
    AW88298_MIC_GAIN_18DB,
    AW88298_MIC_GAIN_24DB,
    AW88298_MIC_GAIN_30DB,
    AW88298_MIC_GAIN_36DB,
    AW88298_MIC_GAIN_42DB,
    AW88298_MIC_GAIN_MAX
} aw88298_mic_gain_t;

typedef enum {
    AW88298_FADE_OFF = 0,
    AW88298_FADE_4LRCK, // 4LRCK means ramp 0.25dB/4LRCK
    AW88298_FADE_8LRCK,
    AW88298_FADE_16LRCK,
    AW88298_FADE_32LRCK,
    AW88298_FADE_64LRCK,
    AW88298_FADE_128LRCK,
    AW88298_FADE_256LRCK,
    AW88298_FADE_512LRCK,
    AW88298_FADE_1024LRCK,
    AW88298_FADE_2048LRCK,
    AW88298_FADE_4096LRCK,
    AW88298_FADE_8192LRCK,
    AW88298_FADE_16384LRCK,
    AW88298_FADE_32768LRCK,
    AW88298_FADE_65536LRCK
} aw88298_fade_t;

typedef enum aw88298_resolution_t {
    AW88298_RESOLUTION_16 = 16,
    AW88298_RESOLUTION_18 = 18,
    AW88298_RESOLUTION_20 = 20,
    AW88298_RESOLUTION_24 = 24,
    AW88298_RESOLUTION_32 = 32
} aw88298_resolution_t;

typedef struct aw88298_clock_config_t {
    bool mclk_inverted;
    bool sclk_inverted;
    bool mclk_from_mclk_pin; // true: from MCLK pin (pin no. 2), false: from SCLK pin (pin no. 6)
    int  mclk_frequency;     // This parameter is ignored if MCLK is taken from SCLK pin
    int  sample_frequency;   // in Hz
} aw88298_clock_config_t;

/**
 * @brief Initialize AW88298
 *
 * There are two ways of providing Master Clock (MCLK) signal to AW88298 in Slave Mode:
 * 1. From MCLK pin:
 *    For flexible scenarios. A clock signal from I2S master is routed to MCLK pin.
 *    Its frequency must be defined in clk_cfg->mclk_frequency parameter.
 * 2. From SCLK pin:
 *    For simpler scenarios. AW88298 takes its clock from SCK pin. MCLK pin does not have to be connected.
 *    In this case, res_in must equal res_out; clk_cfg->mclk_frequency parameter is ignored
 *    and MCLK is calculated as MCLK = clk_cfg->sample_frequency * res_out * 2.
 *    Not all sampling frequencies are supported in this mode.
 *
 * @param dev AW88298 handle
 * @param[in] clk_cfg Clock configuration
 * @param[in] res_in  Input serial port resolution
 * @param[in] res_out Output serial port resolution
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Sample frequency or resolution invalid
 *     - Else fail
 */
esp_err_t aw88298_init(aw88298_handle_t dev, const aw88298_clock_config_t *const clk_cfg, const aw88298_resolution_t res_in,
                      const aw88298_resolution_t res_out);

/**
 * @brief Set output volume
 *
 * Volume paramter out of <0, 100> interval will be truncated.
 *
 * @param dev AW88298 handle
 * @param[in] volume Set volume (0 ~ 100)
 * @param[out] volume_set Volume that was set. Same as volume, unless volume is outside of <0, 100> interval.
 *                        This parameter can be set to NULL, if user does not need this information.
 *
 * @return
 *     - ESP_OK success
 *     - Else fail
 */
esp_err_t aw88298_voice_volume_set(aw88298_handle_t dev, int volume, int *volume_set);

/**
 * @brief Get output volume
 *
 * @param dev AW88298 handle
 * @param[out] volume get volume (0 ~ 100)
 *
 * @return
 *     - ESP_OK success
 *     - Else fail
 */
esp_err_t aw88298_voice_volume_get(aw88298_handle_t dev, int *volume);

/**
 * @brief Print out AW88298 register content
 *
 * @param dev AW88298 handle
 */
void aw88298_register_dump(aw88298_handle_t dev);

/**
 * @brief Mute AW88298 output
 *
 * @param dev AW88298 handle
 * @param[in] enable true: mute, false: don't mute
 * @return
 *     - ESP_OK success
 *     - Else fail
 */
esp_err_t aw88298_voice_mute(aw88298_handle_t dev, bool enable);

/**
 * @brief Set Microphone gain
 *
 * @param dev AW88298 handle
 * @param[in] gain_db Microphone gain
 * @return
 *     - ESP_OK success
 *     - Else fail
 */
esp_err_t aw88298_microphone_gain_set(aw88298_handle_t dev, aw88298_mic_gain_t gain_db);

/**
 * @brief Configure microphone
 *
 * @param dev AW88298 handle
 * @param[in] digital_mic Set to true for digital microphone
 * @return
 *     - ESP_OK success
 *     - Else fail
 */
esp_err_t aw88298_microphone_config(aw88298_handle_t dev, bool digital_mic);

/**
 * @brief Configure sampling frequency
 *
 * @note This function is called by aw88298_init().
 *       Call this function explicitly only if you want to change sample frequency during runtime.
 * @param dev AW88298 handle
 * @param[in] mclk_frequency   MCLK frequency in [Hz] (MCLK or SCLK pin, depending on bit register01[7])
 * @param[in] sample_frequency Required sample frequency in [Hz], e.g. 44100, 22050...
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG cannot set clock dividers for given MCLK and sampling frequency
 *     - Else I2C read/write error
 */
esp_err_t aw88298_sample_frequency_config(aw88298_handle_t dev, int mclk_frequency, int sample_frequency);

/**
 * @brief Configure fade in/out for ADC: voice
 *
 * @param dev AW88298 handle
 * @param[in] fade Fade ramp rate
 * @return
 *     - ESP_OK success
 *     - Else I2C read/write error
 */
esp_err_t aw88298_voice_fade(aw88298_handle_t dev, const aw88298_fade_t fade);

/**
 * @brief Configure fade in/out for DAC: microphone
 *
 * @param dev AW88298 handle
 * @param[in] fade Fade ramp rate
 * @return
 *     - ESP_OK success
 *     - Else I2C read/write error
 */
esp_err_t aw88298_microphone_fade(aw88298_handle_t dev, const aw88298_fade_t fade);

/**
 * @brief Create AW88298 object and return its handle
 *
 * @param[in] port     I2C port number
 * @param[in] dev_addr I2C device address of AW88298
 *
 * @return
 *     - NULL Fail
 *     - Others Success
 */
aw88298_handle_t aw88298_create(const i2c_port_t port, const uint16_t dev_addr);

/**
 * @brief Delete AW88298 object
 *
 * @param dev AW88298 handle
 */
void aw88298_delete(aw88298_handle_t dev);

#ifdef __cplusplus
}
#endif