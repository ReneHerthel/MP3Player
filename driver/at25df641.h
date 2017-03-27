/**
 * @{
 *
 * @brief     Low-level peripherial device driver interface for AT25Df641.
 * @author    Copyright (C) Rene Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef AT25DF641_H
#define AT25DF641_H

#include <stm32f4xx.h>
#include "config/periph_conf.h"

/**
 * @brief Default type definiton for a AT25DF641 memory device
 */
typedef enum {
#if AT25DF641_0_EN
	  AT25DF641_0 = 0,    /**< work */
#endif
#if AT25DF641_1_EN
	  AT25DF641_1         /**< orig */
#endif
} at25df641_dev_t;

/**
 * @brief Default type defintion for the data of a AT25DF641
 */
typedef struct {
    unsigned char *data;/**< data buffer to be sent or received */
    uint32_t address;   /**< serialflash internal address */
    uint16_t data_size; /**< Number of bytes to send/receive */
    uint8_t cmd;        /**< command byte opcode */
    uint8_t cmd_size;   /**< number of cmd + addr bytes */
    GPIO_TypeDef *port; /**< port of the current CS pin */
    uint8_t pin;        /**< pin number of the current CS pin */
} at25df641_t;

/**
 * @brief Erase Opcodes default type defintion
 */
typedef enum {
    BLOCK_ERASE_4KB  = (0x20),  /**< OPcode for erasing  4 KiloByte */
    BLOCK_ERASE_32KB = (0x52),  /**< OPcode for erasing 32 KiloByte */
    BLOCK_ERASE_64KB = (0xD8)   /**< OPcode for erasing 64 KiloByte */
} at25df641_erase_t;

/**
 * @brief Chip protect default type defintion
 */
typedef enum {
    UNPROTECT = 0,
    PROTECT   = 1,
} at25df641_prot_t;

/**
 * @brief Initialize an AT25DF641 module
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_init(at25df641_dev_t dev);

/**
 * @brief Sends a command via SPI
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_command_handler(at25df641_dev_t dev);

/**
 * @brief Write data with given length to the spi bus
 */
void write_spi(int length, unsigned char *data);

/**
 * @brief Read data with given length from the spi bus
 */
void read_spi(int length, unsigned char *data);

/**
 * @brief Write one block with 256 byte into the flash
 *
 * @param[in] dev       device descriptor
 * @param[in] *data     data buffer
 * @param[in] size      number of bytes in buffer
 * @param[in] addr      write address
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_write(at25df641_dev_t dev, unsigned char *data, uint16_t size,
	                  uint32_t addr);

/**
 * @brief Gives write permissions
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_enable_write(at25df641_dev_t dev);

/**
 * @brief Read one block with multiple Bytes from flash
 *
 * @param[in] dev       device descriptor
 * @param[out] *data    readed data from memory
 * @param[in] size      number of bytes needed to read from memory
 * @param[in] address   address from where to read
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_read(at25df641_dev_t dev, unsigned char *data, uint16_t size,
	                 uint32_t addr);

/**
 * @brief Reads the status register from flash and returns it
 *
 * @param[in] dev       device descriptor
 * @param[out] *status  returns one byte from status register
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_read_status(at25df641_dev_t dev, unsigned char *status);

/**
 * @brief Checks the ID of the chip device
 *
 * @param[in] dev       device descriptor
 * @param[out] *dev_id  returns the device ID
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_read_id(at25df641_dev_t dev_id, uint32_t *id_val);

/**
 * @brief Checks the ID of the chip device
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_check_id(at25df641_dev_t dev);

/**
 * @brief Reads the status register til the RDY/BSY-Bit is cleared
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_wait_rdy(at25df641_dev_t dev);

/**
 * @brief Clear memory protection flag
 *
 * @param[in] dev       device descriptor
 * @param[in] prot_type en-,disables chip-protection
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_chip_protect(at25df641_dev_t dev,  at25df641_prot_t prot_type);

/**
 * @brief Writes one byte into the status register
 *
 * @param[in] dev       device descriptor
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_write_status(at25df641_dev_t dev, unsigned char data);

/**
 * @brief Erase one block from flash.
 *
 * @param[in] dev       device descriptor
 * @param[in] addr      address of the block which should be erased
 * @param[in] erase_cmd erase command with different size
 *
 * @return               0 on success
 * @return              ERROR_x on error (see defines)
 */
int at25df641_erase_block(at25df641_dev_t dev, uint32_t addr,
	                        at25df641_erase_t erase_cmd);

/**
 * @brief Compares a specific address length
 */
int at25df641_compare(at25df641_dev_t fir, at25df641_dev_t sec,
	                    uint16_t size, uint32_t address);

/**
 * @brief Erase the whole chip
 */
int at25df641_chip_erase(at25df641_dev_t dev);

/**
 * @brief Compares both memory devices
 */
int at25df641_compare_all(void);

/**
 * @brief Copies the whole content from one to another memory device
 */
int at25df641_copy_all(void);

#endif /* AT25DF641_H */
