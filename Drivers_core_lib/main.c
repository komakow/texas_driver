/**
 * main.c
 */

#include "F2837xS_device.h"
#include "DriverGPIO.h"
#include "F2837xS_pievect.h"

void delay()
{
  int a = 0;
  for (a; a < 30000; a++)
  {
    asm(" NOP");
  }
}

GPIOCfg_Type *pin1;
interrupt void timer0(void);
volatile uint32_t count;

void configtimer0(void)
{
  CpuTimer0Regs.TCR.bit.TSS = 1;   //stop timer

  CpuTimer0Regs.PRD.all = 5000000;  //period
  CpuTimer0Regs.TPR.bit.PSC = 0;   //prescaler by 1
  CpuTimer0Regs.TCR.bit.TRB = 1;   //reload
  CpuTimer0Regs.TCR.bit.TIE = 1;   //enable interrupt

  PieCtrlRegs.PIEIER1.bit.INTx7 = 1;    //hells knows
  CpuTimer0Regs.TCR.bit.TSS = 0;    //start timer

  IER |= M_INT1; //Enable group 1 interrupts
  EINT;
  ERTM;


}

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
  IER = 0x0000;
  IFR = 0x0000;

  InitPieVectTable();

  EALLOW;
  PieVectTable.TIMER0_INT = &timer0;
  EDIS;

  configtimer0();


  while (1)
  {
  }

}

interrupt void timer0(void)
{
  pinGPIOToogle(12);
  PieCtrlRegs.PIEACK.bit.ACK1 = 1;

}
