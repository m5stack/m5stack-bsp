#pragma once

/*
 *   AW9523B_REGISTER NAME_REG_REGISTER ADDRESS
 */

#define AW9523B_P0_IN_STAT          0x00    /*P0 Input Status*/
#define AW9523B_P1_IN_STAT          0x01    /*P1 Input Status*/
#define AW9523B_P0_OUT_STAT         0x02    /*P0 Output Status*/
#define AW9523B_P1_OUT_STAT         0x03    /*P1 Output Status*/
#define AW9523B_P0_IO_CONF          0x04    /*P0 Input/Output Configuration*/
#define AW9523B_P1_IO_CONF          0x05    /*P1 Input/Output Configuration*/
#define AW9523B_P0_INT_EN           0x06    /*P0 Interruption Enable*/
#define AW9523B_P1_INT_EN           0x07    /*P1 Interruption Enable*/

#define AW9523B_ID_REG              0x10    /*ID Register(Read Only)*/
#define AW9523B_CTL_REG             0x11    /*Globle Control Register*/
#define AW9523B_P0_MODE             0x12    /*P0_7~P0_0 Mode Switch*/
#define AW9523B_P1_MODE             0x13    /*P1_7~P1_0 Mode Switch*/

#define AW9523B_P1_0_CTL            0x20    /*P1_0 Current Control*/
#define AW9523B_P1_1_CTL            0x21    /*P1_1 Current Control*/
#define AW9523B_P1_2_CTL            0x22    /*P1_2 Current Control*/
#define AW9523B_P1_3_CTL            0x23    /*P1_3 Current Control*/
#define AW9523B_P0_0_CTL            0x24    /*P0_0 Current Control*/
#define AW9523B_P0_1_CTL            0x25    /*P0_1 Current Control*/
#define AW9523B_P0_2_CTL            0x26    /*P0_2 Current Control*/
#define AW9523B_P0_3_CTL            0x27    /*P0_3 Current Control*/
#define AW9523B_P0_4_CTL            0x28    /*P0_4 Current Control*/
#define AW9523B_P0_5_CTL            0x29    /*P0_5 Current Control*/
#define AW9523B_P0_6_CTL            0x2A    /*P0_6 Current Control*/
#define AW9523B_P0_7_CTL            0x2B    /*P0_7 Current Control*/
#define AW9523B_P1_4_CTL            0x2C    /*P1_4 Current Control*/
#define AW9523B_P1_5_CTL            0x2D    /*P1_5 Current Control*/
#define AW9523B_P1_6_CTL            0x2E    /*P1_6 Current Control*/
#define AW9523B_P1_7_CTL            0x2F    /*P1_7 Current Control*/

#define AW9523B_SW_RESET            0xFF    /*Soft Reset Control*/


