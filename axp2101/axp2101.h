#ifndef _AXP2101_H
#define _AXP2101_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define	AXP2101_ADDRESS	                0x34

/* Power control registers */
#define AXP2101_POWER_STATUS             0x00
#define AXP2101_CHARGE_STATUS            0x01
#define AXP2101_OTG_VBUS_STATUS          0x04
#define AXP2101_DATA_BUFFER0             0x06
#define AXP2101_DATA_BUFFER1             0x07
#define AXP2101_DATA_BUFFER2             0x08
#define AXP2101_DATA_BUFFER3             0x09
#define AXP2101_DATA_BUFFER4             0x0a
#define AXP2101_DATA_BUFFER5             0x0b
/* Output control: 2 EXTEN, 0 DCDC2 */
#define AXP2101_EXTEN_DCDC2_CONTROL      0x10
/* Power output control: 6 EXTEN, 4 DCDC2, 3 LDO3, 2 LDO2, 1 DCDC3, 0 DCDC1 */
#define AXP2101_DCDC13_LDO23_CONTROL     0x12
#define AXP2101_DCDC2_VOLTAGE            0x23
#define AXP2101_DCDC2_SLOPE              0x25
#define AXP2101_DCDC1_VOLTAGE            0x26
#define AXP2101_DCDC3_VOLTAGE            0x27
/* Output voltage control: 7-4 LDO2, 3-0 LDO3 */
#define AXP2101_LDO23_VOLTAGE            0x28
#define AXP2101_VBUS_IPSOUT_CHANNEL      0x30
#define AXP2101_SHUTDOWN_VOLTAGE         0x31
#define AXP2101_SHUTDOWN_BATTERY_CHGLED_CONTROL 0x32
#define AXP2101_CHARGE_CONTROL_1         0x33
#define AXP2101_CHARGE_CONTROL_2         0x34
#define AXP2101_BATTERY_CHARGE_CONTROL   0x35
#define AXP2101_PEK                      0x36
#define AXP2101_DCDC_FREQUENCY           0x37
#define AXP2101_BATTERY_CHARGE_LOW_TEMP  0x38
#define AXP2101_BATTERY_CHARGE_HIGH_TEMP 0x39
#define AXP2101_APS_LOW_POWER1           0x3A
#define AXP2101_APS_LOW_POWER2           0x3B
#define AXP2101_BATTERY_DISCHARGE_LOW_TEMP  0x3c
#define AXP2101_BATTERY_DISCHARGE_HIGH_TEMP 0x3d
#define AXP2101_DCDC_MODE                0x80
#define AXP2101_ADC_ENABLE_1             0x82
#define AXP2101_ADC_ENABLE_2             0x83
#define AXP2101_ADC_RATE_TS_PIN          0x84
#define AXP2101_GPIO30_INPUT_RANGE       0x85
#define AXP2101_GPIO0_ADC_IRQ_RISING     0x86
#define AXP2101_GPIO0_ADC_IRQ_FALLING    0x87
#define AXP2101_TIMER_CONTROL            0x8a
#define AXP2101_VBUS_MONITOR             0x8b
#define AXP2101_TEMP_SHUTDOWN_CONTROL    0x8f

/* GPIO control registers */
#define AXP2101_GPIO0_CONTROL            0x90
#define AXP2101_GPIO0_LDOIO0_VOLTAGE     0x91
#define AXP2101_GPIO1_CONTROL            0x92
#define AXP2101_GPIO2_CONTROL            0x93
#define AXP2101_GPIO20_SIGNAL_STATUS     0x94
#define AXP2101_GPIO43_FUNCTION_CONTROL  0x95
#define AXP2101_GPIO43_SIGNAL_STATUS     0x96
#define AXP2101_GPIO20_PULLDOWN_CONTROL  0x97
#define AXP2101_PWM1_FREQUENCY           0x98
#define AXP2101_PWM1_DUTY_CYCLE_1        0x99
#define AXP2101_PWM1_DUTY_CYCLE_2        0x9a
#define AXP2101_PWM2_FREQUENCY           0x9b
#define AXP2101_PWM2_DUTY_CYCLE_1        0x9c
#define AXP2101_PWM2_DUTY_CYCLE_2        0x9d
#define AXP2101_N_RSTO_GPIO5_CONTROL     0x9e

/* Interrupt control registers */
#define AXP2101_ENABLE_CONTROL_1         0x40
#define AXP2101_ENABLE_CONTROL_2         0x41
#define AXP2101_ENABLE_CONTROL_3         0x42
#define AXP2101_ENABLE_CONTROL_4         0x43
#define AXP2101_ENABLE_CONTROL_5         0x4a
#define AXP2101_IRQ_STATUS_1             0x44
#define AXP2101_IRQ_STATUS_2             0x45
#define AXP2101_IRQ_STATUS_3             0x46
#define AXP2101_IRQ_STATUS_4             0x47
#define AXP2101_IRQ_STATUS_5             0x4d

/* ADC data registers */
#define AXP2101_ACIN_VOLTAGE             0x56
#define AXP2101_ACIN_CURRENT             0x58
#define AXP2101_VBUS_VOLTAGE             0x5a
#define AXP2101_VBUS_CURRENT             0x5c
#define AXP2101_TEMP                     0x5e
#define AXP2101_TS_INPUT                 0x62
#define AXP2101_GPIO0_VOLTAGE            0x64
#define AXP2101_GPIO1_VOLTAGE            0x66
#define AXP2101_GPIO2_VOLTAGE            0x68
#define AXP2101_GPIO3_VOLTAGE            0x6a
#define AXP2101_BATTERY_POWER            0x70
#define AXP2101_BATTERY_VOLTAGE          0x78
#define AXP2101_CHARGE_CURRENT           0x7a
#define AXP2101_DISCHARGE_CURRENT        0x7c
#define AXP2101_APS_VOLTAGE              0x7e
#define AXP2101_CHARGE_COULOMB           0xb0
#define AXP2101_DISCHARGE_COULOMB        0xb4
#define AXP2101_COULOMB_COUNTER_CONTROL  0xb8

/* Computed ADC */
#define AXP2101_COULOMB_COUNTER          0xff

/* IOCTL commands */
#define	AXP2101_READ_POWER_STATUS        0x0001
#define	AXP2101_READ_CHARGE_STATUS       0x0101

#define AXP2101_COULOMB_COUNTER_ENABLE   0xb801
#define AXP2101_COULOMB_COUNTER_DISABLE  0xb802
#define AXP2101_COULOMB_COUNTER_SUSPEND  0xb803
#define AXP2101_COULOMB_COUNTER_CLEAR    0xb804

#define AXP2101_LDOIO0_ENABLE            0x9000
#define AXP2101_LDOIO0_DISABLE           0x9001

#define AXP2101_DCDC2_ENABLE             0x1000
#define AXP2101_DCDC2_DISABLE            0x1001
#define AXP2101_EXTEN_ENABLE             0x1002
#define AXP2101_EXTEN_DISABLE            0x1003

#define AXP2101_LDO2_ENABLE              0x1200
#define AXP2101_LDO2_DISABLE             0x1201
#define AXP2101_LDO3_ENABLE              0x1202
#define AXP2101_LDO3_DISABLE             0x1203
#define AXP2101_DCDC1_ENABLE             0x1204
#define AXP2101_DCDC1_DISABLE            0x1205
#define AXP2101_DCDC3_ENABLE             0x1206
#define AXP2101_DCDC3_DISABLE            0x1207

#define AXP2101_DCDC1_SET_VOLTAGE        0x2600
#define AXP2101_DCDC2_SET_VOLTAGE        0x2300
#define AXP2101_DCDC3_SET_VOLTAGE        0x2700
#define AXP2101_LDO2_SET_VOLTAGE         0x2800
#define AXP2101_LDO3_SET_VOLTAGE         0x2801
#define AXP2101_LDOIO0_SET_VOLTAGE       0x9100

#define AXP2101_LOW                      0
#define AXP2101_HIGH                     1

#define AXP2101_GPIO0_SET_LEVEL          0x9400
#define AXP2101_GPIO1_SET_LEVEL          0x9401
#define AXP2101_GPIO2_SET_LEVEL          0x9402
#define AXP2101_GPIO4_SET_LEVEL          0x9601

/* Error codes */
#define AXP2101_OK                       0
#define AXP2101_ERROR_NOTTY              -1
#define AXP2101_ERROR_EINVAL             -22
#define AXP2101_ERROR_ENOTSUP            -95

typedef struct {
    uint8_t command;
    uint8_t data[2];
    uint8_t count;
} axp2101_init_command_t;

/* These should be provided by the HAL. */
typedef struct {
    int32_t * readvoid *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size;
    int32_t * writevoid *handle, uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size;
    void *handle;
} axp2101_t;

typedef int32_t axp2101_err_t;

axp2101_err_t axp2101_initconst axp2101_t *axp;
axp2101_err_t axp2101_readconst axp2101_t *axp, uint8_t reg, void *buffer;
axp2101_err_t axp2101_writeconst axp2101_t *axp, uint8_t reg, const uint8_t *buffer;
axp2101_err_t axp2101_ioctlconst axp2101_t *axp, int command, ...;

#ifdef __cplusplus
}
#endif
#endif
