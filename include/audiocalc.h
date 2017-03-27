/**
 * @{
 *
 * @brief     Definitions of Calculator for DAC and PWM vaules
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef AUDIOCALC_H
#define AUDIOCALC_H

#include <stdint.h>

/**
 * @brief Calculate PWM value
 */
uint16_t calc_pwm(int16_t value);

/**
 * @brief Calculate DAC value
 */
uint16_t calc_dac(int16_t value);

#endif /* AUDIOCALC_H */
