/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for SPI.
 * @author    Copyright (C) Rene Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef SPI_H
#define SPI_H

#include "config/periph_conf.h"

/**
 * @brief Definition available SPI devices
 */
typedef enum {
#if SPI_0_EN
    SPI_0 = 0     /**< SPI device 0 */
#endif
} spi_t;

/**
* @brief SPI Baudrate definitions for CR[5:3] register
 */
typedef enum {
    /* SPI1, Fpclk = 84MHz */
    SPI_BAUD_84MHZ_DIV_2   = 0x00, /**< 84MHz / 2   = 42Mhz */
    SPI_BAUD_84MHZ_DIV_4   = 0x01, /**< 84MHz / 4   = 21Mhz */
    SPI_BAUD_84MHZ_DIV_8   = 0x02, /**< 84MHz / 8   = 10.5Mhz */
    SPI_BAUD_84MHZ_DIV_16  = 0x03, /**< 84MHz / 16  =  5.25Mhz */
    SPI_BAUD_84MHZ_DIV_32  = 0x04, /**< 84MHz / 32  =  2.625Mhz */
    SPI_BAUD_84MHZ_DIV_64  = 0x05, /**< 84MHz / 64  =  1.3125Mhz */
    SPI_BAUD_84MHZ_DIV_128 = 0x06, /**< 84MHz / 128 =  0.65625Mhz */
    SPI_BAUD_84MHZ_DIV_256 = 0x07, /**< 84MHz / 256 =  0.328125Mhz */
    /* SPI2-3, Fpclk = 42Mhz */
    SPI_BAUD_42MHZ_DIV_2   = 0x00, /**< 42MHz / 2   = 21Mhz */
    SPI_BAUD_42MHZ_DIV_4   = 0x01, /**< 42MHz / 4   = 10.5Mhz */
    SPI_BAUD_42MHZ_DIV_8   = 0x02, /**< 42MHz / 8   =  5.25Mhz */
    SPI_BAUD_42MHZ_DIV_16  = 0x03, /**< 42MHz / 16  =  2.625Mhz */
    SPI_BAUD_42MHZ_DIV_32  = 0x04, /**< 42MHz / 32  =  1.3125Mhz */
    SPI_BAUD_42MHZ_DIV_64  = 0x05, /**< 42MHz / 64  =  0.65625Mhz */
    SPI_BAUD_42MHZ_DIV_128 = 0x06, /**< 42MHz / 128 =  0.328125Mhz */
    SPI_BAUD_42MHZ_DIV_256 = 0x07  /**< 42MHz / 256 =  0.1640625Mhz */
} spi_baud_t;

/**
 * @brief Initialize the SPI device as master
 *
 * @param[in] dev    SPI device descriptor
 * @param[in] baud   baudrate for given device
 *
 * @return            0 on succes
 * @return           -1 on error
 */
int spi_init_master(spi_t dev, spi_baud_t baud);

/**
 * @brief Configure SCK, MISO and MOSI pin
 *
 * @param[in] dev    SPI device descriptor
 *
 * @return            0 on succes
 * @return           -1 on error
 */
int spi_conf_pins(spi_t dev);

/**
 * @brief Transfers one Byte
 *
 * @param[in] dev    SPI device descriptor
 * @param[in] mosi   master output to slaves input
 * @param[out] miso  master input from slaves output
 */
void spi_transfer_byte(spi_t dev, unsigned char out, unsigned char *in);

/**
 * @brief Enables a spi device
 *
 * @param[in] dev    SPI device descriptor
 */
void spi_poweron(spi_t dev);

/**
 * @brief Disables a spi device
 *
 * @param[in] dev    SPI device descriptor
 */
void spi_poweroff(spi_t dev);

#endif /* SPI_H */
