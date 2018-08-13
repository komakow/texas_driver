/**
 * main.c
 */

#include "F2837xS_device.h"
#include "DriverGPIO.h"

void delay()
{
  int a = 0;
  for (a; a < 30000; a++)
  {
    asm(" NOP");
  }
}

GPIOCfg_Type *pin1;

int main(void)
{

  pin1->direction = DIR_Output;
  pin1->pinFunction = MUX_0;
  pin1->type = ODR_NormalOutput;
  pin1->pin = 12;
  pin1->pull = PUD_Enable;
  pin1->core = CORE_CPU1;
  pin1->invert = INV_InvertDisable;
  pin1->samplingPeriod = 0;
  pin1->samplingQualification = QSEL_SYNC;
  pinGPIOCfg(pin1);

  pinGPIOCfg(pin1);
  EALLOW;
  while (1)
  {

    pinGPIOSet(12, GPIO_SET);
    delay();
    pinGPIOSet(12, GPIO_RESET);
    delay();

  }

}
