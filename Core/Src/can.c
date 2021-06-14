/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "inttypes.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

/**
 * @brief Print messages over the serial UART, prefixed with "[CAN]"
 * */
void CAN_serial_print(UART_HandleTypeDef* huart, char* txt) {
  char buf[100] = "[nucleo-f4-bench - CAN] ";
  strncat(buf, txt, 100 - strlen(buf) - 1);
  HAL_UART_Transmit(huart, (uint8_t*)buf, strlen(buf), 500);
}

/**
 * @brief Print the error message in the serial console and activate
 *        the CAN error LED
 * @param msg The message to send over UART
 * */
void CAN_error_handler(CAN_HandleTypeDef* hcan, char* msg) {
  CAN_serial_print(msg);

  uint32_t err_code = HAL_CAN_GetError(hcan);

  if (err_code != HAL_CAN_ERROR_NONE) CAN_serial_print("Error bits set:\r\n");
  if ((err_code & HAL_CAN_ERROR_EWG) == HAL_CAN_ERROR_EWG)
    CAN_serial_print("  + Protocol error warning\r\n");
  if ((err_code & HAL_CAN_ERROR_EPV) == HAL_CAN_ERROR_EPV)
    CAN_serial_print("  + Error passive\r\n");
  if ((err_code & HAL_CAN_ERROR_BOF) == HAL_CAN_ERROR_BOF)
    CAN_serial_print("  + Bus-off error\r\n");
  if ((err_code & HAL_CAN_ERROR_STF) == HAL_CAN_ERROR_STF)
    CAN_serial_print("  + Stuff error\r\n");
  if ((err_code & HAL_CAN_ERROR_FOR) == HAL_CAN_ERROR_FOR)
    CAN_serial_print("  + Form error\r\n");
  if ((err_code & HAL_CAN_ERROR_ACK) == HAL_CAN_ERROR_ACK)
    CAN_serial_print("  + ACK error\r\n");
  if ((err_code & HAL_CAN_ERROR_BR) == HAL_CAN_ERROR_BR)
    CAN_serial_print("  + Bit Recessive error\r\n");
  if ((err_code & HAL_CAN_ERROR_BD) == HAL_CAN_ERROR_BD)
    CAN_serial_print("  + Bit Dominant error\r\n");
  if ((err_code & HAL_CAN_ERROR_CRC) == HAL_CAN_ERROR_CRC)
    CAN_serial_print("  + CRC error\r\n");
  if ((err_code & HAL_CAN_ERROR_RX_FOV0) == HAL_CAN_ERROR_RX_FOV0)
    CAN_serial_print("  + FIFO 0 overrun error\r\n");
  if ((err_code & HAL_CAN_ERROR_RX_FOV1) == HAL_CAN_ERROR_RX_FOV1)
    CAN_serial_print("  + FIFO 1 overrun error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST0) == HAL_CAN_ERROR_TX_ALST0)
    CAN_serial_print("  + TX 0 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR0) == HAL_CAN_ERROR_TX_TERR0)
    CAN_serial_print("  + TX 0 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST1) == HAL_CAN_ERROR_TX_ALST1)
    CAN_serial_print("  + TX 1 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR1) == HAL_CAN_ERROR_TX_TERR1)
    CAN_serial_print("  + TX 1 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST2) == HAL_CAN_ERROR_TX_ALST2)
    CAN_serial_print("  + TX 2 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR2) == HAL_CAN_ERROR_TX_TERR2)
    CAN_serial_print("  + TX 2 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TIMEOUT) == HAL_CAN_ERROR_TIMEOUT)
    CAN_serial_print("  + Timeout error\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_INITIALIZED) ==
      HAL_CAN_ERROR_NOT_INITIALIZED)
    CAN_serial_print("  + CAN bus not initialized\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_READY) == HAL_CAN_ERROR_NOT_READY)
    CAN_serial_print("  + CAN bus not ready\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_STARTED) == HAL_CAN_ERROR_NOT_STARTED)
    CAN_serial_print("  + CAN bus not started\r\n");
  if ((err_code & HAL_CAN_ERROR_PARAM) == HAL_CAN_ERROR_PARAM)
    CAN_serial_print("  + Parameter error\r\n");
  if ((err_code & HAL_CAN_ERROR_INTERNAL) == HAL_CAN_ERROR_INTERNAL)
    CAN_serial_print("  + Internal error\r\n");

  uint8_t rec_val = 0;
  uint8_t tec_val = 0;
  rec_val = (uint8_t)((hcan->Instance->ESR >> 24) && 0xFFU);
  tec_val = (uint8_t)((hcan->Instance->ESR >> 16) && 0xFFU);
  char counters_status[40];
  sprintf(counters_status, "Value of REC %d, Value of TEC %d \r\n", rec_val,
          tec_val);
  CAN_serial_print(counters_status);
}
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void) {
  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (canHandle->Instance == CAN1) {
    /* USER CODE BEGIN CAN1_MspInit 0 */

    /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
    /* USER CODE BEGIN CAN1_MspInit 1 */

    /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle) {
  if (canHandle->Instance == CAN1) {
    /* USER CODE BEGIN CAN1_MspDeInit 0 */

    /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
    /* USER CODE BEGIN CAN1_MspDeInit 1 */

    /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
