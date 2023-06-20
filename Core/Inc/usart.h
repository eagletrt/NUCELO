/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stdbool.h"

/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define LOG_HUART huart2

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
typedef enum { NORM_HEADER = 0U, LOG_HEADER, CAN_HEADER, CAN_ERR_HEADER, PCU_HEADER, NO_HEADER } UART_HeaderTypeDef;
/**
 * @brief Print messages over the serial UART, prefixed with header "[NUCELO-F4-BENCH]"
 * @param txt         Pointer to input string without \r\n and null terminated
 * @param header_type Type of postfix appended to the header, can be a parameter of @arg UART_HeaderTypeDef
 * */
void print_log(char *txt, UART_HeaderTypeDef header_type);

void UART_disable_log(void);

void UART_enable_log(void);
bool UART_is_log_on();
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

