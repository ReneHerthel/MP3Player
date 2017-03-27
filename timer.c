/**
 * @{
 *
 * @brief     Definitions for the main application
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <stm32f4xx.h>

#include "driver/timer.h"
#include "driver/config/periph_conf.h"

/** Type for timer state */
typedef struct {
    void (*cb)(void);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[NUM_OF_TIMER];

void timer_init(tim_t dev, void (*cb)(void))
{
		TIM_TypeDef *tim;
	  GPIO_TypeDef *port;
	  uint32_t pins[PWM_MAX_CHANNELS];
	  int channels = 0, af, i;
	
		switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_CLKEN();
            NVIC_SetPriority( TIMER_0_IRQ, 1 );
            tim = TIMER_0_DEV;
            tim->PSC = TIMER_0_PRESCALER;
            tim->ARR = TIMER_0_ARR;					            
				    /* PWM config */		    
				    port = PWM_0_PORT;
            PWM_0_PORT_CLKEN();
						pins[0] = PWM_0_PIN_CH0N;
#if (PWM_0_CHANNELS > 1)
            pins[1] = PWM_0_PIN_CH1N;
#endif	
				    /* enable PWM outputs */
						tim->BDTR = TIM_BDTR_MOE;
						tim->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
						tim->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
						/* enable output on PWM pins */
						tim->CCER = TIM_CCER_CC3NE | TIM_CCER_CC2NE;
				    channels = PWM_0_CHANNELS;
				    af = PWM_0_PIN_AF;
            break;
#endif
#if TIMER_1_EN
				case TIMER_1:
						TIMER_1_CLKEN();
						NVIC_SetPriority( TIMER_1_IRQ, 2);
						tim = TIMER_1_DEV;
						tim->PSC = TIMER_1_PRESCALER;
						tim->ARR = TIMER_1_ARR;
						break;
#endif
		}
		
    /* setup pins: alternate function */
    for (i = 0; i < channels; i++) {
        port->MODER &= ~(3 << (pins[i] * 2));
        port->MODER |= (2 << (pins[i] * 2));
        if (pins[i] < 8) {
            port->AFR[0] &= ~(0xf << (pins[i] * 4));
            port->AFR[0] |= (af << (pins[i] * 4));
        } else {
            port->AFR[1] &= ~(0xf << ((pins[i] - 8) * 4));
            port->AFR[1] |= (af << ((pins[i] - 8) * 4));
        }
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

		config[dev].cb = cb;	 /**< set callback function */

    tim->CR1 = 0;
    tim->CR2 = 0;
    tim->DIER = TIM_DIER_UIE;
    /* set PWM mode **************************************************/
	  tim->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
    tim->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
    /* enable output on PWM pins */
    tim->CCER = TIM_CCER_CC3NE | TIM_CCER_CC2NE;
    /* enable PWM outputs */
    tim->BDTR = TIM_BDTR_MOE;

    tim->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
		timer_irq_enable(dev);
		timer_start(dev);
}

void timer_stop(tim_t dev)
{
	  switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CR1 &= ~TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CR1 &= ~TIM_CR1_CEN;
            break;
#endif
    }
}

void timer_start(tim_t dev)
{
	  switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            TIMER_0_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
#if TIMER_1_EN
        case TIMER_1:
            TIMER_1_DEV->CR1 |= TIM_CR1_CEN;
            break;
#endif
		}
}

int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN
        case TIMER_0:
            return TIMER_0_DEV->CNT;
#endif
#if TIMER_1_EN
        case TIMER_1:
            return TIMER_1_DEV->CNT;
#endif
        default:
            return -1;
    }
}

void timer_irq_enable(tim_t dev)
{
		switch (dev) {
#if TIMER_0_EN
				case TIMER_0:
					  NVIC_EnableIRQ( TIMER_0_IRQ );
						break;
#endif
#if TIMER_1_EN
				case TIMER_1:
						NVIC_EnableIRQ( TIMER_1_IRQ );
						break;
#endif
#if TIMER_2_EN
				case TIMER_2:
						NVIC_EnableIRQ( TIMER_2_IRQ );
						break;
#endif
		}
}

void timer_irq_disable(tim_t dev)
{
	  switch (dev) {
#if TIMER_0_EN
		    case TIMER_0:
			      NVIC_DisableIRQ( TIMER_0_IRQ );
			      break;
#endif
#if TIMER_1_EN
		    case TIMER_1:
			      NVIC_DisableIRQ( TIMER_1_IRQ );
			      break;
#endif
#if TIMER_2_EN
				case TIMER_2:
						NVIC_EnableIRQ( TIMER_2_IRQ );
						break;
#endif
		}
}

void TIMER_0_ISR(void)
{
    if (TIMER_0_DEV->SR & TIM_SR_UIF) {
        TIMER_0_DEV->SR &= ~TIM_SR_UIF;
        config[TIMER_0].cb();
    }
}

void TIMER_1_ISR(void)
{
	  if (TIMER_1_DEV->SR & TIM_SR_UIF) {
	  	  TIMER_1_DEV->SR &= ~TIM_SR_UIF;
	  	  config[TIMER_1].cb();
	  }
}

void TIMER_2_ISR(void)
{
	  if(TIMER_2_DEV->SR & TIM_SR_UIF) {
			 TIMER_2_DEV->SR &= ~TIM_SR_UIF;
			 config[TIMER_2].cb();
		}
}
