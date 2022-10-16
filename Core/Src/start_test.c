
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
//#include "iwdg.h"
#include "gpio.h"
//#include "usart.h"
//#include "SystemClock_Config.h"
//#include "SystemPower_Config.h"
#include "start_test.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "power_management.h"
#include "tools.h"
#include "experiment_constants.h"
//#include "header.h"




/* Structs -------------------------------------------------------------------*/
struct experiment_package {
  /*
  Si temperature
  Si Vbe (voltage frop from base to emitter)
  Si Vb (voltage over resistor on base)
  Si Vc (voltage over resistor on collector)
  SiC temperature
  SiC Vbe (voltage frop from base to emitter)
  SiC Vb (voltage over resistor on base)
  SiC Vc (voltage over resistor on collector)
  */
  uint16_t temperature1;
  uint16_t Vbe1;
  uint16_t Vb1;
  uint16_t Vc1;
  uint16_t temperature2;
  uint16_t Vbe2;
  uint16_t Vb2;
  uint16_t Vc2;

};

/* Private variables ---------------------------------------------------------*/

/* External variables*/
extern ADC_ChannelConfTypeDef           sConfigAdc;
extern DAC_ChannelConfTypeDef 	        sConfigDac;
extern ADC_HandleTypeDef                hadc;
extern DAC_HandleTypeDef    		hdac;
//extern UART_HandleTypeDef 		huart1;
//extern I2C_HandleTypeDef 		hi2c1;
static struct experiment_package  	experiments[EXPERIMENTPOINTS];

uint8_t buffer[BUFFERLENGTH];


// @brief Clears the experiment buffer
void clear_sic_buffer (void)
{
    Flush_Buffer8(buffer, BUFFERLENGTH);
}


void sic_get_data(unsigned char *buf, unsigned long len, unsigned long data_offset)
{
  for (unsigned long i = data_offset; (i < BUFFERLENGTH) && (i - data_offset < len); i++)
  {
    buf[i - data_offset] = buffer[i];
  }
}

/*
  @brief Runs the SiC in space experiment. If something is not working as it should,
         check if the voltage levels are set to the correct values. (Battery voltage, 48V voltage etc.)
*/
void start_test(void){
  sic_power_on();
  HAL_Delay(1000);

  // Make a dummy Read to fix incorrect start values?
  setDAC_voltage(DACMINIMUMVOLTAGE * 1.25);  // rescales for the call
  HAL_Delay(500);
  readADCvalues(0);

  for(uint16_t i = 0; i < EXPERIMENTPOINTS; i++){
    experiments[i].temperature1 = 0;
    experiments[i].Vbe1 = 0;
    experiments[i].Vb1 = 0;
    experiments[i].Vc1 = 0;
    experiments[i].temperature2 = 0;
    experiments[i].Vbe2 = 0;
    experiments[i].Vb2 = 0;
    experiments[i].Vc2 = 0;
  }

  uint16_t dac_voltage = DACMINIMUMVOLTAGE;
  // setDAC( Voltage * constant) = set DAC to Voltage.
  // Constant is used to translate actual voltage to binary value by factor 4095 steps (12 bit) / 3300 mV = 1.24
  // This was rounded up to 1.25 to work with integer steps of 100
  for(uint16_t index = 0; index < EXPERIMENTPOINTS; index++){
    setDAC_voltage(dac_voltage * 1.25);  // rescales for the call
    HAL_Delay(500);
    readADCvalues(index);
    dac_voltage = dac_voltage + DACSTEPS;
  }
  convert_8bit(buffer);

  setDAC_voltage(0);
  HAL_Delay(100);
  sic_power_off();

}

void readADCvalues(uint8_t index){

	ADC_ChannelConfTypeDef sConfig = {0};

	uint8_t AVE = 16; 	// each voltage point is measured AVE times and averaged by accumulation in the variable

	//Calibrate ADCs in the beginning of every run
	if(HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK){
		Error_Handler();
	}

	HAL_Delay(10);


	// Activate channel 0
	sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    		experiments[index].temperature1 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 1
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    		experiments[index].Vbe1 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 2
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	    if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    	    	experiments[index].Vb1 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 3
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	    if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    	    	experiments[index].Vc1 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 5
	sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    		experiments[index].temperature2 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 6
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    		experiments[index].Vbe2 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 7
    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	    if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    	    	experiments[index].Vb2 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



	// Activate channel 8
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

    //Start ADC reading
    if(HAL_ADC_Start(&hadc) != HAL_OK){ while(1) Error_Handler(); }

    for(uint8_t i = 0; i < AVE; i++){
    	HAL_ADC_PollForConversion(&hadc, 100);
    	    if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    	    	experiments[index].Vc2 += HAL_ADC_GetValue(&hadc);
    	}
    }

    // Stop ADC and reconfigure
    HAL_ADC_Stop (&hadc);

    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }



}


 void convert_8bit(uint8_t * buffer){
   //printf("\n temp si %d\n", experiments[0].temperature);

   uint16_t buffer_index = 0;
   uint16_t offset = EXPERIMENTPOINTS * 8;  // Used to repack buffer with Si values and then SiC values

   for(uint16_t experiment_index = 0; experiment_index < EXPERIMENTPOINTS; experiment_index++){
    buffer[buffer_index + 0] = experiments[experiment_index].temperature1 >> 8 & 0xFF;
    buffer[buffer_index + 1] = experiments[experiment_index].temperature1 & 0xFF;

    buffer[buffer_index + 2] = experiments[experiment_index].Vbe1 >> 8 & 0xFF;
    buffer[buffer_index + 3] = experiments[experiment_index].Vbe1 & 0xFF;

    buffer[buffer_index + 4] = experiments[experiment_index].Vb1 >> 8 & 0xFF;
    buffer[buffer_index + 5] = experiments[experiment_index].Vb1;

    buffer[buffer_index + 6] = experiments[experiment_index].Vc1 >> 8 & 0xFF;
    buffer[buffer_index + 7] = experiments[experiment_index].Vc1;

    buffer[buffer_index + 0 + offset] = experiments[experiment_index].temperature2 >> 8 & 0xFF;
    buffer[buffer_index + 1 + offset] = experiments[experiment_index].temperature2 & 0xFF;

    buffer[buffer_index + 2 + offset] = experiments[experiment_index].Vbe2 >> 8 & 0xFF;
    buffer[buffer_index + 3 + offset] = experiments[experiment_index].Vbe2 & 0xFF;

    buffer[buffer_index + 4 + offset] = experiments[experiment_index].Vb2 >> 8 & 0xFF;
    buffer[buffer_index + 5 + offset] = experiments[experiment_index].Vb2;

    buffer[buffer_index + 6 + offset] = experiments[experiment_index].Vc2 >> 8 & 0xFF;
    buffer[buffer_index + 7 + offset] = experiments[experiment_index].Vc2;

    buffer_index += 8;
   }
 }



/**
@brief SetDAC_voltage, Sets the voltage of the DAC to a specified value
@param uint32_t, value in mV to set DAC, but scaled by 1.25
@ The calling routine should calculate the corresponding digital value of the param with a reference
voltage set to 3300 mV and sets the DAC to the value.
Scale factor is 4095 steps / 3300 mV = 1.24 steps / mV
This was rounded up to 1.25 = 4095/3276 to work with integers in steps of 100
@return void
*/
void setDAC_voltage(uint32_t voltage){
	HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, voltage);
	HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
}

