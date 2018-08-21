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
interrupt void adc0(void);

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
}

void initGpio()
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

}

void initADC()
{
  EALLOW;
  CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
  delay();
  AdcaRegs.ADCCTL2.bit.PRESCALE = 6;
  AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;      //ADC_A0
  AdcaRegs.ADCSOC0CTL.bit.ACQPS = 24;     //acquiisition prescale
  AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 1;    //trig from TIM0
  AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;   //set pulse to late
  AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
  AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
  AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;      //power up
  PieCtrlRegs.PIEIER1.bit.INTx1 = 1;      //interrupt from int1
  AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
  delay();
  EDIS;

}


int main(void)
{
  IER = 0x0000;
  IFR = 0x0000;

  InitPieVectTable();
  EALLOW;
  PieVectTable.TIMER0_INT = &timer0;
  PieVectTable.ADCA1_INT = &adc0;
  EDIS;

  initGpio();
  configtimer0();
  initADC();

  EALLOW;
  IER = 0x0001;
  IFR = 0x0000;
  EINT;
  EDIS;

  while (1)
  {
  }

}

interrupt void timer0(void)
{
  pinGPIOToogle(12);
  PieCtrlRegs.PIEACK.bit.ACK1 = 1;

}

interrupt void adc0(void)
{
  PieCtrlRegs.PIEACK.bit.ACK1 = 1;
  AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
}
