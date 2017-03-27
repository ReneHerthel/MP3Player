/**
 * @{
 *
 * @brief     FSMC implementation for the µC
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <stm32f4xx.h>

#include "driver/gpio.h"
#include "driver/debug.h"
#include "driver/config/periph_conf.h"

/* ********** GPIO_D ********** */
#define PD0  (0)  /**< FSMC_D2  */
#define PD1  (1)  /**< FSMC_D3  */
/* PD2 is unused                */
/* PD3 is unused                */
#define PD4  (4)  /**< NOE      */
#define PD5  (5)  /**< NWE      */
#define PD6  (6)  /**< NWAIT    */
#define PD7  (7)  /**< NE1      */
#define PD8  (8)  /**< FSMC_D13 */
#define PD9  (9)  /**< FSMC_D14 */
#define PD10 (10) /**< FSMC_D15 */
/* PD11 is unused               */
/* PD12 is unused               */
/* PD13 is unused               */
#define PD14 (14) /**< FSMC_D0  */
#define PD15 (15) /**< FSMC_D1  */
/* ********** **11** ********** */

/* ********** GPIO_E ********** */
/* PE0 is unused                */
/* PE1 is unused                */
/* PE2 is unused                */
/* PE3 is unused                */
/* PE4 is unused                */
/* PE5 is unused                */
/* PE6 is unused                */
#define PE7  (7)  /**< FSMC_D4  */
#define PE8  (8)  /**< FSMC_D5  */
#define PE9  (9)  /**< FSMC_D6  */
#define PE10 (10) /**< FSMC_D7  */
#define PE11 (11) /**< FSMC_D8  */
#define PE12 (12) /**< FSMC_D9  */
#define PE13 (13) /**< FSMC_D10 */
#define PE14 (14) /**< FSMC_D11 */
#define PE15 (15) /**< FSMC_D12 */
/* ********** **09** ********** */

#define NUM_OF_D_PINS   (11)
#define NUM_OF_E_PINS   (9)
#define NUM_OF_PINS     (NUM_OF_D_PINS + NUM_OF_E_PINS)

#define AF_FSMC         ((uint8_t)0xC)

#define BANK1_ADDR      (0x60000000)

#define DATAST_R				(4) // max: 255		min: 4
#define ADDSET_R				(1)	// max: 15		min: 0

#define DATAST_W				(4) // max: 255		min: 4
#define ADDSET_W				(1)	// max: 15		min: 0

#define BUSTURN					(0)	// max: 15		min: 0

#define RESERVED_7			((uint32_t)0x00000080)

/**
 * @brief TODO
 */
static inline void _config_pins(void)
{
		GPIO_TypeDef *port[NUM_OF_PINS];
	  int hl, i, pin[NUM_OF_PINS];
	
	  port[0]  = GPIOD; /**< PD7  / NE1     */
	  pin[0]   = PD7;
	  port[1]  = GPIOD; /**< PD4  / NOE     */
	  pin[1]   = PD4; 
	  port[2]  = GPIOD; /**< PD5  / NWE     */
	  pin[2]   = PD5;
	  port[3]  = GPIOD; /**< PD6  / NWAIT   */
	  pin[3]   = PD6;
	  port[4]  = GPIOD; /**< PD14 / FSMC_D0 */
	  pin[4]   = PD14;
	  port[5]  = GPIOD; /**< PD15 / FSMC_D1 */
		pin[5]   = PD15;
		port[6]  = GPIOD; /**< PD0  / FSMC_D2 */
		pin[6]   = PD0;
		port[7]  = GPIOD; /**< PD1  / FSMC_D3 */
		pin[7]   = PD1;
		port[8]  = GPIOE; /**< PE7  / FSMC_D4 */
		pin[8]   = PE7;
		port[9]  = GPIOE; /**< PE8  / FSMC_D5 */
		pin[9]   = PE8;	
		port[10] = GPIOE; /**< PE9  / FSMC_D6 */
		pin[10]  = PE9;	
		port[11] = GPIOE; /**< PE10 / FSMC_D7 */
		pin[11]  = PE10;	
		port[12] = GPIOE; /**< PE11 / FSMC_D8 */
		pin[12]  = PE11;	
		port[13] = GPIOE; /**< PE12 / FSMC_D9 */
		pin[13]  = PE12;	
		port[14] = GPIOE; /**< PE13 / FSMC_D10 */
		pin[14]  = PE13;	
		port[15] = GPIOE; /**< PE14 / FSMC_D11 */
		pin[15]  = PE14;	
		port[16] = GPIOE; /**< PE15 / FSMC_D12 */
		pin[16]  = PE15;	
		port[17] = GPIOD; /**< PD8  / FSMC_D13 */
		pin[17]  = PD8;	
		port[18] = GPIOD; /**< PD9  / FSMC_D14 */
		pin[18]  = PD9;	
		port[19] = GPIOD; /**< PD10 / FSMC_D15 */
		pin[19]  = PD10;
		DMSG("FSMC: _config_pins():\n");
	  /* Initialize GPIO */
	  for (i = 0; i < NUM_OF_PINS; i++) {
        /* Set GPIOs to AF mode */
        port[i]->MODER &= ~(3 << (2 * pin[i]));
        port[i]->MODER |= (GPIO_MODE_AF << (2 * pin[i]));
			  /* Clear push-pull configuration */
        port[i]->OTYPER &= ~(GPIO_OT_OPENDRAIN << pin[i]);
        /* Set speed */
        port[i]->OSPEEDR &= ~(3 << (2 * pin[i]));
        port[i]->OSPEEDR |= (GPIO_SPEED_HIGH << (2 * pin[i])); //<<< SIND DAS WIRKLICH 100MHZ ?!?
        /* Clear push-pull configuration */
        port[i]->PUPDR &= ~(3 << (2 * pin[i]));
        /* Configure GPIOs for the FMSC alternate function */
        hl = (pin[i] < 8) ? 0 : 1;
        port[i]->AFR[hl] &= ~(0xf << ((pin[i] - (hl * 8)) * 4));
        port[i]->AFR[hl] |= (AF_FSMC << ((pin[i] - (hl * 8)) * 4));
			  DMSG("pin: %d, hl: %d\n", pin[i], hl);
    }
		DMSG("FSMC: _config_pins() - done!\n");
}

/**
 * @brief TODO
 */
static inline void _write(int16_t data)
{
    (*(int16_t*)BANK1_ADDR) = data;
}

/**
 * @brief TODO
 */
static inline void _read(int16_t *data)
{
	  *data = (*(int16_t*)BANK1_ADDR);
}

int fsmc_init(void)
{
	  /* Enable all needed clocks */
	  GPIOD_CLKEN();
	  GPIOE_CLKEN();
		GPIOF_CLKEN();
	  RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN; // FSMC Enable
	  /* Configure GPIO pins */
	  _config_pins();
		/* Configure banks */
	
		FSMC_Bank1->BTCR[0] = 0;
		FSMC_Bank1->BTCR[1] = 0;
		FSMC_Bank1E->BWTR[0] = 0;

		FSMC_Bank1->BTCR[0] = FSMC_BCR1_ASYNCWAIT | FSMC_BCR1_EXTMOD | FSMC_BCR1_WREN | RESERVED_7 | FSMC_BCR1_MWID_0 | FSMC_BCR1_MBKEN;
		FSMC_Bank1->BTCR[1] = (BUSTURN << 16) /* BUSTURN */ | (DATAST_R << 8) /* DATAST (5 * HCLK) */ | (ADDSET_R << 0) /* ADDSET (1 * HCLK) */ ;
		FSMC_Bank1E->BWTR[0] = (BUSTURN << 16) /* BUSTURN */ | (DATAST_W << 8) /* DATAST (5 * HCLK) */ | (ADDSET_W << 0) /* ADDSET (1 * HCLK) */ ;
	
	  return 0;
}

void fsmc_transfer(int16_t wr_val, int16_t *rd_val)
{
	  if (wr_val != NULL) {
			  //DMSG("FSMC: fsmc_transfer() wr_val != NULL\n");
			  _write(wr_val);
		}
		if (rd_val != NULL) {
			  //DMSG("FSMC: fsmc_transfer() rd_val != NULL\n");
			  _read(rd_val);
		}
}
