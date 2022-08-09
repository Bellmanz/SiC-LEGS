/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IN0_VT_Si_Pin GPIO_PIN_0
#define IN0_VT_Si_GPIO_Port GPIOA
#define IN1_Vbe_Si_Pin GPIO_PIN_1
#define IN1_Vbe_Si_GPIO_Port GPIOA
#define IN2_Vb_Si_Pin GPIO_PIN_2
#define IN2_Vb_Si_GPIO_Port GPIOA
#define IN3_Vc_Si_Pin GPIO_PIN_3
#define IN3_Vc_Si_GPIO_Port GPIOA
#define IN5_VT_SiC_Pin GPIO_PIN_5
#define IN5_VT_SiC_GPIO_Port GPIOA
#define IN6_Vbe_SiC_Pin GPIO_PIN_6
#define IN6_Vbe_SiC_GPIO_Port GPIOA
#define IN7_Vb_SiC_Pin GPIO_PIN_7
#define IN7_Vb_SiC_GPIO_Port GPIOA
#define IN8_Vc_SiC_Pin GPIO_PIN_0
#define IN8_Vc_SiC_GPIO_Port GPIOB
#define Linear_10V_ON_Pin GPIO_PIN_10
#define Linear_10V_ON_GPIO_Port GPIOB
#define Piezo_48V_ON_Pin GPIO_PIN_11
#define Piezo_48V_ON_GPIO_Port GPIOB
#define Battery_SW_ON_Pin GPIO_PIN_12
#define Battery_SW_ON_GPIO_Port GPIOB
#define Piezo_ON_Pin GPIO_PIN_13
#define Piezo_ON_GPIO_Port GPIOB
#define USART1_NRE_Pin GPIO_PIN_11
#define USART1_NRE_GPIO_Port GPIOA
#define USART1_DE_Pin GPIO_PIN_12
#define USART1_DE_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
