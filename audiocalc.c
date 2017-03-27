/**
 * @{
 *
 * @brief     Calculator for DAC and PWM vaules
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include "include/audiocalc.h"
#include "driver/config/periph_conf.h"

#define BIT                     (1)
#define SHIFT_16                (16)
#define SHIFT_15                (15)
#define SHIFT_12                (12)

#define MIN_VALUE               (BIT << SHIFT_15)         /**< (32768) */
#define VALUE_SPAN              ((BIT << SHIFT_16) - BIT) /**< ((65536) - 1) */
#define DAC_MAX_VAL             ((BIT << SHIFT_12) - BIT) /**< ((4096) - 1) */
#define SCALE_PWM               ((((float)SYS_FREQ / TIMER_FREQ) - 1) / VALUE_SPAN)
#define SCALE_DAC               ((float)DAC_MAX_VAL / VALUE_SPAN)

uint16_t calc_pwm(int16_t value)
{
    return (uint16_t)(((value + MIN_VALUE) * SCALE_PWM));
}

uint16_t calc_dac(int16_t value)
{
    return (uint16_t)(((value + MIN_VALUE) * SCALE_DAC));
}
