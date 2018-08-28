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
  MODE_MIN = 0,       //Not related to SPI, for debug purpose

  SPI_1,              //SPI number one
  SPI_2,
  SPI_3,
  MODE_MAX            //Not related to SPI, for debug purpose

}SPIType;

/**
 * @brief Network mode control of SPI
 */
typedef enum
{
  MODE_MIN = 0,       //Not related to SPI, for debug purpose

  MODE_SLAVE,         //Work as a slave
  MODE_MASTER,        //Work as a master
  MODE_MAX            //Not related to SPI, for debug purpose

}SPI_ModeType;

/**
 * @brief Clock polarity control
 */
typedef enum
{
  POL_MIN = 0,        //Not related to SPI, for debug purpose

  POL_RISING,         //Data is output on rising edge and input on falling edge
  POL_FALLING,        //Data is output on falling edge and input on rising edge
  POL_MAX             //Not related to SPI, for debug purpose

}SPI_PolarityType;

/**
 * @brief Size of SPI word
 */
typedef enum
{
  WORD_MIN = 0,        //Not related to SPI, for debug purpose

  WORD_2b,             //2-bit word
  WORD_4b,             //4-bit word
  WORD_8b,             //8-bit word
  WORD_16b,           //16-bit word
  POL_MAX             //Not related to SPI, for debug purpose

}SPI_WordType;

/**
 * @brief Clock Phase Select
 */
typedef enum
{
  PHA_MIN = 0,        //Not related to SPI, for debug purpose

  PHA_NORMAL,         //Normal SPI clocking scheme, depending on the CLOCK POLARITY
  PHA_DELAY,          //SPICLK signal delayed by one half-cycle
  PHA_MAX             //Not related to SPI, for debug purpose

}SPI_PhaseType;

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
     * WORD_2b,             //2-bit word
     * WORD_4b,             //4-bit word
     * WORD_8b,             //8-bit word
     * WORD_16b,           //16-bit word
     */
    SPI_WordType word_size;

    /*
     *   PHA_NORMAL,         //Normal SPI clocking scheme, depending on the CLOCK POLARITY
     *   PHA_DELAY,          //SPICLK signal delayed by one half-cycle
     */
    SPI_PhaseType phase;

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
 * @brief Function enable save read data to readbuff
 *
 * @param SPIType spi       - numerate representation of used SPI
 * @uint16_t *readbuff      - Pointer to read circle buffer contains received data
 * @uint16_t bufflength     - length of circle read buffer
 *
 * @return Status of operation
 */
err_spi spiReadEnable(SPIType spi, uint16_t *readbuff, uint16_t bufflength);

/**
 * @brief Function enable DMA interrupt after send data from sendbuff
 */
void spiIRQ_TCEnable();

/**
 * @brief Function enable DMA interrupt after read 'bufflength' data
 */
void spiIRQ_RCEnable();

/**
 * @brief Function disable DMA interrupt after send data from sendbuff
 */
void spiIRQ_TCDisable();

/**
 * @brief Function disable DMA interrupt after read 'bufflength' data
 */
void spiIRQ_RCdisable();

#endif /* DRIVERSPI_H_ */
