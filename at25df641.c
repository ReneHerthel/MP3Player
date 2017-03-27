 /**
 * @{
 *
 * @brief     Low-level driver implementation for at25df641.
 * @author    Copyright (C) Rene Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <stm32f4xx.h>
#include "driver/at25df641.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "driver/config/periph_conf.h"
#include "driver/define/at25df641_def.h"
#include "driver/debug.h"

/** initialize a port_pin for CS */
#define INIT_CS(port, pin) (gpio_init(GPIO_DIR_OUT, port, pin))
/** set a port-pin for CS */
#define SET_CS(port, pin)  (gpio_set(port, pin))
/** clear a port-pin for CS */
#define CLR_CS(port, pin)  (gpio_clear(port, pin))

/** Masks different bytes from the config array for better access */
#define CMD(x)             (at25df641_cfg[x].cmd)
#define CMD_SIZ(x)         (at25df641_cfg[x].cmd_size)
#define ADDR(x)            (at25df641_cfg[x].address)
#define DATA(x)            (at25df641_cfg[x].data)
#define SIZE(x)            (at25df641_cfg[x].data_size)
#define PORT(x)            (at25df641_cfg[x].port)
#define PIN(x)             (at25df641_cfg[x].pin)

/** Dummy byte */
#define DUMMY              (0xFF)

/**
 * @brief Array holding all needed information for all devices
 */
at25df641_t at25df641_cfg[] = {
	  /*data, address, data_size, cmd, cmd_size, port, pin*/
	  {0, 0, 0, 0x00, 0, WORK_CS_PORT, WORK_CS_PIN},
	  {0, 0, 0, 0x00, 0, ORIG_CS_PORT, ORIG_CS_PIN},
	  /** Add here more devices to use more physical components */
};

/**
 * @brief Reads bytes from the memory
 */
void read_spi(int length, unsigned char *data)
{
	  unsigned char val;
    int i = 0;
    
    while(length > 0) {
        //spi_transfer_byte(SPI_0, DUMMY, &val);
			
        /*----------- READ VIA SPI -----------*/
        while(!(SPI_0_DEV->SR & SPI_SR_TXE)){};
        SPI_0_DEV->DR = (char) DUMMY;	
        while(!(SPI_0_DEV->SR & SPI_SR_RXNE)){};
        val = SPI_0_DEV->DR;
        /*------------------------------------*/
					
        //DMSG("_read: &val = 0x%x\n", val);
        data[i] = val;
        i++;
        length--;
	  }
}

/**
 * @brief Writes bytes to the memory
 */
void write_spi(int length, unsigned char *data)
{
    unsigned char val;
    int i = 0;

    while (length > 0) {
        val = data[i];
        //spi_transfer_byte(SPI_0, val, NULL);
			
        /*----------- WRITE VIA SPI -----------*/
        while(!(SPI_0_DEV->SR & SPI_SR_TXE)){};
        SPI_0_DEV->DR = (char) val;	
        while(!(SPI_0_DEV->SR & SPI_SR_RXNE)){};
        SPI_0_DEV->DR; // reset flags
        /*-------------------------------------*/
					
        //DMSG("_write: &val%i = 0x%x\n",i, val);
        i++;
        length--;
    }
}

/**
 * @brief Builds and send a package included cmd & addr
 */
static void _package(at25df641_dev_t dev)
{
	  unsigned char buf[4];

    buf[0] = (CMD(dev) & 0x000000FF);
    buf[1] = ((ADDR(dev) & 0xFF0000) >> 16); // 0000FF
    buf[2] = ((ADDR(dev) & 0x00FF00) >> 8);  // 0000FF
    buf[3] = (ADDR(dev) & 0x0000FF);         // 0000FF
    
	  //DMSG("_package: 0x%02x%02x%02x 0x%02x\n", buf[3], buf[2], buf[1], buf[0]);
    write_spi(CMD_SIZ(dev), buf);
}

int at25df641_command_handler(at25df641_dev_t dev)
{
    CLR_CS(PORT(dev), PIN(dev)); /**< select chip */

    _package(dev);
    
    if(CMD(dev) == AT25DF641_OPCODE_RD_ARRY){
        //spi_transfer_byte(SPI_0, DUMMY, NULL);
			
        /*----------- WRITE VIA SPI -----------*/
        while(!(SPI_0_DEV->SR & SPI_SR_TXE)){};
        SPI_0_DEV->DR = (char) DUMMY;	
        while(!(SPI_0_DEV->SR & SPI_SR_RXNE)){};
        SPI_0_DEV->DR; // reset flags
        /*-------------------------------------*/
    }
    
    if ((CMD(dev) == AT25DF641_OPCODE_RD_ID)
        || (CMD(dev) == AT25DF641_OPCODE_RD_ARRY)
		    || (CMD(dev) == AT25DF641_OPCODE_RD_SR)) {
        read_spi(SIZE(dev), DATA(dev));
	  }
	  else if((CMD(dev) == AT25DF641_OPCODE_WR_SR_FRST_BYTE)
            || (CMD(dev) == AT25DF641_OPCODE_BYTE_PROGR)) {
        write_spi(SIZE(dev), DATA(dev));
	  }

	  SET_CS(PORT(dev), PIN(dev)); /**< deselect chip */

    return OK;
}

int at25df641_init(at25df641_dev_t dev)
{
	  /* intialize chip select with predefined device */
	  INIT_CS(PORT(dev), PIN(dev));

    /* deselect chip */
	  SET_CS(PORT(dev), PIN(dev));

    return at25df641_check_id(dev);
}

int at25df641_read_id(at25df641_dev_t dev, uint32_t *id_val)
{
	  int status;

	  /* initialize a new command */
	  CMD(dev) = AT25DF641_OPCODE_RD_ID;
	  CMD_SIZ(dev) = 1;
	  DATA(dev) = (unsigned char*) id_val;
	  SIZE(dev) = 3;
	  ADDR(dev) = 0;

	  status = at25df641_command_handler(dev);

	  *id_val &= 0x00FFFFFF; /**< mask the id out */
	  //DMSG("_read_id: %08x\n", *id_val);

	  return status;
}

int at25df641_check_id(at25df641_dev_t dev)
{
    uint32_t val;

    /* read memory device id */
    at25df641_read_id(dev, &val);
    //DMSG("_check_id: %08x\n", val);

    if (val == AT25DF641_DEV_ID) {
        DMSG("Id for device %i = %x\n", dev, val);
        return OK;
    }
    else {
        return ERROR_ID;
    }

}

int at25df641_read(at25df641_dev_t dev, unsigned char *data, uint16_t size, uint32_t addr)
{
    /* Check if address plus size is out of bound */
    if ((size + addr) > AT25DF641_MEM_SIZE) {
        DMSG("_read OOB - addr: 0x%x, size: %d\n", addr, size);
        return ERROR_OUT_OF_BOUND;
    }

    /* initialize a new command */
    CMD(dev) = AT25DF641_OPCODE_RD_ARRY;
    CMD_SIZ(dev) = 4;
    DATA(dev) = data;
    SIZE(dev) = size;
    ADDR(dev) = addr;

    //DMSG("ADDR(dev) = %x\n", ADDR(dev)); 

    return at25df641_command_handler(dev);
}

int at25df641_write(at25df641_dev_t dev,unsigned char *data, uint16_t size, uint32_t addr)
{
	  unsigned char status;
	  uint32_t write_size;
    
    at25df641_chip_protect(dev, UNPROTECT);

	  /* Check if address plus size is out of bound */
	  if ((size + addr) > AT25DF641_MEM_SIZE) {
			  //DMSG("_write_byte: %16x out of bounds\n", (size + addr));
			  return ERROR_OUT_OF_BOUND;
		}

		/* program one page after another */
		while (size > 0) {
			  /* compute the number of bytes to program in page */
			  write_size = MINIMUM(size, AT25DF641_PAGE_SIZE -
			                      (addr % AT25DF641_PAGE_SIZE));

			  /* enable critical write operation */
		    if (at25df641_enable_write(dev) != OK) {
			      return ERROR_DEFAULT;
		    }

				/* initialize a new command */
				CMD(dev) = AT25DF641_OPCODE_BYTE_PROGR;
				CMD_SIZ(dev) = 4;
				DATA(dev) = data;
				SIZE(dev) = write_size;
				ADDR(dev) = addr;

				/* execute command */
				if (at25df641_command_handler(dev) != OK){
					  return ERROR_DEFAULT;
				}

				/* wait til the command is executed */
			  if (at25df641_wait_rdy(dev) != OK ) {
			      return ERROR_DEFAULT;
		    }

				/* read the status register */
				if (at25df641_read_status(dev, &status) != OK) {
					  return ERROR_DEFAULT;
				}

				/* Check if the writing process had an error */
				if ((status & AT25DF641_MASK_SR_EPE) != AT25DF641_SR_EPE_SUCCESS) {
					  //return ERROR_WRITE;
				}

				data += write_size; /**< increment data buffer */
				size -= write_size; /**< decrement number of bytes to write */
				addr += write_size; /**< increment address */
		}
        
        at25df641_chip_protect(dev, PROTECT);

		return OK;
}

int at25df641_enable_write(at25df641_dev_t dev)
{
    int status;
	  /* initialize a new command */
    CMD(dev) = AT25DF641_OPCODE_WR_EN;
    CMD_SIZ(dev) = 1;
	  DATA(dev) = NULL;
	  SIZE(dev) = 0;
	  ADDR(dev) = 0;

	  status = at25df641_command_handler(dev);
    
    /* wait til command is executed */
	  if (at25df641_wait_rdy(dev) != OK ) {
	      return ERROR_DEFAULT;
	  }
    
    return status;
}

int at25df641_read_status(at25df641_dev_t dev, unsigned char *status)
{
	  /* initialize a new command */
	  CMD(dev) = AT25DF641_OPCODE_RD_SR;
	  CMD_SIZ(dev) = 1;
	  DATA(dev) = status;
	  SIZE(dev) = 1;
	  ADDR(dev) = 0;

    return at25df641_command_handler(dev);
}

int at25df641_chip_protect(at25df641_dev_t dev, at25df641_prot_t prot_type)
{
	  unsigned char tmp;

		/* enable critical write operation */
		if (at25df641_enable_write(dev) != OK) {
			  return ERROR_DEFAULT;
		}
        
    /* wait til command is executed */
	  if (at25df641_wait_rdy(dev) != OK ) {
	      return ERROR_DEFAULT;
	  }

		/* write one byte in the status register */
		if (prot_type == PROTECT) {
			  if ((at25df641_read_status(dev, &tmp)) != OK) {
            return ERROR_DEFAULT;
				}
        /* write the old status with new status in status register */
				if ((at25df641_write_status(dev, tmp | AT25DF641_GLOBAL_PROTECT_VALUE)) != OK) {
					  return ERROR_DEFAULT;
				}
		}
		else {
			  if ((at25df641_write_status(dev, 0x00)) != OK) {
					  return ERROR_DEFAULT;
				}
		}

		/* read the new status register */
		if ((at25df641_read_status(dev, &tmp)) != OK) {
        return ERROR_DEFAULT;
		}

		/* check if the corresponding flag is set or not */
    if (prot_type == PROTECT) {
        if ((tmp & AT25DF641_MASK_SR_SWP) == AT25DF641_SR_SWP_NONE) {
            //DMSG("_chip_prot: UNprotected\n");
            return ERROR_UNPROTECTED;
        }
    }
		else {
			  if ((tmp & (AT25DF641_MASK_SR_SWP | AT25DF641_MASK_SR_SPRL))
   					!= 0x00) {
			      //DMSG("_chip_prot: protected\n");
            return ERROR_PROTECTED;
        }
		}

		return OK;
}

int at25df641_write_status(at25df641_dev_t dev, unsigned char data)
{
	  /* initialize a new command */
	  CMD(dev) = AT25DF641_OPCODE_WR_SR_FRST_BYTE;
		CMD_SIZ(dev) = 1;
	  DATA(dev) = (unsigned char *) &data; /** byte to write */
	  SIZE(dev) = 1;              /**< Write one Byte */
	  ADDR(dev) = 0;              /**< No address needed */

	  return at25df641_command_handler(dev);
}

int at25df641_erase_block(at25df641_dev_t dev, uint32_t addr, at25df641_erase_t erase_cmd)
{
	 unsigned char status;
    
    at25df641_chip_protect(dev, UNPROTECT);

	  /* check if address is out of bounds */
	  if (addr > AT25DF641_MEM_SIZE) {
			  //DMSG("_erase_blck: %16x out of bounds\n", addr);
			  return ERROR_OUT_OF_BOUND;
		}

		/* read the status register */
    if (at25df641_read_status(dev, &status) != OK) {
			  return ERROR_DEFAULT;
	  }

		/* check if device is busy */
		if ((status & AT25DF641_MASK_SR_RDYBSY) != AT25DF641_SR_RDYBSY_READY) {
			  return ERROR_BUSY;
		}

		/* check if some sectors are protected */
	  if ((status & AT25DF641_MASK_SR_SWP) != AT25DF641_SR_SWP_NONE) {
			  return ERROR_PROTECTED;
		}

		/* enable critical write operation */
		if (at25df641_enable_write(dev) != OK) {
			  return ERROR_DEFAULT;
		}

	  /* initialize a new command */
	  CMD(dev) = erase_cmd; /**< OPcode */
		CMD_SIZ(dev) = 4;
	  DATA(dev) = NULL;     /**< No data to transmitt */
	  SIZE(dev) = 1;        /**< command size is 1 Byte */
	  ADDR(dev) = addr;     /**< which address should be erased */

		/* execute command */
    if (at25df641_command_handler(dev) != OK) {
			  return ERROR_DEFAULT;
		}

		/* wait til command is executed */
	  if (at25df641_wait_rdy(dev) != OK ) {
			  return ERROR_DEFAULT;
		}
      
        at25df641_chip_protect(dev, PROTECT);

		return OK;
}

int at25df641_wait_rdy(at25df641_dev_t dev)
{
	  unsigned char status;
	  unsigned char ready = 0;

	  /* polling the status register to check busy bit */
	  while (!ready) {
			  if (at25df641_read_status(dev, &status) != OK) {
					  return ERROR_DEFAULT;
				}

				if ((status & AT25DF641_MASK_SR_RDYBSY) == AT25DF641_SR_RDYBSY_READY) {
					  ready = 1;
				}
		}

		return OK;
}

int at25df641_compare(at25df641_dev_t fir, at25df641_dev_t sec,
	                     uint16_t size, uint32_t address)
{
	int i, counter = 0;
	unsigned char work_buf[AT25DF641_PAGE_SIZE];
	unsigned char orig_buf[AT25DF641_PAGE_SIZE];

	at25df641_read(fir, work_buf, size, address);
    at25df641_read(sec, orig_buf, size, address);
// memcompare
	  //DMSG("comparing work \\ orig .. \n");
		for (i = 0; i < size; i++) {
				if (work_buf[i] != orig_buf[i]) {
					  //DMSG("0x%02x\t0x%02x\n", work_buf[i], orig_buf[i]);
                    counter++;
				}
		}
    //DMSG("Done with %d non matching values\n", counter);
        return OK;
}

int at25df641_chip_erase(at25df641_dev_t dev)
{
    at25df641_chip_protect(dev, UNPROTECT);
    
	  /* enable critical write operation */
	  if (at25df641_enable_write(dev) != OK) {
		    return ERROR_DEFAULT;
	  }
    
    /* wait til command is executed */
	  if (at25df641_wait_rdy(dev) != OK ) {
	      return ERROR_DEFAULT;
	  }
    
    CMD(dev) = AT25DF641_OPCODE_CHIP_ERASE;
	  CMD_SIZ(dev) = 1;
	  DATA(dev) = NULL;    
	  SIZE(dev) = 0;     
	  ADDR(dev) = 0;
    
	  /* execute command */
    if (at25df641_command_handler(dev) != OK) {
		    return ERROR_DEFAULT;
	  }

	  /* wait til command is executed */
	  if (at25df641_wait_rdy(dev) != OK ) {
	      return ERROR_DEFAULT;
	  }
    
    at25df641_chip_protect(dev, PROTECT);
      
    return OK;
}

int at25df641_copy_all(void)
{
    int i;
	  int status;
    unsigned char buf[AT25DF641_PAGE_SIZE];

    for (i = 0; i < MEM_DIV_PAGE_SIZE; i++) {
        if ((status = at25df641_read(AT25DF641_1, buf, AT25DF641_PAGE_SIZE, AT25DF641_PAGE_SIZE * i)) != OK) {
            return status;
        }
        if ((status = at25df641_write(AT25DF641_0, buf, AT25DF641_PAGE_SIZE, AT25DF641_PAGE_SIZE * i)) != OK) {
            return status;
        }
    }

    //DMSG("Done\n");
    return OK;
}

int at25df641_compare_all(void)
{
    int j, i;
    int	counter = 0;
    unsigned char work_buf[AT25DF641_PAGE_SIZE];
	  unsigned char orig_buf[AT25DF641_PAGE_SIZE];

    for (j = 0; j < MEM_DIV_PAGE_SIZE; j++) {
        at25df641_read(AT25DF641_0, work_buf, AT25DF641_PAGE_SIZE, AT25DF641_PAGE_SIZE * j);
        at25df641_read(AT25DF641_1, orig_buf, AT25DF641_PAGE_SIZE, AT25DF641_PAGE_SIZE * j);      
        for (i = 0; i < AT25DF641_PAGE_SIZE; i++) {
            if (work_buf[i] != orig_buf[i]) {
                counter++;
            }
		    }
    }

    //DMSG("Done with %d non matching values.\n", counter);
    return OK;
}
