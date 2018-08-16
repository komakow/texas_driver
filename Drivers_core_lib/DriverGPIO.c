/**
 * @DriverGPIO.c
 *
 * @Created on: 31 lip 2018
 * @Author: KamilM
 *
 * @brief source code of abstract GPIO driver working at F2837xS devices.
 * For uses at other MCU you should change header file by include it.
 */

#include "DriverGPIO.h"
#include "F2837xS_device.h"

//OFFSET of first CTRL register of GPIO
#define GPIO_CTRL_REG_F2877S 0x00007C00
#define GPIO_DATA_REG_F2877S 0x00007F00

//defines of specific hardware parameters
#define NUMBER_OF_PINS              168
#define SAMPLING_PERIOD_DIVIDER     512

#define GPY_CTRL_OFFSET    (0x40/2)
#define GPY_DATA_OFFSET    (0x8/2)

#define GPYQSEL            (0x2/2)
#define GPYMUX             (0x6/2)
#define GPYDIR             (0xA/2)
#define GPYPUD             (0xC/2)
#define GPYINV             (0x10/2)
#define GPYODR             (0x12/2)
#define GPYGMUX            (0x20/2)
#define GPYCSEL            (0x28/2)
#define GPYLOCK            (0x3C/2)
#define GPYCR              (0x3E/2)

#define GPYDAT             (0x0/2)
#define GPYSET             (0x2/2)
#define GPYCLEAR           (0x4/2)
#define GPYTOGGLE          (0x6/2)

/**
 * @brief Address of specific GPIO registers
 */
typedef struct
{
  volatile uint32_t *base_address;
  volatile uint32_t *ctrl;
  volatile uint32_t *qsel;
  volatile uint32_t *mux;
  volatile uint32_t *dir;
  volatile uint32_t *pud;
  volatile uint32_t *inv;
  volatile uint32_t *odr;
  volatile uint32_t *gmux;
  volatile uint32_t *csel;
  volatile uint32_t *lock;
  volatile uint32_t *cr;

} GPIO_RegisterAddress;

/**
 * @brief Offset of pin
 */
typedef struct
{
  uint16_t pin32;
  uint16_t pin16;
  uint16_t pin8;
  uint16_t pin4;
} GPIO_PinOffset;

/**
 * @brief Static function used to set register address
 */
static void GPIO_REGISTER(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOffset)
{
  //offset of pin
  pinOffset->pin32 = gpio->pin % 32;
  pinOffset->pin16 = gpio->pin % 16;
  pinOffset->pin8 = gpio->pin % 8;
  pinOffset->pin4 = gpio->pin % 4;

  //address of specific registers. Make a array of register for easy numerical operation
  registers->base_address = (uint32_t *) GPIO_CTRL_REG_F2877S + (gpio->pin / 32) * GPY_CTRL_OFFSET;
  registers->ctrl = registers->base_address;
  registers->qsel = registers->base_address + GPYQSEL + (pinOffset->pin32) / 16;
  registers->mux = registers->base_address + GPYMUX + (pinOffset->pin32) / 16;
  registers->dir = registers->base_address + GPYDIR;
  registers->pud = registers->base_address + GPYPUD;
  registers->inv = registers->base_address + GPYINV;
  registers->odr = registers->base_address + GPYODR;
  registers->gmux = registers->base_address + GPYGMUX + (pinOffset->pin32) / 16;
  registers->csel = registers->base_address + GPYCSEL + (pinOffset->pin32) / 8;
  registers->lock = registers->base_address + GPYLOCK;
  registers->cr = registers->base_address + GPYCR;
}

static void GPIO_LOCK_REGISTER(int32_t pin, volatile uint32_t *lock_register)
{
  //base register
  volatile uint32_t* base = (uint32_t *) GPIO_CTRL_REG_F2877S + (pin / 32) * GPY_CTRL_OFFSET;
  lock_register = base + GPYLOCK;       //address to lock register
}

static void GPIO_SETUP_MUX(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  EALLOW;
  *registers->mux &= ~(0x3UL << (2 * pinOff->pin16));                                         //clear specific place at register
  *registers->gmux &= ~(0x3UL << (2 * pinOff->pin16));

  //Data Synchronization Barrier, wait until memory is idle
  //__asm("DSB");
  *registers->gmux |= (uint32_t)(((gpio->pinFunction-1) >> 2) & 0x3UL) << (2 * pinOff->pin16);    //from 0b1111 take only younger '11' and write to register
  *registers->mux |= (uint32_t)((gpio->pinFunction-1) & 0x3UL) << (2 * pinOff->pin16);            //take older '11'
  EDIS;
}

static void GPIO_SETUP_INPUT(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  uint32_t pinMask = 0;
  pinMask = 1UL << pinOff->pin32;               //make mask of for one bit in register

  EALLOW;

  //input mode
  *registers->dir &= ~pinMask;

  //pull up resistor enable/disable
  if (gpio->pull == PUD_Enable)
  {
    *registers->pud &= ~pinMask;
  }
  else
  {
    *registers->pud |= pinMask;
  }

  //invert gpio enable/disable
  if (gpio->invert == INV_InvertEnable)
  {
    *registers->inv |= pinMask;
  }
  else
  {
    *registers->inv &= ~pinMask;
  }

  EDIS;
}

static void GPIO_SETUP_OUTPUT(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  uint32_t pinMask = 0;
  pinMask = (1UL << pinOff->pin32);               //make mask of for one bit in register

  EALLOW;

  //output mode
  *registers->dir |= pinMask;

  //open-drain or normal output mode
  if(gpio->type == ODR_NormalOutput)
  {
    *registers->odr &= ~pinMask;
  }
  else
  {
    *registers->odr |= pinMask;
  }

  //pull up enable/disable
  if(gpio->pull == PUD_Enable)
  {
    *registers->pud &= ~pinMask;
  }
  else
  {
    *registers->pud |= pinMask;
  }

  EDIS;
}

static void GPIO_SETUP_SAMPLING(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  EALLOW;

  //Qualifier Select
  *registers->qsel &= ~(0x3UL << (2 * pinOff->pin16));       //clear specific place in register

  //Data Synchronization Barrier, wait until memory is idle
  //__asm(:"DSB");
  *registers->qsel |= (gpio->samplingQualification - 1) << (2 * pinOff->pin16);     //minus '1' because at interface is numerate from '1'

  //Qualification Sampling Period Control
  *registers->ctrl &= ~(0x8UL << (8 * pinOff->pin4));                                  //clear specific place in register

   //Data Synchronization Barrier, wait until memory is idle
  //__asm(:"DSB");
  *registers->ctrl |= (gpio->samplingPeriod << (8 * pinOff->pin4));

  EDIS;
}

static void GPIO_SETUP_INVERT(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  uint32_t pinMask = 0;
  pinMask = (1UL << pinOff->pin32);               //make mask of for one bit in register

  EALLOW;
  if(gpio->invert == INV_InvertEnable)
  {
    *registers->inv |= pinMask;
  }
  else
  {
    *registers->inv &= ~pinMask;
  }
  EDIS;
}

static void GPIO_SETUP_CORE(GPIOCfg_Type* gpio, GPIO_RegisterAddress *registers, GPIO_PinOffset *pinOff)
{
  EALLOW;

  *registers->csel &= ~(0x3UL << (4*pinOff->pin8));   //clear specific place in register

  if(gpio->core == CORE_CLA)
  {
    *registers->csel |= (uint32_t)(1 << (4 * pinOff->pin8));  //set CLA
  }

  EDIS;
}

static err GPIO_CHECK(GPIOCfg_Type* gpio)
{
  err ret = E_GPIO_OK;

  //Check correctness of number of pin
  int pin = -1;
  uint32_t i = 0;
  for(i = 0; i < (uint32_t)NUMBER_OF_PINS; i++)
  {
    if(gpio->pin == i)
    {
      pin = i;
      break;
    }
  }

  //Check correctness of number of divider sampling period divider
  int sampling_period = -1;
  for(i = 0; i < (uint32_t)SAMPLING_PERIOD_DIVIDER; i++)
  {
    if(gpio->samplingPeriod == i)
    {
      sampling_period = i;
      break;
    }
  }

  //Check correctness number of multiplexing pin function
  int pin_function = -1;
  for(i = MUX_MIN; i < MUX_MAX; i++)
  {
    if(gpio->pinFunction == i)
    {
      pin_function = i;
      break;
    }
  }

  //Check correctness of sampling qualification
  int qualification = -1;
  for(i = QSEL_MIN; i < QSEL_MAX; i++)
  {
    if(gpio->samplingQualification == i)
    {
      qualification = i;
      break;
    }
  }

  //Check correctness of pin direction
  int direction = -1;
  for(i = DIR_MIN; i < DIR_MAX; i++)
  {
    if(gpio->direction == i)
    {
      direction = i;
      break;
    }
  }

  //Check correctness of pull up
  int pull = -1;
  for(i = PUD_MIN; i < PUD_MAX; i++)
  {
    if(gpio->pull == i)
    {
      pull = i;
      break;
    }
  }

  //Check correctness of pin invert
  int invert = -1;
  for(i = INV_MIN; i < INV_MAX; i++)
  {
    if(gpio->invert == i)
    {
      invert = i;
      break;
    }
  }

  //Check correctness of pin output type
  int type = -1;
  for(i = ODR_MIN; i < ODR_MAX; i++)
  {
    if(gpio->type == i)
    {
      type = i;
      break;
    }
  }

  //Check correctness of pin core
  int core = -1;
  for(i = CORE_MIN; i < CORE_MAX; i++)
  {
    if(gpio->core == i)
    {
      core = i;
      break;
    }
  }

  if(!((pin > -1) && (sampling_period > -1) && (pin_function > 0) && (qualification > 0) && (direction > 0) && (pull > 0) && (invert > 0) && (type > 0) && (core > 0)))
  {
    ret = E_GPIO_INVALID_PARAM;
  }

  return ret;
}


//********************************************INTERFACE*********************************************************

err pinGPIOCfg(GPIOCfg_Type *gpio)
{
  err ret = E_GPIO_OK;

  //check correctness of parameters in gpio struct
  if (GPIO_CHECK(gpio) == E_GPIO_OK)
  {
    //struct with registers address
    GPIO_RegisterAddress registers_array;
    //struct with pin offset
    GPIO_PinOffset pin_offset;

    GPIO_REGISTER(gpio, &registers_array, &pin_offset);         //setup register and pin offset array
    GPIO_SETUP_SAMPLING(gpio, &registers_array, &pin_offset);   //setup sampling
    GPIO_SETUP_MUX(gpio, &registers_array, &pin_offset);        //setup multiplexing function of pin
    GPIO_SETUP_CORE(gpio, &registers_array, &pin_offset);       //setup core for pin

    if(gpio->direction == DIR_Output)
    {
    GPIO_SETUP_OUTPUT(gpio, &registers_array, &pin_offset);     //setup output
    }
    else
    {
      GPIO_SETUP_INPUT(gpio, &registers_array, &pin_offset);    //setup input
      GPIO_SETUP_INVERT(gpio, &registers_array, &pin_offset);   //invert signal at pin
    }
  }
  return ret;
}

err pinGPIOSet(uint32_t pin, GPIOSet_Type state)
{
  err ret = E_GPIO_OK;
  uint32_t pinMask = 0;

  EALLOW;
  //address of data register
  volatile uint32_t *data_reg;
  data_reg = data_reg = (uint32_t *)GPIO_DATA_REG_F2877S + ((pin / 32) * GPY_DATA_OFFSET);

  pinMask = 1UL << (pin % 32);

  if(state == GPIO_SET)
  {
    data_reg = data_reg + GPYSET;
    *data_reg |= pinMask;  //set pin
  }
  else
  {
    data_reg = data_reg + GPYCLEAR;
    *data_reg |= pinMask;  //reset pin
  }

  EDIS;
  return ret;
}

err pinGPIOToogle(uint32_t pin)
{
  err ret = E_GPIO_OK;
  uint32_t pinMask = 0;

  EALLOW;
  //address of data register
  volatile uint32_t *data_reg;
  data_reg = data_reg = (uint32_t *)GPIO_DATA_REG_F2877S + ((pin / 32) * GPY_DATA_OFFSET);
  pinMask = 1UL << (pin % 32);
  data_reg = data_reg + GPYTOGGLE;
  *data_reg |= pinMask;

  return ret;
}

uint32_t pinGPIORead(uint32_t pin)
{
  uint32_t state;

  EALLOW;
  //address of data register
  volatile uint32_t *data_reg;
  data_reg = data_reg = (uint32_t *)GPIO_DATA_REG_F2877S + ((pin / 32) * GPY_DATA_OFFSET);

  state = (data_reg[GPYDAT] >> (pin % 32)) & 0x1;

  EDIS;
  return state;
}

err pinLOCKset(uint32_t pin, GPIOLock_Type lock)
{
  err ret = E_GPIO_OK;

  uint32_t pinMask = 0;
  uint16_t pin32 = pin % 32;

  pinMask = (1UL << pin32);               //make mask of for one bit in register

  volatile uint32_t *lock_reg = NULL;
  GPIO_LOCK_REGISTER(pin, lock_reg);    //get address of lock register

  EALLOW;
  if(lock == LOCK_Enable)
  {
    *lock_reg |= pinMask;
  }
  else if(lock == LOCK_Disable)
  {
    *lock_reg &= ~pinMask;
  }
  else
  {
    ret = E_GPIO_INVALID_PARAM;
  }
  EDIS;
  return ret;
}

