/**
 * @{
 *
 * @brief     Low-Level GPIO peripherial driver
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include "driver/gpio.h"

int gpio_init(gpio_dir_t dir, GPIO_TypeDef *port, gpio_t pin)
{
    port->PUPDR &= ~(3 << (2 * pin));

    if (dir == GPIO_DIR_OUT) {
        port->MODER &= ~(3 << (2 * pin));
        port->MODER |= (GPIO_MODE_OUT << (2 * pin));
        port->OTYPER &= ~(1 << pin);
        port->OSPEEDR |= (GPIO_SPEED_HIGH << (2 * pin));
        port->ODR &= ~(1 << pin);
    }
    else if(dir == GPIO_DIR_IN) {
        port->MODER |= (3 << (2 * pin));
    }
		else {
			  return -1;
		}

		return 0;
}

int gpio_read(GPIO_TypeDef *port, gpio_t pin)
{
    if (port->MODER & (3 << (pin * 2))) {   /* if configured as output 	*/
        return port->ODR & (1 << pin);      /* read output data reg 		*/
    }
    else {
        return port->IDR & (1 << pin);      /* else read input data reg */
    }
}

void gpio_set(GPIO_TypeDef *port, gpio_t pin)
{
    port->BSRRL = (1 << pin);
}

void gpio_clear(GPIO_TypeDef *port, gpio_t pin)
{
    port->BSRRH = (1 << pin);
}

void gpio_toggle(GPIO_TypeDef *port, gpio_t pin)
{
    if (gpio_read(port, pin)) {
        gpio_clear(port, pin);
    }
    else {
        gpio_set(port, pin);
    }
}

void gpio_write(GPIO_TypeDef *port, gpio_t pin, int value)
{
    if (value) {
        gpio_set(port, pin);
    }
    else {
        gpio_clear(port, pin);
    }
}
