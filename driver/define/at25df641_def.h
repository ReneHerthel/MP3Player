/**
 * @{
 *
 * @brief     AT25DF641 configurations
 * @author    Copyright (C) Rene Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#ifndef AT25DF641_DEF_H
#define AT25DF641_DEF_H

/******************************************************************************
 * @brief General defines
 *****************************************************************************/
/** Total memory size of the chip (8MB) */
#define AT25DF641_MEM_SIZE                        ((int)8 * 1024 * 1024)
/** Size of one block (64KB) */
#define AT25DF641_BLOCK_SIZE                      (64 * 1024)
/** Size of one page of the memory (256 Bytes)*/
#define AT25DF641_PAGE_SIZE                       ((int)256)
/** Chip identification value of the device */
#define AT25DF641_DEV_ID                          (0x481F)
/** Value which is needed for global protect*/
#define AT25DF641_GLOBAL_PROTECT_VALUE            (0x3C)
/** Size of a command package */
#define PACK_SIZ                                  (4)
/** Dummy byte */
#define DUMMY                                     (0xFF)
/* */
#define MEM_DIV_PAGE_SIZE                         (AT25DF641_MEM_SIZE / AT25DF641_PAGE_SIZE) // 32768

/******************************************************************************
 * @brief Mathematical defintions
 *****************************************************************************/
/** Returns the minimum of two values */
#define MINIMUM(a, b)  (((a) < (b))?(a):(b))

/******************************************************************************
 * @brief Register masks and checking-defines for if-cases
 *****************************************************************************/
/** Mask the status RDY/BSY bit out from the SR reg */
#define AT25DF641_MASK_SR_RDYBSY                  (1 << 0)
/** Check if the device is rdy */
#define AT25DF641_SR_RDYBSY_READY                 (0 << 0)
/** Mask the SWP bit out from the SR reg */
#define AT25DF641_MASK_SR_SWP                     (2 << 2)
/** Check if all sectors are unprotected */
#define AT25DF641_SR_SWP_NONE                     (0 << 2)
/** Mask the EPE bit out from the SR reg */
#define AT25DF641_MASK_SR_EPE                     (1 << 4)
/** Check if the Erase or program operation was successfull */
#define AT25DF641_SR_EPE_SUCCESS                  (0 << 4)
/** Mask the SPRL bit out from the SR reg */
#define AT25DF641_MASK_SR_SPRL                    (1 << 7)
/** Check if the SPRL bit is not set */
#define AT25DF641_SR_SPRL_NONE                    (0 << 7)

/******************************************************************************
 * @brief Error definitions of the code
 *****************************************************************************/
/** Success default return code */
#define OK                                        (0)
/** Error default return code */
#define ERROR_DEFAULT                             (-1)
/** Error return code for address out of bound */
#define ERROR_OUT_OF_BOUND                        (-2)
/** Error return code for device is busy */
#define ERROR_BUSY                                (-3)
/** Error return code for memory is protected */
#define ERROR_PROTECTED                           (-4)
/** Error return code for writing error */
#define ERROR_WRITE                               (-5)
/** Error return code for wrong device ID */
#define ERROR_ID                                  (-6)
/** Error return code for memory is not protected */
#define ERROR_UNPROTECTED                         (-7)
/** Error return code for  gpio failuer */
#define ERROR_GPIO                                (-8)

/******************************************************************************
 * @brief Commands as Opcode in hex of the device
 *****************************************************************************/
/* read commands */
#define AT25DF641_OPCODE_RD_ARRY                  (0x0B)
#define AT25DF641_OPCODE_DUAL_OUT_RD_ARRY         (0x3B)

/* program and erase commands */
#define AT25DF641_OPCODE_BLOCK_ERASE_4KB          (0x20)
#define AT25DF641_OPCODE_BLOCK_ERASE_32KB         (0x52)
#define AT25DF641_OPCODE_BLOCK_ERASE_64KB         (0xD8)
#define AT25DF641_OPCODE_CHIP_ERASE               (0x60)
#define AT25DF641_OPCODE_BYTE_PROGR               (0x02)
#define AT25DF641_OPCODE_DUAL_IN_BYTE_PROGR       (0xA2)
#define AT25DF641_OPCODE_PROGR_SUSP               (0xB0)
#define AT25DF641_OPCODE_PROGR_RESUM              (0xD0)

/* protection commands */
#define AT25DF641_OPCODE_WR_EN                    (0x06)
#define AT25DF641_OPCODE_WR_DIS                   (0x04)
#define AT25DF641_OPCODE_PROT_SECT                (0x36)
#define AT25DF641_OPCODE_UNPROT_SECT              (0x39)
#define AT25DF641_OPCODE_RD_SECT_PROT_REG         (0x3C)

/* security commands */
// Nothing to do so far. DO NOT USE LOCK-DOWN OPCODES!

/* status register commands */
#define AT25DF641_OPCODE_RD_SR                    (0x05)
#define AT25DF641_OPCODE_WR_SR_FRST_BYTE          (0x01)
#define AT25DF641_OPCODE_WR_SR_SECD_BYTE          (0x31)

/* miscellaneous commands */
#define AT25DF641_OPCODE_RESET                    (0xF0)
#define AT25DF641_OPCODE_RD_ID                    (0x9F)
#define AT25DF641_OPCODE_DEEP_POWDWN              (0xB9)
#define AT25DF641_OPCODE_RESUM_POWDWN             (0xAB)

#endif /* AT25DF641_DEF_H */
