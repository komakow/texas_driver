/**
 * @DriverGPIO.h
 *
 * @Created on: 31 lip 2018
 * @Author: KamilM
 *
 * @brief Header file for abstract GPIO driver.
 * Header file include only for 'uint_32' and other numeric types.
 */

#ifndef DRIVERGPIO_H_
#define DRIVERGPIO_H_

#include <stdint.h>

typedef int err;


/**
 * @brief Numeric representation of error. TODO: multiple if necessary
 */
#define E_GPIO_OK                0                   //Operation successful
#define E_GPIO_NOT_INITIALIZE   -1                   //GPIO not initialized
#define E_GPIO_INVALID_PARAM    -2                   //Invalid input parameters

/**
 * @brief Numeric representation state of pin
 */
typedef enum
{
  GPIO_SET,                //set state of specific pin
  GPIO_RESET               //reset state of specific pin
} GPIOSet_Type;

/**
 * brief Input qualification type:
 */
typedef enum
{
  QSEL_MIN = 0,               //Not related to GPIO, for debug purpose
  QSEL_SYNC,                  //Synchronization
  QSEL_3Samples,              //Qualification (3 samples)
  QSEL_6Samples,              //Qualification (6 samples)
  QSEL_Async,                 //Async (no Sync or Qualification)
  QSEL_MAX                    //Not related to GPIO, for debug purpose

} GPIOQsel_Type;

/**
 * @brief Numeric representation of other function which can be selected
 */
typedef enum
{
  MUX_MIN = 0,                    //Not related to GPIO, for debug purpose
  MUX_0,                          //zero mux
  MUX_1,                          //first mux
  MUX_2,                          //etc
  MUX_3,
  MUX_4,
  MUX_5,
  MUX_6,
  MUX_7,
  MUX_8,
  MUX_9,
  MUX_10,
  MUX_11,
  MUX_12,
  MUX_13,
  MUX_14,
  MUX_15,
  MUX_MAX                         //Not related to GPIO, for debug purpose

} GPIOMux_Type;

/**
 * @brief Direction Register
 */
typedef enum
{
  DIR_MIN = 0,            //Not related to GPIO, for debug purpose
  DIR_Input,              //GPIO as a input
  DIR_Output,             //GPIO as a output
  DIR_MAX                 //Not related to GPIO, for debug purpose

} GPIODir_Type;

/**
 * @brief Disables or enables the Pull-Up on GPIO.
 */
typedef enum
{
  PUD_MIN = 0,        //Not related to GPIO, for debug purpose
  PUD_Enable,         //Enables the Pull-up resistors
  PUD_Disable,        //Disables the Pull-up resistors
  PUD_MAX             //Not related to GPIO, for debug purpose

} GPIOPud_Type;

/**
 * @brief Input Polarity Invert Registers. Selects between non-inverted and inverted GPIO input to the device
 */
typedef enum
{
  INV_MIN = 0,                //Not related to GPIO, for debug purpose
  INV_InvertDisable,          //Non invert GPIO input
  INV_InvertEnable,           //Invert GPIO input
  INV_MAX                     //Not related to GPIO, for debug purpose

} GPIOInv_Type;

/**
 * @briefOpen Drain Output Register. Selects between normal and open-drain output for the GPIO pin
 */
typedef enum
{
  ODR_MIN = 0,                //Not related to GPIO, for debug purpose
  ODR_NormalOutput,           //Normal output of GPIO
  ODR_OpenDrainOutput,        //Open Drain output of GPIO
  ODR_MAX                     //Not related to GPIO, for debug purpose

} GPIOOdr_Type;

/**
 * @brief Core Select Register. Selects which master's GPIODAT/SET/CLEAR/TOGGLE registers control this GPIO pin
 */
typedef enum
{
  CORE_MIN = 0,               //Not related to GPIO, for debug purpose
  CORE_CPU1,                  //CPU1 selected as master
  CORE_CLA,                   //CLA selected as master
  CORE_MAX                    //Not related to GPIO, for debug purpose

} GPIOCore_Type;

/**
 * @brief Lock Configuration Register. GPyMUX1, GPyMUX2, GPyDIR, GPyINV, GPyODR, GPyAMSEL, GPyGMUX1, GPyGMUX2 and GPyCSELx
 * can't be changed
 */
typedef enum
{
  LOCK_MIN = 0,               //Not related to GPIO, for debug purpose
  LOCK_Disable,               //Lock not enabled
  LOCK_Enable,                //Lock enabled
  LOCK_MAX                    //Not related to GPIO, for debug purpose

} GPIOLock_Type;

typedef struct
{
    /*
     * 1 - pin_1
     * 2 - pin_2
     * etc
     */
    int32_t pin;

    /*
     * Qualification Sampling Period Control
     * 0 - QUALPRDx = PLLSYSCLK
     * 1 - QUALPRDx = PLLSYSCLK/2
     * 2 - QUALPRDx = PLLSYSCLK/4
     * i.e
     */
    int32_t samplingPeriod;

    /*
     *MUX_1,                          //first mux
     *MUX_2,                          //i.e
     */
    GPIOMux_Type pinFunction;

    /*
     *     QSEL_SYNC,                  //Synchronization
     *     QSEL_3Samples,              //Qualification (3 samples)
     *     QSEL_6Samples,              //Qualification (6 samples)
     *     QSEL_Async,                 //Async (no Sync or Qualification)
     */
    GPIOQsel_Type samplingQualification;

    /*
     *     DIR_Input - GPIO as a input
     *     DIR_Output - GPIO as a output
     */
    GPIODir_Type direction;

    /*
     *     PUD_Enable - Enables the Pull-up resistors
     *     PUD_Disable - Disables the Pull-up resistors
     */
    GPIOPud_Type pull;

    /*
     *     INV_InvertDisable - Non invert GPIO input
     *     INV_InvertEnable - Invert GPIO input
     */
    GPIOInv_Type invert;

    /*
     *     ODR_NormalOutput - Normal output of GPIO
     *     ODR_OpenDrainOutput - Open Drain output of GPIO
     */
    GPIOOdr_Type type;

    /*
     *    CORE_CPU1 - CPU1 selected as master
     *    CORE_CLA - CLA selected as master
     */
    GPIOCore_Type core;

} GPIOCfg_Type;

/**
 * @brief Function used to config specific pin by settings from structure
 *
 * @param GPIOCfg_Type* gpio - pointer to config structure
 *
 * @return Status of operation
 */
err
pinGPIOCfg(GPIOCfg_Type* gpio);

/**
 * @brief Function used to change state specific pin.
 *
 * @param Uint32 pin - numeric representation on specific pin
 * @param GPIOSet_Type status - state which specific pin should be changed
 *
 * @return Status of operation
 */
err
pinGPIOSet(uint32_t pin, GPIOSet_Type state);

/**
 * @brief Function used to enable or disable lock at specific pin
 *
 * @param Uint32 pin - numeric representation on specific pin
 * @param GPIOLock_Type lock - state which specific pin should be changed
 */
err
pinLOCKset(uint32_t pin, GPIOLock_Type lock);

/**
 * @brief Function used to read state of input pin
 *
 * @param uint32_t pin - number of specific pin which state should be read
 *
 * @return State of pin
 */
uint32_t
pinGPIORead(uint32_t pin);

/**
 * @brief Function used to toogle pin by Data register
 *
 * @param uint32_t pin - number of specific pin which state should be toogle
 *
 * return State of operation
 */
err
pinGPIOToogle(uint32_t pin);
#endif /* DRIVERGPIO_H_ */
