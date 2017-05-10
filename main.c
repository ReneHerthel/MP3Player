/**
 * @{
 *
 * @brief     Main application for the mp3 player.
 * @author    Copyright (C) René Herthel <rene-herthel@outlook.de>
 * @author    Copyright (C) Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <string.h>

/** HAW-TI-LAB libraries */
#include "CE_Lib.h"
#include "tft.h"

/** Application helper */
#define ARM_ADS
#include "include/mp3dec.h"
#include "include/audiocalc.h"
#include "include/periph_access.h"
#include "include/fsmc.h"

/** Low-level peripheral driver */
#include "driver/pwm.h"
#include "driver/dac.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "driver/at25df641.h"
#include "driver/spi.h"

/** Commando macros for memory */
#include "driver/define/at25df641_def.h"

/** Configuration of all peripherie */
#include "driver/config/periph_conf.h"

/** Application macros */
#define TFT_EN          (1)
#define MSEC_DIVIDER    (SYS_FREQ / 1000)
#define FIFO_BUFF_SIZE  (MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)
#define LEFT_CHANNEL    (0)
#define RIGHT_CHANNEL   (1)
#define OUTPUT_AMP			(181) // amplification of the signal to reach original scale, sqrt(32768) = 181

/** Fifo declarations */
typedef struct {
    int16_t data[FIFO_BUFF_SIZE]; /**< data of fifo */
    uint16_t index;               /**< Current index of the buffer */
    uint8_t full;                 /**< Flag if buffer is full */
} fifo_t;

static volatile fifo_t fifo_0;    /**< first buffer */
static volatile fifo_t fifo_1;    /**< second buffer */
static volatile fifo_t *bg_buf;   /**< pointer to bgBuffer */
static volatile fifo_t *isr_buf;  /**< pointer to isrBuffer */

/** Misc */
static volatile int tft_refresh;  /**< flag to refresh the display */
static volatile uint32_t counter; /**< counts the number of outputs */
static uint32_t address;
static int forever = 0;

/*****************************************************************************
 * @brief Resets all relevant variables used for a new program start         *
 *****************************************************************************/
static void _reset_var(void)
{
    forever = 1;
    address = 0;
    tft_refresh = 0;
    counter = 0;
}

/*****************************************************************************
 * @brief Prints runtime informations to the tft-display.                    *
 *                                                                           *
 * @detail Calculates the runtime in milliseconds by multiplying the counter *
 *         value with the value from the auto-reload-register and then       *
 *         divide it with the System frequency divided by thousand.          *
 *****************************************************************************/
static void _lcd_out()
{
    char tmp[sizeof(int) * 3 + 2];
    uint32_t msec = (counter * TIMER_0_ARR) / MSEC_DIVIDER;

    snprintf(tmp, sizeof tmp, "%d", msec);
    TFT_gotoxy(15, 4);
    TFT_puts("msec:");
    TFT_gotoxy(21, 5);
    TFT_puts(tmp);
    snprintf(tmp, sizeof tmp, "%d", counter);
    TFT_gotoxy(15, 7);
    TFT_puts("count:");
    TFT_gotoxy(21, 8);
    TFT_puts(tmp);
}

/*****************************************************************************
 * @brief Check the buttons for change the starting address                  *
 *                                                                           *
 * @detail Check via macro if button S1 or S2 is pressed.                    *
 *         When S1 is pressed, reset the data address to 0x000000 and start. *
 *         When S2 is pressed, Set the data address to 0x200000 and start.   *
 *****************************************************************************/
static void _check_buttons(unsigned char *mem_data)
{
    if (S1)
    {
        _reset_var();
        address = 0x000000;
        at25df641_read(AT25DF641_1, mem_data, MAINBUF_SIZE, address);
        address += MAINBUF_SIZE;
    }

    if (S2)
    {
        _reset_var();
        address = 0x200000;
        at25df641_read(AT25DF641_1, mem_data, MAINBUF_SIZE, address);
        address += MAINBUF_SIZE;
    }
}

/*****************************************************************************
 * @brief Moves the data in the memory for next operations                   *
 *                                                                           *
 * @detail TODO                                                              *
 *****************************************************************************/
static void _update_memory(unsigned char *data, unsigned char *data_ptr, int bytes_left)
{
    memcpy(data, data_ptr, bytes_left);
    at25df641_read(AT25DF641_1, &data[bytes_left], MAINBUF_SIZE - bytes_left, address);
    address += (MAINBUF_SIZE - bytes_left);
}

/*****************************************************************************
 * @brief Transfer data trough the FSMC.                                     *
 *                                                                           *
 * @detail Takes a single 16 bit data and send it trough the FSMC interface  *
 *         as a write operation. Then wait for the FPGA RDY signal (NWAIT)   *
 *         to read the encoded data. At last overwrite the old data with     *
 *         the new one.                                                      *
 *****************************************************************************/
static inline void _fsmc(void)
{
    int i;
    int16_t tmp;

    for (i = 0; i < FIFO_BUFF_SIZE; i++)
    {
        /* write to FPGA via FSMC */
        fsmc_transfer(bg_buf->data[i], NULL);
        /* read from FPGA via FSMC */
        fsmc_transfer(NULL, &tmp);
        bg_buf->data[i] = tmp * OUTPUT_AMP;
    }
}

/*****************************************************************************
 * @brief INTERUPT-SERVICE-ROUTINE                                           *
 *                                                                           *
 * @detail First check if the isr_buf is not empty, set the LED on PH11,     *
 *         when the buffer is full or clear it when it is empty.             *
 *                                                                           *
 *         Calculate four values, two for each peripheral output             *
 *         for each of the two channels. Each channel belongs to their       *
 *         own physically output, one for left and one for the right.        *
 *                                                                           *
 *         Writes the calculated value to the corresponding peripherial      *
 *         output.                                                           *
 *                                                                           *
 *         The index increments by two.                                      *
 *         The counter counts also four times for four calculated outputs    *
 *                                                                           *
 *         Next check if the buffer has reached his end.                     *
 *         When true, reset the buffers values and change the buffer.        *
 *         When false, end the ISR and wait for the next routine             *
 *****************************************************************************/
static void isr (void)
{
    uint16_t dac_left;
    uint16_t dac_right;
    uint16_t pwm_left;
    uint16_t pwm_right;

    if (!isr_buf->full)
    {
        SET_PH11();
    }
    else
    {
        CLR_PH11();

        dac_left = calc_dac(isr_buf->data[isr_buf->index]);
        dac_right = calc_dac(isr_buf->data[isr_buf->index + 1]);
        pwm_left = calc_pwm(isr_buf->data[isr_buf->index]);
        pwm_right = calc_pwm(isr_buf->data[isr_buf->index + 1]);

        dac_write(DAC_0, LEFT_CHANNEL, dac_left);
        dac_write(DAC_0, RIGHT_CHANNEL, dac_right);
        pwm_set(PWM_0, LEFT_CHANNEL, pwm_left);
        pwm_set(PWM_0, RIGHT_CHANNEL, pwm_right);

        isr_buf->index += 2;
        counter += 4;

        if (isr_buf->index >= FIFO_BUFF_SIZE)
        {
            isr_buf->index = 0;
            isr_buf->full = 0;

            if (isr_buf == &fifo_0)
            {
                isr_buf = &fifo_1;
            }
            else
            {
                isr_buf = &fifo_0;
            }
        }
    } /* else */
}

/*****************************************************************************
 * @brief TFT Service routine                                                *
 *                                                                           *
 * @detail Sets a flag, which signals that the display should be refreshed   *
 *****************************************************************************/
static void tft (void)
{
    tft_refresh = 1;
}

/*****************************************************************************
 * @brief Main application                                                   *
 *                                                                           *
 * @detail TODO                                                              *
 *****************************************************************************/
int main(void)
{
    HMP3Decoder mp3Decoder = NULL;
    int skip_bytes;
    int	bytes_left = MAINBUF_SIZE;
    int	status;
    unsigned char mem_data[MAINBUF_SIZE];
    unsigned char *mem_data_ptr = mem_data;

    /* Need to initialize the CEP-TI-LAB-BOARD */
    initCEP_Board();
    _reset_var();

    /* Initialize both buffer */
    fifo_0.full = 0;
    fifo_0.index = 0;
    fifo_1.full = 0;
    fifo_1.index = 0;

    /* Set both pointer to first buffer */
    bg_buf = &fifo_0;
    isr_buf = &fifo_0;

    /* Enable all needed GPIO clocks */
    GPIOA_CLKEN();
    GPIOC_CLKEN();
    GPIOF_CLKEN();
    GPIOH_CLKEN();
    GPIOI_CLKEN();
    WORK_CS_PORT_CLKEN();
    ORIG_CS_PORT_CLKEN();

    /* Initialize all needed peripheral low-level drivers */
    fsmc_init();                                  /**< FSMC interface */
    timer_init(TIMER_0, isr);                     /**< PWM Output timer */
    timer_init(TIMER_1, tft);                     /**< TFT Output timer */
    dac_init(DAC_0);                              /**< DAC (PA4) Analog Output */
    spi_init_master(SPI_0, SPI_BAUD_42MHZ_DIV_2); /**< SPI3 with 21 MHz */
    at25df641_init(AT25DF641_1);                  /**< init work memory */
    gpio_init(GPIO_DIR_OUT, GPIOI, PI7);          /**< D23 */
    gpio_init(GPIO_DIR_OUT, GPIOH, PH11);         /**< Underflow-LED */
    gpio_init(GPIO_DIR_OUT, GPIOI, PI6);          /**< D22 */
    gpio_init(GPIO_DIR_OUT, GPIOH, PH13);         /**< Wait-LED */

    /* Fills the memory buffer for the first time */
    at25df641_read(AT25DF641_1, mem_data, MAINBUF_SIZE, address);
    address += MAINBUF_SIZE;

    /* Initialize a new mp3 decoder */
    if ((mp3Decoder = MP3InitDecoder()) == 0) {
        perror("MP3InitDecoder() [ FAIL ]\n");
        return 1;
    }

    /* Main loop */
    while (1)
    {
        /*********************************************************************
         * @detail MP3 Play loop.                                            *
         *         1. Find the next word of the track                        *
         *         2. Set the LED PH13 and wait til background buffer is     *
         *            empty                                                  *
         *         3. Clear the LED PH13, because background buffer is empty *
         *         4. Decodes a new frame                                    *
         *         5. Starts the FSMC module                                 *
         *         6. Marks background buffer as full                        *
         *         7. Set the bg_buf pointer to another buffer               *
         *         8. Clean up the memory                                    *
         *         9. [Optional] check buttons when playing                  *
         *********************************************************************/
        while (forever)
        {
#if TFT_EN
            if (tft_refresh)
            {
                _lcd_out();
                tft_refresh = 0;
            }
#endif
            bytes_left = MAINBUF_SIZE;
            mem_data_ptr = mem_data;

            if ((skip_bytes = MP3FindSyncWord(mem_data, MAINBUF_SIZE)) > 0)
            {
                printf("Bytes skipped: %d\n", skip_bytes);
                address += skip_bytes;
                _update_memory(mem_data, mem_data_ptr, skip_bytes);
            }
            else if (skip_bytes < 0)
            {
                printf("MP3FindSyncWord() [ FAIL ]\n");
                forever = 0;
                break;
            }

            SET_PH13();
            while(bg_buf->full);
            CLR_PH13();

            if ((status = MP3Decode(mp3Decoder, &mem_data_ptr, &bytes_left, (short *)bg_buf->data, 0)) < 0)
            {
                printf("MP3Decode() [ ERROR %d ]\n", status);
                forever = 0;
                break;
            }

            _fsmc();

            bg_buf->full = 1;

            if (bg_buf == &fifo_0) {
                bg_buf = &fifo_1;
            }
            else {
                bg_buf = &fifo_0;
            }

            _update_memory(mem_data, mem_data_ptr, bytes_left);
            _check_buttons(mem_data);
        }  /* while (forever) */

        _check_buttons(mem_data);
    }  /* while (1) */

    return 0; // Never reached.
}
