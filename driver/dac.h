/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for DAC.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef DAC_H
#define DAC_H

#include "config/periph_conf.h"

/**
 * @brief   Define the default DAC type identifier
 */
typedef enum {
#if DAC_0_EN
	  DAC_0 = 0,  /**< DAC device 0 */
#endif
} dac_t;

/**
 * @brief   Initialize a pin as DAC with analog output
 *
 * @param[in] dev	dac device descriptor
 */
void dac_init(dac_t dev);

/**
 * @brief	Writes to a DAC channelx register
 *
 * @param[in] dev	dac device descriptor
 * @param[in] data	data to write into the DAC channel
 */
void dac_write(dac_t dac, uint8_t channel, uint16_t data);

/**
 * @brief	Reads from an DAC data output register
 *
 * @param[in] dev	dac device descriptor
 *
 * @return	  data	data recieved from output register
 */
int dac_read(dac_t dac);

/**
 * @brief 	Enables the dac clock
 *
 * @param[in] dev	dac device descriptor
 */
void dac_poweron(dac_t dev);

/**
 * @brief	Disables the dac clock
 *
 * @param[in] dev	dac device descriptor
 */
void dac_poweroff(dac_t dev);

#endif /* DAC_H */
