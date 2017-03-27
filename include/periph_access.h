/**
 * @{
 *
 * @brief     Defines access macros for gpio pins
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */
 
 #ifndef PERIPH_ACCESS_H
 #define PERIPH_ACCESS_H
 
/** Direct access to switches S0 - S7 of the TI-Lab board */
#define S8              (!(GPIOH->IDR & (1 << 15)))
#define S7              (!(GPIOH->IDR & (1 << 12)))
#define S6              (!(GPIOH->IDR & (1 << 10)))
#define S5              (!(GPIOF->IDR & (1 << 8 )))
#define S4              (!(GPIOF->IDR & (1 << 7 )))
#define S3              (!(GPIOF->IDR & (1 << 6 )))
#define S2              (!(GPIOC->IDR & (1 << 2 )))
#define S1              (!(GPIOI->IDR & (1 << 9 )))
 
/** Access to wait and underflow LED */
#define SET_PI7()       (gpio_set(GPIOI, PI7))     /**< D23 */
#define CLR_PI7()       (gpio_clear(GPIOI, PI7))   /**< D23 */
#define SET_PI6()       (gpio_set(GPIOI, PI6))     /**< D22 */
#define CLR_PI6()       (gpio_clear(GPIOI, PI6))   /**< D22 */
#define SET_PH11()      (gpio_set(GPIOH, PH11))    /**< D17 */
#define CLR_PH11()      (gpio_clear(GPIOH, PH11))  /**< D17 */
#define SET_PH13()      (gpio_set(GPIOH, PH13))    /**< D16 */
#define CLR_PH13()      (gpio_clear(GPIOH, PH13))  /**< D16 */

/** Direct access to FSMC pins */
#define NE1             (!(GPIOD->IDR & (1 << 7)))
#define NOE             (!(GPIOD->IDR & (1 << 4)))
#define NWE             (!(GPIOD->IDR & (1 << 5)))
#define NWAIT           (!(GPIOD->IDR & (1 << 6)))
 
 #endif /* PERIPH_ACCESS_H */
