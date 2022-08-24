/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "stm32l0xx_hal_i2c.h"
#include "tools.h"
#include "start_test.h"
#include "test_driver.h"
#include "piezo.h"
#include "msp_exp.h"
#include "eeprom_circular.h"
#include "msp_exp.h"
#include "msp_exp_state.h"
#include "interface_flags.h"
#include "tools.h"
#include "experiment_constants.h"
#include "power_management.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t recvBuffer[9];
//uint8_t recvBuffer[MSP_EXP_MAX_FRAME_SIZE];
long unsigned int recvLength = 0; // changed 220809, the amount of data in the buffer containing valid data
uint8_t sendBuffer[MSP_EXP_MAX_FRAME_SIZE];
long unsigned int sendLength = 0; // changed 220809, the amount of data in the buffer containing valid data
uint8_t volatile addr = 0x45; //defines the adress when only one i2c adress is used.
bool volatile has_function_to_execute = false;
void (* volatile command_ptr) () = NULL;
extern I2C_HandleTypeDef hi2c1;
void print16bit(uint8_t , uint8_t, uint8_t );
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void start_driver(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SystemClock_Config(void);
void RECIVE_FINISHED(I2C_HandleTypeDef *hi2c);
uint8_t IF_DIRECTION_IS_RECIVE(I2C_HandleTypeDef *hi2c);
uint8_t IF_DIRECTION_IS_SEND(I2C_HandleTypeDef *hi2c);
// void BUFF_LENTH(uint8_t* pBuffer, long *BufferLength);
uint8_t msp_error_code_receive;
uint8_t msp_error_code_send;



// TEST DRIVER debug 
uint16_t addr_debug = 0x0;
uint8_t piezoBufferDebug[200];
uint8_t current_state = 0x0;
uint8_t sic_test_data[BUFFERLENGTH];
// volatile uint16_t bffLength = BUFFERLENGTH;
uint16_t max_number_lines = 0;
uint16_t test_index = 0;
bool run_all = 0;
bool run_piezo = 0;
bool run_transceiver_RE_DE = 0;
bool run_10V = 1;
bool run_sic_test = 1;
bool run_dac = 1;


extern volatile struct msp_exp_state_information msp_exp_state;
bool ready = false;
//piezo_sic_type volatile piezo_sic = NONE;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_DAC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // the following funtion call will initialize the eeprom by clearing it.
  reset_EEPROM_buffer();
  restore_seqflags();
  turn_off_48v();
  turn_off_5v();
  turn_off_10v();
  turn_off_vbat();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //start_driver(); // If this line is included, runs the test program instead to be deleted in final version
	//test_driver();  // If this line is included, runs Bellmans test program in a separate file, easier to delete in final version
    if (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)recvBuffer, sizeof(recvBuffer)) != HAL_OK) {
        Error_Handler();
    }
   
    //wait for the i2c reception to finish this must timeout at some point, otherwise there is risk for getting stuck.
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

    // Assuming no overflow here, i.e. that we did not receive too much data and
    // that hi2c1.XferCount <= sizeof(recvBuffer)
    recvLength = sizeof(recvBuffer);
    //recvLength = sizeof(recvBuffer) - hi2c1.XferCount;

    msp_error_code_receive = msp_recv_callback((uint8_t *)recvBuffer, recvLength, addr);
    if (msp_error_code_receive != 0) {
      // TODO Handle error
    }

    if (recvBuffer[0] != MSP_OP_T_ACK) {
		msp_error_code_send = msp_send_callback((uint8_t *)sendBuffer, &sendLength, addr);
		if (msp_error_code_send != 0) {
			// TODO Handle error
		}

	    if (HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t *)sendBuffer, sendLength) != HAL_OK) {
	        Error_Handler();
	    }

    }


    if(has_function_to_execute)
    {
    	(*command_ptr) ();
         has_function_to_execute = false;
    }

    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void start_driver(void){
  while(1){
    // bffLength = BUFFERLENGTH;
    // This tests if 48V can be turned on and off
    
    if (run_transceiver_RE_DE){
     piezo_power_on();
     HAL_Delay(5000);
     piezo_power_off();
     // RS485(RS_MODE_TRANSMIT);
     // RS485(RS_MODE_RECEIVE);
     // HAL_Delay(5000);
     // RS485(RS_MODE_DEACTIVATE);
     // HAL_Delay(5000);
    }
    
    
    if(run_piezo || run_all){
      current_state = 0x1;
      piezo_start_exp();
      HAL_Delay(30000);
      
      current_state = 0x2;
      piezo_stop_exp();
      
      
      // piezo_get_data((uint8_t*) piezoBufferDebug, 0);
      uint16_t length = piezo_get_data_length();
      //printf("%d\n", length);
      
     
      
      for(uint8_t i = 0; i<length/2; i++){
        //printf("0x%X ", sic_test_data[i]);
       if(i % 8 == 0){
         //printf("\n");
        }
        print16bit(piezoBufferDebug[i*2], piezoBufferDebug[i*2+1], 1);
      }
      
      for(uint8_t i = 0; i<length; i++){
        //printf("0x%X ", sic_test_data[i]);
        if(i % 8 == 0){
          //printf("\n");
        }
        //printf("0x%02x\t", piezoBufferDebug[i]);
      }
      HAL_Delay(60000);
    }
    
    // Test if the LDO(U3_regul10V) can be turned on and off.
    // There is no pin for 10V, so a multimeter or probe will have to be used on U3
    if(run_10V || run_all){
      current_state = 0x3;
      sic_power_on();
      HAL_Delay(5000);
      
      current_state = 0x4;
      sic_power_off();
      HAL_Delay(5000);
      
    
    }
    
    if(run_sic_test || run_all){
      if(max_number_lines < 360){
        current_state = 0x4;
        start_test();
        // sic_get_data((uint8_t*) sic_test_data, 0);
        
       
        
        for(test_index = 0; test_index<BUFFERLENGTH/2; test_index++){
          //printf("0x%X ", sic_test_data[i]);
          
          if(test_index % 8 == 0 && test_index != 0){
            //printf("\n");
            max_number_lines++;
          }
          
          print16bit(sic_test_data[test_index*2], sic_test_data[test_index*2+1], 0);
          
        }
        //printf("\n");
        //sic_power_off();
        //HAL_Delay(100);
      }
      else {
        //printf("Please save data from the terminal now. Max number of lines almost reached.\n");
        max_number_lines = 0;
      }
      
    }
    
    if (run_dac){
      sic_power_on();
      setDAC_voltage(700);
      HAL_Delay(10000);
      
      setDAC_voltage(1200);
      HAL_Delay(10000);
      
      setDAC_voltage(2100);
      HAL_Delay(10000);
      
      setDAC_voltage(2800);
      HAL_Delay(10000);
      
      //setDAC(1.2*1241);
      //HAL_Delay(10000);
      
      //setDAC(2.1*1241);
     // HAL_Delay(10000);
      
     
    //  setDAC(2.8*1241);
    //  HAL_Delay(10000);
      
      sic_power_off();
      setDAC(0);
      HAL_Delay(3000);
      
      //sic_power_on();
      //setDAC(2.8*1241);
      HAL_Delay(1000);
    }
    //piezo_get_data(&piezoBufferDebug, 30);
  
  }
}

void print16bit(uint8_t biggest_number, uint8_t smallest_number, uint8_t number_format){
  uint16_t val = (biggest_number << 8) + smallest_number;
  switch(number_format){
    case 0:
      //printf("%d\t", val);
      break;
    
    case 1:
      //printf("0x%x\t", val);
      break;
      
    case 2:
      //printf("0x02%x\t", val);
      break;
    
  }
  //printf("%d\t", val);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
