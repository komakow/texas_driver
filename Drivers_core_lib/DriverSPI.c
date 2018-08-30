/**
 * @file DriverSPI.c
 *
 * @Created on: 28 sie 2018
 * @Author: KamilM
 *
 * @brief Source file of abstract SPI driver working at tms320F28377S. All of SPI work with DMA
 */
#include "F2837xS_device.h"
#include "DriverSPI.h"

//******************************************************STATIC FUNCTION**************************************************

static err_spi SPI_CHECK(SPI_Cfg *config)
{
  err_spi ret = E_SPI_OK;
  uint32_t a = 0;

  //check correctness of spi type
  int spi = -1;
  for(a = SPI_MIN + 1;a < SPI_MAX; a++)
  {
    if(config->spi == a)
    {
      spi = a;
      break;
    }
  }

  //check correctness of mode type
  int mode = -1;
  for(a = MODE_MIN + 1;a < MODE_MAX; a++)
  {
    if(config->mode == a)
    {
      mode = a;
      break;
    }
  }

  int baudrate = 0;
  if(config->mode == MODE_MASTER)
  {
    baudrate = -1;
    for(a = 3 ;a < 129; a++)
    {
      if(config->baud_rate == a)
      {
        baudrate = a;
        break;
      }
    }
  }


  //check correctness of polarity
  int pol = -1;
  for(a = POL_MIN + 1;a < POL_MAX; a++)
  {
    if(config->polarity == a)
    {
      pol = a;
      break;
    }
  }

  //check correctness of word size
  int word = -1;
  for(a = WORD_MIN + 1;a < WORD_MAX; a++)
  {
    if(config->word_size == a)
    {
      word = a;
      break;
    }
  }

  //check correctness of phase
  int phase = -1;
  for(a = PHA_MIN + 1;a < PHA_MAX; a++)
  {
    if(config->phase == a)
    {
      phase = a;
      break;
    }
  }

  //check correctness of FIFO enabled
  int fifo = -1;
  for(a = FIFO_MIN + 1;a < FIFO_MAX; a++)
  {
    if(config->word_size == a)
    {
      fifo = a;
      break;
    }
  }

  int fifolvl = 0;
  //if fifo is enabled
  if(config->fifo_set == FIFO_ON)
  {
    //check correctness of FIFO enabled
    fifolvl = -1;
    for(a = FIFO_LVLMIN + 1;a < FIFO_LVLMAX; a++)
    {
      if(config->fifo_lvl == a)
      {
        fifolvl = a;
        break;
      }
    }
  }

  if( !((spi >= 0) && (mode >= 0) && (pol >= 0) && (word >=0) && (phase >= 0) && (fifo >= 0) && (fifolvl >= 0) && (baudrate >= 0) ))
  {
    ret = E_SPI_INVALID_PARAM;
  }

  return ret;
}

static void SPIA_SETUP_FIFO(SPI_Cfg *config)
{
  //enable FIFO TX, interrupt lvl bits
  SpiaRegs.SPIFFTX.all = ((0x6000) | (uint16_t)config->fifo_lvl);

  //interrupt RX lvl bits
  SpiaRegs.SPIFFRX.all = ((0x0000) | (uint16_t)config->fifo_lvl);

  //no transmit delay
  SpiaRegs.SPIFFCT.all = 0x00;

  //release transmit FIFO from reset
  SpiaRegs.SPIFFTX.bit.TXFIFO = 1;

  //re-enable RX FIFO operation
  SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
}

static void SPIA_SETUP_POLARITY(SPI_Cfg *config)
{
  if(config->polarity == POL_FALLING)
  {
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
  }
  else
  {
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
  }

}

static void SPIA_SETUP_WORDSIZE(SPI_Cfg *config)
{
  SpiaRegs.SPICCR.bit.SPICHAR = (uint16_t)config->word_size;
}

static void SPIA_SETUP_PHASE(SPI_Cfg *config)
{
  if(config->phase == PHA_DELAY)
  {
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
  }
  else
  {
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
  }

}

static void SPIA_SETUP_BITRATE(SPI_Cfg *config)
{
  SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = (uint16_t)config->baud_rate;
}

static void SPIA_SETUP_MODE(SPI_Cfg *config)
{
  if(config->mode == MODE_MASTER)
  {
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
  }
  else
  {
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 0;
  }
}

static void SPIA_SET_RESET(uint16_t state)
{
  if(state == 0)
  {
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
  }
  else if(state == 1)
  {
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
  }
}

static void SPIA_ENABLE_TALK(uint16_t talk)
{
  if(talk == 0)
  {
    //disable talk
    SpiaRegs.SPICTL.bit.TALK = 0;
  }
  else if(talk == 1)
  {
    //enable talk
    SpiaRegs.SPICTL.bit.TALK = 1;
  }

}

static void SPIA_CONFIG(SPI_Cfg *config)
{
  if(config->fifo_set == FIFO_ON)
  {
    //config FIFO for RX and TX
    SPIA_SETUP_FIFO(config);
  }

  SPIA_SET_RESET(0);                //reset SPI
  SPIA_SETUP_POLARITY(config);      //setup polatiry
  SPIA_SETUP_WORDSIZE(config);      //setup word size

  if(config->mode == MODE_MASTER)
  {
    //setup bitrane olny if SPI work as master
    SPIA_SETUP_BITRATE(config);
  }

  SPIA_SETUP_MODE(config);            //setup master or slave mode
  SPIA_SETUP_PHASE(config);           //setup clock phase
  SPIA_ENABLE_TALK(1);                //enable talk
  SPIA_SET_RESET(1);                  //enable SPI
}

//******************************************************INTERFACE FUNCTION************************************************


err_spi spiCfg(SPI_Cfg *config)
{
  err_spi ret = E_SPI_OK;

  switch(config->spi)
  {
    case SPI_A:
    {
      //check correctness of struct parameters
      if(SPI_CHECK(config) == 0)
      {
        SPIA_CONFIG(config);            //configure and enable SPI_A
      }
      else
      {
        ret = E_SPI_INVALID_PARAM;
        break;
      }
      ret = E_SPI_OK;
      break;
    }
    case SPI_B:
    case SPI_C:
    {
      ret = E_SPI_NOT_INITIALIZE;
      break;
    }

  }
  return ret;
}

