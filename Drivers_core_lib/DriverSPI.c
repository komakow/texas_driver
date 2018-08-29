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

  if(!((spi >= 0) && (mode >= 0) && (pol >= 0) && (word >=0) && (phase >= 0) && (fifo >= 0) && (fifolvl >= 0) ))
  {
    ret = E_SPI_INVALID_PARAM;
  }

  return ret;
}




err_spi spiCfg(SPI_Cfg *config)
{
  err_spi ret = E_SPI_OK;


  return ret;
}

