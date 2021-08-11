/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "common.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void) {
    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle->Instance == USART2) {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
        GPIO_InitStruct.Pin       = USART_TX_Pin | USART_RX_Pin;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */

        /* USER CODE END USART2_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle) {
    if (uartHandle->Instance == USART2) {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
        HAL_GPIO_DeInit(GPIOA, USART_TX_Pin | USART_RX_Pin);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
volatile static bool _is_log_on = true;
volatile static char character  = 0;

void UART_disable_log(void) {
    char buf[50];
    sprintf(buf, "Logging disabled. Press %s to enable", M_NAME_TO_STR(CMD_TOGGLE_LOG));
    print_log(buf, LOG_HEADER);
    _is_log_on = false;
};

void UART_enable_log(void) {
    _is_log_on = true;
    print_log("Logging enabled", LOG_HEADER);
};
bool UART_is_log_on() {
    return _is_log_on;
};

void print_log(char *txt, UART_HeaderTypeDef header_type) {
    if (_is_log_on) {
        char *header;
        switch (header_type) {
            case NORM_HEADER:
                header = "\r\n+[" M_NAME_TO_STR(MCU_NAME) "]\r\n|+-> ";
                break;
            case LOG_HEADER:
                header = "\r\n+[" M_NAME_TO_STR(MCU_NAME) " - LOG]\r\n|+-> ";
                break;
            case CAN_HEADER:
                header = "\r\n+[" M_NAME_TO_STR(MCU_NAME) " - CAN]\r\n|+-> ";
                break;
            case CAN_ERR_HEADER:
                header = "\r\n+[" M_NAME_TO_STR(MCU_NAME) " - CAN: # ERROR #]\r\n|+-> ";
                break;
            case PCU_HEADER:
                header = "\r\n+[" M_NAME_TO_STR(MCU_NAME) " - PCU]\r\n|+-> ";
                break;
            case NO_HEADER:
            default:
                header = "\r\n|+-> ";
                break;
        }
        char buf[200];
        sprintf(buf, "%s", header);
        strncat(buf, txt, 200 - strlen(buf) - 1);
        HAL_UART_Transmit(&LOG_HUART, (uint8_t *)buf, strlen(buf), 500);
    }
}

#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
}
#endif

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
