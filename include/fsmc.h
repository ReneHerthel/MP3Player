/**
 * @{
 *
 * @brief     Interface of the fsmc module.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef FSMC_H
#define FSMC_H

/**
 * @brief Initialize the fsmc pin interface
 */
int fsmc_init(void);

/**
 * @brief Transfers data through the fsmc
 */
void fsmc_transfer(int16_t wr_val, int16_t *rd_val);

#endif /* FMSC_H */
