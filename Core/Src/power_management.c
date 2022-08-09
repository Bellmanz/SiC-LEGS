/****************************************************************************
 * POWER MANAGEMENT FUNCTIONS                                               *
 ****************************************************************************/

/**
 *****************************************************************************
 * @file power_management.c
 * @date 2019-06-26 restructured 2022-08-08 CMZ
 * @bug no known bugs
 * @brief power management
 *****************************************************************************
 * this file contains the information of the experiment power state, active /
 * inactive, which is necessary in order to avoid removing power from a running
 * experiment if the two experiemnts are run simultaneously.
 * functions are defined in order to turn on and off the power buses individually
 */


#include "power_management.h"
#include "stdbool.h"
#include "usart.h"

/* data section */
bool sic_running = false;
bool piezo_running = false;

/**
 * @brief turns on power for piezo
 */
void piezo_power_on(void)
{
  piezo_running = true;
  if (!sic_running) {
	  turn_on_vbat();
  }
  turn_on_5v();
  turn_on_48v();
}

/**
 * @brief turns off power for piezo
 */
void piezo_power_off(void)
{
	turn_off_48v();
	turn_off_5v();
	if(!sic_running) {
		turn_off_vbat();
	}
   piezo_running = false;
}

/**
 * @brief turns on power for sic
 */
void sic_power_on(void)
{
  sic_running = true;
  if (!piezo_running) {
	  turn_on_vbat();
  }
  turn_on_10v();
}

/**
 * @brief turns off power for sic
 */
void sic_power_off(void)
{
	turn_off_10v();
	if(!piezo_running) {
		turn_off_vbat();
	}
	sic_running = false;
}

/**
 * @brief turns on vbat for Piezo / SiC
 */
void turn_on_vbat(void)
{
  HAL_GPIO_WritePin(GPIOB, Battery_SW_ON_Pin, GPIO_PIN_SET);
}

/**
 * @brief turns on 48 V for Piezo
 */
void turn_on_48v(void)
{
    HAL_GPIO_WritePin(GPIOB, Piezo_48V_ON_Pin, GPIO_PIN_SET);
}

/**
 * @brief turns on 10v for SiC
 */
void turn_on_10v(void)
{
   HAL_GPIO_WritePin(GPIOB, Linear_10V_ON_Pin, GPIO_PIN_SET);
}

/**
 * @brief turns on 5v for Piezo
 *
 * The pin controls a PMOSFET, therefore the polarity is reversed
 */
void turn_on_5v(void)
{
  HAL_GPIO_WritePin(GPIOB, Piezo_ON_Pin, GPIO_PIN_RESET);  // reverse polarity!
}

/**
 * @brief turns off vbat for Piezo / SiC
 */
void turn_off_vbat(void)
{
  HAL_GPIO_WritePin(GPIOB, Battery_SW_ON_Pin, GPIO_PIN_RESET);
}

/**
 * @brief turns off 48v for Piezo
 */
void turn_off_48v(void)
{
    HAL_GPIO_WritePin(GPIOB, Piezo_48V_ON_Pin, GPIO_PIN_RESET);
}

/**
 * @brief turns off 10v for SiC
 */
void turn_off_10v(void)
{
   HAL_GPIO_WritePin(GPIOB, Linear_10V_ON_Pin, GPIO_PIN_RESET);
}

/**
 * @brief turns off 5v for Piezo
 *
 * The pin controls a PMOSFET, therefore the polarity is reversed
 */
void turn_off_5v(void)
{
  HAL_GPIO_WritePin(GPIOB, Piezo_ON_Pin, GPIO_PIN_SET);  // reverse polarity!
}

