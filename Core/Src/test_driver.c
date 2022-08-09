/**
 * @file      test_driver.c
 * @author    C-M Zetterling
 * @brief     Driver for running brief experiments without OBC
 *
 * @details
 * This driver starts and stops both the SiC experiment and Piezo-LEGS
 * 
 */

#include <stdio.h>
#include "stm32l0xx_hal.h"
#include "test_driver.h"
#include "power_management.h"
#include "start_test.h"
#include "piezo.h"

void test_driver(void) {

	uint8_t piezoBufferDebug[200];

	while(1) {

/*	  piezo_start_exp();
	  HAL_Delay(30000);  // run for 30 s

	  piezo_stop_exp();

	  piezo_get_data((uint8_t*) piezoBufferDebug, 0);
	  uint16_t length = piezo_get_data_length();
	  //printf("%d\n", length);
	  HAL_Delay(60000);
*/
		 turn_on_vbat();
		 HAL_Delay(3000);
		 turn_on_48v();
		 HAL_Delay(3000);
		 turn_on_10v();
		 HAL_Delay(3000);
		 turn_on_5v();
		 HAL_Delay(3000);
		 turn_off_5v();
		 HAL_Delay(3000);
		 turn_off_10v();
		 HAL_Delay(3000);
		 turn_off_48v();
		 HAL_Delay(3000);
		 turn_off_vbat();
		 HAL_Delay(15000);



/*		sic_power_on();
	    HAL_Delay(5000);
	    sic_power_off();
	    HAL_Delay(5000);

	    piezo_power_on();
	    HAL_Delay(5000);
	    piezo_power_off();
	    HAL_Delay(5000);
*/

	}
}


/*		uint32_t startGetTick;
		// instead of HAL_Delay(5000);
		startGetTick=HAL_GetTick();
		while((HAL_GetTick()-startGetTick) < 5000){
			// just wait
		}
*/
