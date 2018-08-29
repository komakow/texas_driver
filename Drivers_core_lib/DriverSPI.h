/**
 * @file DriverSPI.h
 *
 * @Created on: 28 sie 2018
 * @Author: KamilM
 *
 * @brief Header file of abstract SPI driver.
 */

#ifndef DRIVERSPI_H_
#define DRIVERSPI_H_

//for typedef like a uint8_t
#include <stdint.h>

typedef int err_spi;

/**
 * @brief Numeric representation of SPI error. Multiple if necessary.
 */
#define E_SPI_OK                   0     //Operation successful
#define E_SPI_INVALID_PARAM       -1     //Invalid parameters of config SPI
#define E_SPI_NOT_INITIALIZE      -2     //SPI is not initialize


/**
 * @brief Numeric representation of SPI
 */
typedef enum
{
  SPI_MIN = -1,       //Not related to SPI, for debug purpose

  SPI_1,              //SPI number one
  SPI_2,
  SPI_3,
  SPI_MAX            //Not related to SPI, for debug purpose

}SPIType;

/**
 * @brief Network mode control of SPI
 */
typedef enum
{
  MODE_MIN = -1,       //Not related to SPI, for debug purpose

  MODE_SLAVE,         //Work as a slave
  MODE_MASTER,        //Work as a master
  MODE_MAX            //Not related to SPI, for debug purpose

}SPI_ModeType;

/**
 * @brief Clock polarity control
 */
typedef enum
{
  POL_MIN = -1,        //Not related to SPI, for debug purpose

  POL_RISING,         //Data is output on rising edge and input on falling edge
  POL_FALLING,        //Data is output on falling edge and input on rising edge
  POL_MAX             //Not related to SPI, for debug purpose

}SPI_PolarityType;

/**
 * @brief Size of SPI word
 */
typedef enum
{
  WORD_MIN = -1,        //Not related to SPI, for debug purpose

  WORD_1b,             //1-bit word
  WORD_2b,             //2-bit word
  WORD_3b,             //2-bit word
  WORD_4b,             //4-bit word
  WORD_5b,             //2-bit word
  WORD_6b,             //6-bit word
  WORD_7b,             //7-bit word
  WORD_8b,             //8-bit word
  WORD_9b,             //9-bit word
  WORD_10b,             //10-bit word
  WORD_11b,             //11-bit word
  WORD_12b,             //12-bit word
  WORD_13b,             //13-bit word
  WORD_14b,             //14-bit word
  WORD_15b,             //15-bit word
  WORD_16b,           //16-bit word
  WORD_MAX             //Not related to SPI, for debug purpose

}SPI_WordType;

/**
 * @brief Clock Phase Select
 */
typedef enum
{
  PHA_MIN = -1,        //Not related to SPI, for debug purpose

  PHA_NORMAL,         //Normal SPI clocking scheme, depending on the CLOCK POLARITY
  PHA_DELAY,          //SPICLK signal delayed by one half-cycle
  PHA_MAX             //Not related to SPI, for debug purpose

}SPI_PhaseType;

/**
 * @brief FIFO used or not
 */
typedef enum
{
  FIFO_MIN = -1,         //Not related to SPI, for debug purpose

  FIFO_OFF,             //FIFO RX and TX is disabled
  FIFO_ON,              //FIFO RX and TX is enabled
  FIFO_MAX             //Not related to SPI, for debug purpose

}SPI_FIFOType;

/**
 * @brief FIFO interrupt level for TX and RX is equal
 */
typedef enum
{
  FIFO_LVLMIN = -1,         //Not related to SPI, for debug purpose

  FIFO_LVL_0,             //FIFO RX and TX interrupt request is generated when there is 0 or more words
  FIFO_LVL_1,             //FIFO RX and TX interrupt request is generated when there is 1 or more words
  FIFO_LVL_2,             //FIFO RX and TX interrupt request is generated when there is 2 or more words
  FIFO_LVL_3,             //FIFO RX and TX interrupt request is generated when there is 3 or more words
  FIFO_LVL_4,             //FIFO RX and TX interrupt request is generated when there is 4 or more words
  FIFO_LVL_5,             //FIFO RX and TX interrupt request is generated when there is 5 or more words
  FIFO_LVL_6,             //FIFO RX and TX interrupt request is generated when there is 6 or more words
  FIFO_LVL_7,             //FIFO RX and TX interrupt request is generated when there is 7 or more words
  FIFO_LVL_8,             //FIFO RX and TX interrupt request is generated when there is 8 or more words
  FIFO_LVL_9,             //FIFO RX and TX interrupt request is generated when there is 9 or more words
  FIFO_LVL_10,             //FIFO RX and TX interrupt request is generated when there is 10 or more words
  FIFO_LVL_11,             //FIFO RX and TX interrupt request is generated when there is 11 or more words
  FIFO_LVL_12,             //FIFO RX and TX interrupt request is generated when there is 12 or more words
  FIFO_LVL_13,             //FIFO RX and TX interrupt request is generated when there is 13 or more words
  FIFO_LVL_14,             //FIFO RX and TX interrupt request is generated when there is 14 or more words
  FIFO_LVL_15,             //FIFO RX and TX interrupt request is generated when there is 15 or more words
  FIFO_LVL_16,             //FIFO RX and TX interrupt request is generated when there is 16 or more words
  FIFO_LVLMAX             //Not related to SPI, for debug purpose

}SPI_FIFOLVLType;


typedef struct
{
    /*
     *   SPI_1,              //SPI number one
     *   SPI_2,
     *   SPI_3,
     */
    SPIType spi;

    /*
     *   MODE_SLAVE,         //Work as a slave
     *   MODE_MASTER,        //Work as a master
     */
    SPI_ModeType mode;

    /*
     *   POL_RISING,         //Data is output on rising edge and input on falling edge
     *   POL_FALLING,        //Data is output on falling edge and input on rising edge
     */
    SPI_PolarityType polarity;

    /*
     * WORD_1b,             //1-bit word
     * WORD_2b,             //2-bit word
     * WORD_3b,             //3-bit word
     * ...
     * WORD_16b,           //16-bit word
     */
    SPI_WordType word_size;

    /*
     *   PHA_NORMAL,         //Normal SPI clocking scheme, depending on the CLOCK POLARITY
     *   PHA_DELAY,          //SPICLK signal delayed by one half-cycle
     */
    SPI_PhaseType phase;

    /*
     *   FIFO_OFF,             //FIFO RX and TX is disabled
     *   FIFO_ON,              //FIFO RX and TX is enabled
     */
    SPI_FIFOType fifo_set;

    /*
     *   FIFO_LVL_0,             //FIFO RX and TX interrupt request is generated when there is 0 or more words
     *   FIFO_LVL_1,             //FIFO RX and TX interrupt request is generated when there is 1 or more words
     *   FIFO_LVL_2,             //FIFO RX and TX interrupt request is generated when there is 2 or more words
     *   FIFO_LVL_3,             //FIFO RX and TX interrupt request is generated when there is 3 or more words
     *   ...
     *   FIFO_LVL_16,            //FIFO RX and TX interrupt request is generated when there is 16 or more words
     */
    SPI_FIFOLVLType fifo_lvl;

}SPI_Cfg;


/**
 * @brief Function used to initialize SPI with specific parameters
 *
 * @param SPI_Cfg *config - pointer to initialize struct
 *
 * @return Status of operation
 */
err_spi spiCfg(SPI_Cfg *config);

/**
 * @brief Function used to send data thru SPI. Function send word of size initial by 'spiCfg()' function.
 * i.e If you try send data with different word size function send only first 'size' bits of your data
 * i.e without report any error.
 * i.e Driver do not copy sendbuff, so application MUST hold buff until data is send.
 *
 * @param SPIType spi             - numerate representation of used SPI
 * @param uint16 *sendbuff        - pointer to buff with data to send
 * @param uint16_t length         - number of data to send
 *
 * @return Status of operation
 */
err_spi spiSend(SPIType spi, uint16_t *sendbuff, uint16_t length);

/**
 * @brief Function read data from specific SPI
 *
 * @param SPIType spi  - numerate representation of used SPI
 *
 * @return Status of operation
 */
err_spi spiRead(SPIType spi);

/**
 * @brief Function enable interrupt from
 */
void spiIRQ_ReadEnable();

/**
 * @brief Function disable DMA interrupt after read 'bufflength' data
 */
void spiIRQ_ReadDisable();

#endif /* DRIVERSPI_H_ */
