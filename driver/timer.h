/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for TIM.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef TIMER_H
#define TIMER_H

/**
 * @brief Define the default TIM type identifier
 */
typedef int tim_t;

/**
 * @brief Initialize all needed timers
 *
 * @param[in] dev		timer device descriptor
 * @param[in] *tft	pointer to callback function
 */
void timer_init(tim_t dev, void (*tft)(void));

/**
 * @brief Disables the clock of a timer device
 *
 * @param[in] dev		timer device descriptor
 */
void timer_stop(tim_t dev);

/**
 * @brief Enables the clock of a timer device
 *
 * @param[in] dev		timer device descriptor
 */
void timer_start(tim_t dev);

/**
 * @brief Enables irq of given timer
 *
 * @param[in] dev		timer device descriptor
 */
void timer_irq_enable(tim_t dev);

/**
 * @brief	Disables irq of given timer
 *
 * @param[in] dev		timer device descriptor
 */
void timer_irq_disable(tim_t dev);

/**
 * @brief Reads the count of a timer
 *
 * @param[in] dev    devices descriptor
 *
 * @return           -1 on error
 */
int timer_read(tim_t dev);

#endif /* TIMER_H */
