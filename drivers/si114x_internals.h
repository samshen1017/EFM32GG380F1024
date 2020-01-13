/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si114x
 * @brief       Internal addresses, registers, constants for the Si114x sensors family.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the Si114x sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SI114X_INTERNALS_H
#define SI114X_INTERNALS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Si114x I2C address
 */
#define SI114X_ADDR (0x5A)

/**
 * @name Si114x commands
 */
#define CMD_PARAM_QUERY (0x80)
#define CMD_PARAM_SET (0xA0)
#define CMD_NOP (0x00)
#define CMD_RESET (0x01)
#define CMD_BUSADDR (0x02)
#define CMD_PS_FORCE (0x05)
#define CMD_ALS_FORCE (0x06)
#define CMD_PS_ALS_FORCE (0x07)
#define CMD_PS_PAUSE (0x09)
#define CMD_ALS_PAUSE (0x0A)
#define CMD_PS_ALS_PAUSE (0x0B)
#define CMD_PS_AUTO (0x0D)
#define CMD_ALS_AUTO (0x0E)
#define CMD_PS_ALS_AUTO (0x0F)
#define CMD_GET_CAL (0x12)

/**
 * @name Si114x registers
 */
#define REG_PART_ID (0x00)
#define REG_REV_ID (0x01)
#define REG_SEQ_ID (0x02)
#define REG_INT_CFG (0x03)
#define REG_IRQ_ENABLE (0x04)
#define REG_IRQ_MODE1 (0x05)
#define REG_IRQ_MODE2 (0x06)
#define REG_HW_KEY (0x07)
#define REG_MEAS_RATE (0x08)
#define REG_ALS_RATE (0x09)
#define REG_PS_RATE (0x0A)
#define REG_ALS_LOW_TH0 (0x0B)
#define REG_ALS_LOW_TH1 (0x0C)
#define REG_ALS_HI_TH0 (0x0D)
#define REG_ALS_HI_TH1 (0x0E)
#define REG_PS_LED21 (0x0F)
#define REG_PS_LED3 (0x10)
#define REG_PS1_TH0 (0x11)
#define REG_PS1_TH1 (0x12)
#define REG_PS2_TH0 (0x13)
#define REG_PS2_TH1 (0x14)
#define REG_PS3_TH0 (0x15)
#define REG_PS3_TH1 (0x16)
#define REG_PARAM_WR (0x17)
#define REG_COMMAND (0x18)
#define REG_RESPONSE (0x20)
#define REG_IRQ_STATUS (0x21)
#define REG_ALS_VIS_DATA0 (0x22)
#define REG_ALS_VIS_DATA1 (0x23)
#define REG_ALS_IR_DATA0 (0x24)
#define REG_ALS_IR_DATA1 (0x25)
#define REG_PS1_DATA0 (0x26)
#define REG_PS1_DATA1 (0x27)
#define REG_PS2_DATA0 (0x28)
#define REG_PS2_DATA1 (0x29)
#define REG_PS3_DATA0 (0x2A)
#define REG_PS3_DATA1 (0x2B)
#define REG_UV_INDEX0 (0x2C)
#define REG_UV_INDEX1 (0x2D)
#define REG_PARAM_RD (0x2E)
#define REG_CHIP_STAT (0x30)

/**
 * @name Si114x response register error codes
 */
#define RESPONSE_NO_ERROR (0x00)
#define RESPONSE_INVALID_SETTING (0x80)
#define RESPONSE_PS1_ADC_OVERFLOW (0x88)
#define RESPONSE_PS2_ADC_OVERFLOW (0x89)
#define RESPONSE_PS3_ADC_OVERFLOW (0x8A)
#define RESPONSE_ALS_VIS_ADC_OVERFLOW (0x8C)
#define RESPONSE_ALS_IR_ADC_OVERFLOW (0x8D)
#define RESPONSE_UV_ADC_OVERFLOW (0x8E)

/**
 * @name Si114x parameters RAM
 *
 * @note These parameter can be accessed indirectly using PARAM_QUERY or
 *       PARAM_SET commands.
 */
#define PARAM_I2C_ADDR 0x00
#define PARAM_CH_LIST 0x01
#define PARAM_PSLED12_SELECT 0x02
#define PARAM_PSLED3_SELECT 0x03
#define PARAM_FILTER_EN 0x04
#define PARAM_PS_ENCODING 0x05
#define PARAM_ALS_ENCODING 0x06
#define PARAM_PS1_ADC_MUX 0x07
#define PARAM_PS2_ADC_MUX 0x08
#define PARAM_PS3_ADC_MUX 0x09
#define PARAM_PS_ADC_COUNTER 0x0A
#define PARAM_PS_ADC_CLKDIV 0x0B
#define PARAM_PS_ADC_GAIN 0x0B
#define PARAM_PS_ADC_MISC 0x0C
#define PARAM_VIS_ADC_MUX 0x0D
#define PARAM_IR_ADC_MUX 0x0E
#define PARAM_AUX_ADC_MUX 0x0F
#define PARAM_ALSVIS_ADC_COUNTER 0x10
#define PARAM_ALSVIS_ADC_CLKDIV 0x11
#define PARAM_ALSVIS_ADC_GAIN 0x11
#define PARAM_ALSVIS_ADC_MISC 0x12
#define PARAM_ALS_HYST 0x16
#define PARAM_PS_HYST 0x17
#define PARAM_PS_HISTORY 0x18
#define PARAM_ALS_HISTORY 0x19
#define PARAM_ADC_OFFSET 0x1A
#define PARAM_SLEEP_CTRL 0x1B
#define PARAM_LED_RECOVERY 0x1C
#define PARAM_ALSIR_ADC_COUNTER 0x1D
#define PARAM_ALSIR_ADC_CLKDIV 0x1E
#define PARAM_ALSIR_ADC_GAIN 0x1E
#define PARAM_ALSIR_ADC_MISC 0x1F

/**
 * @name Si114x constants
 */
#define SI1141_ID 0x41
#define SI1145_ID 0x45
#define SI1146_ID 0x46
#define SI1147_ID 0x47
#define SI114X_HW_KEY_VAL0 0x17

/**
 * @name Si114x register bits
 * @{
 */
#define SI114X_EN_AUX (0x40)
#define SI114X_EN_ALS_IR (0x20)
#define SI114X_EN_ALS_VIS (0x10)
#define SI114X_EN_PS1 (0x01)
#define SI114X_EN_PS2 (0x02)
#define SI114X_EN_PS3 (0x04)

#ifdef __cplusplus
}
#endif

#endif /* SI114X_INTERNALS_H */
