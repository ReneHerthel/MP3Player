/**
 * @{
 *
 * @brief     Main application for memory components
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stm32f4xx.h>

#include "driver/pwm.h"

int pwm_init(pwm_t dev)
{
    TIM_TypeDef *tim = 0;
	  GPIO_TypeDef *port;
	  uint32_t pins[PWM_MAX_CHANNELS];
	  int channels = 0, af, hl, i;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
        tim = PWM_0_DEV;
		port = PWM_0_PORT;
        PWM_0_PORT_CLKEN();
		pins[0] = PWM_0_PIN_CH0N;
#if (PWM_0_CHANNELS > 1)
        pins[1] = PWM_0_PIN_CH1N;
#endif
		af = PWM_0_PIN_AF;
		channels = PWM_0_CHANNELS;
		break;
#endif
    }

    /* setup pins: alternate function */
    for (i = 0; i < channels; i++) {
        port->MODER &= ~(3 << (pins[i] * 2));
        port->MODER |= (2 << (pins[i] * 2));
	    hl = (pins[i] < 8) ? 0 : 1;
	    port->AFR[hl] &= ~(0xf << ((pins[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pins[i] - (hl * 8)) * 4));
    }
		
		/* Reset C/C and timer configuration register */
    switch (channels) {
        case 2:
            tim->CCR2 = 0;
            /* Fall through */
        case 1:
            tim->CCR1 = 0;
            break;
    }
		
    /* set PWM mode */
	  tim->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
    tim->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
    /* enable output on PWM pins */
    tim->CCER = TIM_CCER_CC3NE | TIM_CCER_CC2NE;
    /* enable PWM outputs */
    tim->BDTR = TIM_BDTR_MOE;

    return 0;
}

int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    TIM_TypeDef *tim = NULL;

    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            tim = PWM_0_DEV;
            if (channel >= PWM_0_CHANNELS) {
                return -1;
            }
            break;
#endif
    }

    /* norm value to maximum possible value */
    if (value > tim->ARR) {
        value = (unsigned int) tim->ARR;
    }

    switch (channel) {
        case 0:
            tim->CCR2 = value;
            break;
        case 1:
            tim->CCR3 = value;
            break;
        default:
            return -1;
    }

    return 0;
}

void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
      case PWM_0:
          PWM_0_DEV->CR1 |= TIM_CR1_CEN;
          break;
#endif
#if PWM_1_EN
      case PWM_1:
          PWM_1_DEV->CR1 |= TIM_CR1_CEN;
          break;
#endif
    }
}

void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN
        case PWM_0:
            PWM_0_DEV->CR1 &= ~(TIM_CR1_CEN);
            break;
#endif
#if PWM_1_EN
        case PWM_1:
            PWM_1_DEV->CR1 &= ~(TIM_CR1_CEN);
            break;
#endif
    }
}
