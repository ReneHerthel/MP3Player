/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for GPIO.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef GPIO_H
#define GPIO_H

#include <stm32f4xx.h>

/**
 * @brief   Define the default GPIO type identifier
 */
typedef int gpio_t;

/**
 * @brief   Otyper enumartions
 */
typedef enum {
    GPIO_OT_PUSHPUll  = 0x00,	/**< enable pushpull 		*/
    GPIO_OT_OPENDRAIN = 0x01,	/**< enable open drain	*/
} gpio_ot_t;

/**
 * @brief   Ospeedr enumartions
 */
typedef enum {
    GPIO_SPEED_LOW		= 0x01,	/**<  2Mhz speed 				*/
    GPIO_SPEED_MEDIUM = 0x02,	/**< 10Mhz speed 				*/
    GPIO_SPEED_HIGH   = 0x03,	/**< 50Mhz speed				*/
} gpio_ospeedr_t;

/**
 * @brief   Moder enumartions
 */
typedef enum {
    GPIO_MODE_IN     = 0x00,	/**< Input  						*/
    GPIO_MODE_OUT    = 0x01,	/**< Output							*/
    GPIO_MODE_AF     = 0x02,	/**< Alternate function */
    GPIO_MODE_ANALOG = 0x03,	/**< analog							*/
} gpio_moder_t;

/**
 * @brief   Definition of available pin directions
 */
typedef enum {
    GPIO_DIR_IN = 0,        /**< configure pin as input  */
    GPIO_DIR_OUT = 1,       /**< configure pin as output */
} gpio_dir_t;

/**
 * @brief   Definition of pull-up/pull-down modes
 */
typedef enum {
    GPIO_NOPULL = 0,        /**< do not use internal pull resistors */
    GPIO_PULLUP = 1,        /**< enable internal pull-up resistor 	*/
    GPIO_PULLDOWN = 2       /**< enable internal pull-down resistor */
} gpio_pp_t;

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 */
typedef enum {
    GPIO_FALLING = 0,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank  */
    GPIO_BOTH = 2           /**< emit interrupt on both flanks 	 */
} gpio_flank_t;

/**
 * @brief Initialize a pin as input or output
 *
 * @param[in] dir				pin direction
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 *
 * @return               0 on success
 * @return              -1 on errur
 */
int gpio_init(gpio_dir_t dir, GPIO_TypeDef *port, gpio_t pin);

/**
 * @brief Reads the value from a pin
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 */
int gpio_read(GPIO_TypeDef *port, gpio_t pin);

/**
 * @brief Set a pin (set to HIGH)
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 */
void gpio_set(GPIO_TypeDef *port, gpio_t pin);

/**
 * @brief clear a pin (set to low)
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 */
void gpio_clear(GPIO_TypeDef *port, gpio_t pin);

/**
 * @brief toggles a pin (HIGH to LOW, LOW to HIGH)
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 */
void gpio_toggle(GPIO_TypeDef *port, gpio_t pin);

/**
 * @brief write the given value to the pin
 * @param[in] *port			port of the given pin
 * @param[in] pin				pin number
 */
void gpio_write(GPIO_TypeDef *port, gpio_t pin, int value);

#endif /* GPIO_H */
