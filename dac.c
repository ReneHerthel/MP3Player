/**
 * @{
 *
 * @brief     Low-level peripheral driver for DAC.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */


#include <stm32f4xx.h>

#include "driver/dac.h"
#include "driver/gpio.h"

void dac_init(dac_t dev)
{
    DAC_TypeDef *dac = 0;

    switch (dev)
    {
#if DAC_0_EN
        case DAC_0:
            DAC_0_CLKEN();
            dac = DAC_0_DEV;
            DAC_0_PORT->MODER |= GPIO_MODE_ANALOG << (2 * DAC1);
            break;
#endif
  }

    dac->CR = 0;
    dac->CR |= (DAC_CR_EN1 | DAC_CR_EN2);
    dac->DHR12R1 = 0;
    dac->DHR12R2 = 0;
}

void dac_write(dac_t dev, uint8_t channel, uint16_t data)
{
  DAC_TypeDef *dac = 0;

    switch (dev)
    {
#if DAC_0_EN
        case DAC_0:
            dac = DAC_0_DEV;

            switch (channel)
            {
                case 0:
                    dac->DHR12R1 = data;
                    break;

                case 1:
                    dac->DHR12R2 = data;
                    break;
            }
#endif
    }
}

int dac_read(dac_t dev)
{
    DAC_TypeDef *dac = 0;
    int data = 0;

    switch (dev)
    {
#if DAC_0_EN
        case DAC_0:
             dac = DAC_0_DEV;
             data = dac->DOR1;
#endif
    }

    return data;
}

void dac_poweron(dac_t dev)
{
    switch (dev)
    {
#ifdef DAC_0_EN
        case DAC_0:
            DAC_0_CLKEN();
            break;
#endif
    }
}

void dac_poweroff(dac_t dev)
{
    switch (dev)
    {
#ifdef DAC_0_EN
        case DAC_0:
             DAC_0_CLKDIS();
             break;
#endif
    }
}
