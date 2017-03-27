/**
 * @{
 *
 * @brief     Definitions for all periphal components of the application.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/*****************************************************************************
 * @brief		Timer configuration                                              *
 *****************************************************************************/
 /** Timer setup values */
#define SYS_FREQ          		  (168000000) /**< 168MHz system frequency */
#define TIMER_FREQ        		  (44100) /**< 44100Hz requested timer frequency */

/* General TIMER configuration */
#define TIMER_0_EN				      (1) // ISR & PWM
#define TIMER_1_EN				      (1) // PWM
#define TIMER_2_EN              (1) // TFT
#define NUM_OF_TIMER      		  (TIMER_0_EN + TIMER_1_EN + TIMER_2_EN)

/* TIMER 0 configuration */
#define TIMER_0					        (0)
#define TIMER_0_DEV				      TIM1
#define TIMER_0_PRESCALER	      (0U)
#define TIMER_0_ARR				      ((SYS_FREQ / TIMER_FREQ) - 1)
#define TIMER_0_CLKEN()			    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN
#define TIMER_0_CLKDIS();       RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN
#define TIMER_0_ISR				      TIM1_UP_TIM10_IRQHandler
#define TIMER_0_IRQ				      TIM1_UP_TIM10_IRQn

/* TIMER 1 configuration */
#define TIMER_1					        (1)
#define TIMER_1_DEV				      TIM8
#define TIMER_1_PRESCALER	      (4096U)
#define TIMER_1_ARR				      (131072 - 1)
#define TIMER_1_CLKEN()			    RCC->APB2ENR |= RCC_APB2ENR_TIM8EN
#define TIMER_1_CLKDIS()			  RCC->APB2ENR &= ~RCC_APB2ENR_TIM8EN
#define TIMER_1_ISR				      TIM8_UP_TIM13_IRQHandler
#define TIMER_1_IRQ				      TIM8_UP_TIM13_IRQn

/* Timer 2 configuration */
#define TIMER_2                 (2)
#define TIMER_2_DEV             TIM3
#define TIMER_2_PRESCALER       (1024U)
#define TIMER_2_ARR             (131072 - 1)
#define TIMER_2_CLKEN()			    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN
#define TIMER_2_CLKDIS()			  RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN
#define TIMER_2_ISR				      TIM3_IRQHandler
#define TIMER_2_IRQ				      TIM3_IRQn

/*****************************************************************************
 * @brief PWM configuration                                                  *
 *****************************************************************************/
#define PWM_0_EN                1
#define PWM_NUMOF               (PWM_0_EN)
#define PWM_MAX_CHANNELS        2

/* PWM 0 device configuration */
#define PWM_0_DEV               TIM1
#define PWM_0_CHANNELS          2
/* PWM 0 pin configuration */
#define PWM_0_PORT              GPIOB
#define PWM_0_PORT_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define PWM_0_PORT_CLKDIS()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN)
#define PWM_0_PIN_CH0N          0  // PB0
#define PWM_0_PIN_CH1N          1  // PB1
#define PWM_0_PIN_AF            1

/*****************************************************************************
 * @brief AT25DF641 configuration                                            *
 *****************************************************************************/
#define AT25DF641_0_EN          1
#define AT25DF641_1_EN          1
#define AT25DF641_NUMOF         (AT25DF641_0_EN + AT25DF641_1_EN)

/*****************************************************************************
 * @brief SPI configuration                                                  *
 *****************************************************************************/
/* General SPI configuration */
#define SPI_0_EN			    1
#define SPI_1_EN                0
#define SPI_2_EN                0
#define SPI_3_EN                0
#define SPI_NUMOF               (SPI_0_EN + SPI_1_EN + SPI_2_EN + SPI_3_EN)

/* SPI 0 configuration */
#define SPI_0_DEV               SPI3
#define SPI_0_CLKEN()           (RCC->APB1ENR |= RCC_APB1ENR_SPI3EN)
#define SPI_0_CLKDIS()          (RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN)

/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          GPIOC
#define SPI_0_SCK_PIN           10
#define SPI_0_SCK_AF            6
#define SPI_0_SCK_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define SPI_0_MISO_PORT         GPIOC
#define SPI_0_MISO_PIN          11
#define SPI_0_MISO_AF           6
#define SPI_0_MISO_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define SPI_0_MOSI_PORT         GPIOC
#define SPI_0_MOSI_PIN          12
#define SPI_0_MOSI_AF           6
#define SPI_0_MOSI_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)

#define WORK_CS_PORT            GPIOG
#define WORK_CS_PIN             6
#define WORK_CS_PORT_CLKEN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN)

#define ORIG_CS_PORT            GPIOB
#define ORIG_CS_PIN             9
#define ORIG_CS_PORT_CLKEN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)

/*****************************************************************************
 * @brief		DAC configuration                                                *
 *****************************************************************************/
/* General DAC configuration */
#define DAC_0_EN			          (1)
#define DAC_NUMOF               (DAC_0_EN)

/* DAC 0 configuration */
#define DAC_0_DEV				        DAC
#define DAC_0_CLKEN()     		  (RCC->APB1ENR |=  (RCC_APB1ENR_DACEN))
#define DAC_0_CLKDIS()    		  (RCC->APB1ENR &= ~(RCC_APB1ENR_DACEN))
#define DAC_0_PORT        		  GPIOA
#define DAC_0_PORT_CLKEN()   	  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)

/*****************************************************************************
 * @brief GPIO configuration                                                 *
 *****************************************************************************/
#define GPIOA_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define GPIOB_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIOC_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIOD_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define GPIOE_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN)
#define GPIOF_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN)
#define GPIOG_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN)
#define GPIOH_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN)
#define GPIOI_CLKEN()           (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN)

/*****************************************************************************
 * @brief PIN TI BOARD configuration                                         *
 *****************************************************************************/
/* Defines available output pins of the TI-Lab board in order */
#define PI7                     (0x07) 	/**< WAIT-LED */
#define PI6                     (0x06)
#define PI5                     (0x05)
#define PI4                     (0x04)
#define PH14                    (0x0E)
#define PH13                    (0x0D)
#define PH11                    (0x0B) 	/**< UNDERfLOW-LED */
#define PA0                     (0x00)

/* Defines available input pins of the TI-Lab board in order */
#define PH15                    (0x0F)
#define PH12                    (0x0C)
#define PH10                    (0x0A)
#define PF8                     (0x08)
#define PF7                     (0x07)
#define PF6                     (0x06)
#define PC2                     (0x02)
#define PI9                     (0x09)

/* Defines available DAC-Output pins */
#define DAC1                    (0x04)  /**< PA4 */
#define DAC2                    (0x05)  /**< PA5 */

#endif /* PERIPH_CONF_H */
