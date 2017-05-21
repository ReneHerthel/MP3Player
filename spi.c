/**
 * @{
 *
 * @brief     Low-level peripheral driver for DAC.
 * @author    Copyright (C) Rene Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "driver/spi.h"
#include "driver/gpio.h"

#include "driver/debug.h"

/** Shifts to bit 3 baud bit in CR1 register */
#define SPI_CR1_BR_SHIFT     (3)

int spi_init_master(spi_t dev, spi_baud_t baud)
{
    SPI_TypeDef *spi;

    switch (dev)
    {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            SPI_0_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_MISO_PORT_CLKEN();
            SPI_0_MOSI_PORT_CLKEN();
            break;
#endif /* SPI_0_EN */
        default:
            return -1;
    }

    if (spi_conf_pins(dev))
    {
        return -1;
    }

    /* activate SPI mode */
    spi->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);
    spi->CR1 = 0;
    spi->CR2 = 0;
    spi->CR1 |= (baud << SPI_CR1_BR_SHIFT);
    spi->CR1 |= SPI_CR1_MSTR;
    /* NSS (chip select) managed by software and NSS is low (slave enabled)*/
    spi->CR1 |= SPI_CR1_SSM;
    spi->CR1 |= SPI_CR1_SSI;
    spi->CR1 |= SPI_CR1_CPOL;
    spi->CR1 |= SPI_CR1_CPHA;
    spi->CR1 |= SPI_CR1_SPE;

    return 0;
}

int spi_conf_pins(spi_t dev)
{
    GPIO_TypeDef *port[3];
    int hl, i, pin[3], af[3];

    switch (dev)
    {
#if SPI_0_EN
        case SPI_0:
            port[0] = SPI_0_SCK_PORT;
            pin[0] = SPI_0_SCK_PIN;
            af[0] = SPI_0_SCK_AF;
            port[1] = SPI_0_MOSI_PORT;
            pin[1] = SPI_0_MOSI_PIN;
            af[1] = SPI_0_MOSI_AF;
            port[2] = SPI_0_MISO_PORT;
            pin[2] = SPI_0_MISO_PIN;
            af[2] = SPI_0_MISO_AF;
            break;
#endif /* SPI_0_EN */
        default:
            return 1;
    }

    for (i = 0; i < 3; i++)
    {
        /* Set GPIOs to AF mode */
        port[i]->MODER &= ~(3 << (2 * pin[i]));
        port[i]->MODER |= (GPIO_MODE_AF << (2 * pin[i]));
        /* Set speed */
        port[i]->OSPEEDR &= ~(3 << (2 * pin[i]));
        port[i]->OSPEEDR |= (GPIO_SPEED_HIGH << (2 * pin[i]));
        /* Set to push-pull configuration */
        port[i]->OTYPER &= ~(GPIO_OT_OPENDRAIN << pin[i]);
        /* Configure push-pull resistors */
        port[i]->PUPDR &= ~(3 << (2 * pin[i]));
        port[i]->PUPDR |= (GPIO_PULLDOWN << (2 * pin[i]));
        /* Configure GPIOs for the SPI alternate function */
        hl = (pin[i] < 8) ? 0 : 1;
        port[i]->AFR[hl] &= ~(0xf << ((pin[i] - (hl * 8)) * 4));
        port[i]->AFR[hl] |= (af[i] << ((pin[i] - (hl * 8)) * 4));
    }

    return 0;
}

int spi_acquire(spi_t dev)
{
    return -1;
}

int spi_release(spi_t dev)
{
    return -1;
}

void spi_transfer_byte(spi_t dev, unsigned char out, unsigned char *in)
{
    char val;
    SPI_TypeDef *spi;

    switch (dev)
    {
#if SPI_0_EN
        case SPI_0:
        spi = SPI_0_DEV;
#endif
    }

    /* wait for an eventually previous byte to be finished transferred */
    while(!(spi->SR & SPI_SR_TXE))
    {
        /* Do nothing here. */
    };

    /* put next byte into the output register */
    spi->DR = (char) out;

    /* wait until the current byte was successfully transferred */
    while(!(spi->SR & SPI_SR_RXNE))
    {
        /* Do nothing here. */
    };

    /* read response byte to reset flags */
    val = spi->DR;

    /* 'return' response byte if wished for */
    if (in != NULL)
    {
        *in = (unsigned char) val;
    }
}

void spi_poweron(spi_t dev)
{
    switch (dev)
    {
#if SPI_0_EN
        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_BSY);
            SPI_0_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev)
    {
#if SPI_0_EN
        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_BSY);
            SPI_0_CLKDIS();
            break;
#endif
    }
}
