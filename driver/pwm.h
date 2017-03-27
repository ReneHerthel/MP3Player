/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for PWM.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef PWM_H
#define PWM_H

#include "config/periph_conf.h"

/**
 * @brief Define the default PWM type identifier
 */
typedef enum {
#ifdef PWM_0_EN
    PWM_0 = 0,
#endif
} pwm_t;

/**
 * @brief Definition of available PWM modes
 */
typedef enum {
    PWM_LEFT = 0,       /*< use left aligned PWM */
    PWM_RIGHT,          /*< use right aligned PWM */
    PWM_CENTER          /*< use center aligned PWM */
} pwm_mode_t;

/**
 * @brief
 *
 * @param[in] dev           PWM channel to initialize
 * @param[in] mode          PWM mode, left, right or center aligned
 * @param[in] frequency     the PWM frequency in Hz
 * @param[in] resolution    the PWM resolution
 *
 * @return                  Actual PWM frequency on success
 * @return                  -1 on mode not applicable
 * @return                  -2 on frequency and resolution not applicable
 */
int pwm_init(pwm_t dev);

/**
 * @brief Set the duty-cycle for a given channel of the given PWM device
 */
int pwm_set(pwm_t dev, int channel, unsigned int value);

/**
 * @brief Start PWM generation on the given device
 *
 * @param[in] dev           device to start
 */
void pwm_start(pwm_t dev);

/**
 * @brief Stop PWM generation on the given device
 *
 * @param[in] dev           device to stop
 */
void pwm_stop(pwm_t dev);

/**
 * @brief Power on the PWM device
 *
 * The PWM deice is powered on. It is dependent on the implementing platform,
 * if the previously set configuration is still available after power on.
 *
 * @param[in] dev           device to power on
 */
void pwm_poweron(pwm_t dev);

/**
 * @brief Power off the given PWM device
 *
 * The given PWM is completely powered off. On most platform this means, that
 * the clock for the PWM device is disabled.
 *
 * @param[in] dev           device to power off
 */
void pwm_poweroff(pwm_t dev);

#endif /* PWM_H */
