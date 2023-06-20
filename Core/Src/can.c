/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "../Lib/can/lib/primary/primary_network.h"
#include "../Lib/can/lib/secondary/secondary_network.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"


static void _can_error_handler(CAN_HandleTypeDef *hcan, char *msg);
static void _can_apply_filter(CAN_HandleTypeDef *hcan);

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

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
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
    _can_apply_filter(&hcan1);
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
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

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

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
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    char buf[50];
    sprintf(buf, "[CAN ERROR] Code: %lu", (uint32_t)hcan->ErrorCode);
    _can_error_handler(hcan, buf);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef h;
    uint8_t data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &h, data);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef h;
    uint8_t data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &h, data);
}
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {
    _can_error_handler(hcan, "RX FIFO#0 is full");
}
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {
    _can_error_handler(hcan, "RX FIFO#1 is full");
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    print_log("Mailbox 0 complete", CAN_HEADER);
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    print_log("Mailbox 1 complete", CAN_HEADER);
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    print_log("Mailbox 2 complete", CAN_HEADER);
}
/**
 * @brief Print the error message in the serial console and activate
 *        the CAN error LED
 * @param msg The message to send over UART
 * */
static void _can_error_handler(CAN_HandleTypeDef *hcan, char *msg) {
    print_log(msg, CAN_ERR_HEADER);

    uint32_t err_code = HAL_CAN_GetError(hcan);

    if ((err_code & HAL_CAN_ERROR_EWG) == HAL_CAN_ERROR_EWG)
        print_log("Protocol error warning", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_EPV) == HAL_CAN_ERROR_EPV)
        print_log("Error passive", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_BOF) == HAL_CAN_ERROR_BOF)
        print_log("Bus-off error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_STF) == HAL_CAN_ERROR_STF)
        print_log("Stuff error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_FOR) == HAL_CAN_ERROR_FOR)
        print_log("Form error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_ACK) == HAL_CAN_ERROR_ACK)
        print_log("ACK error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_BR) == HAL_CAN_ERROR_BR)
        print_log("Bit Recessive error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_BD) == HAL_CAN_ERROR_BD)
        print_log("Bit Dominant error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_CRC) == HAL_CAN_ERROR_CRC)
        print_log("CRC error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_RX_FOV0) == HAL_CAN_ERROR_RX_FOV0)
        print_log("FIFO 0 overrun error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_RX_FOV1) == HAL_CAN_ERROR_RX_FOV1)
        print_log("FIFO 1 overrun error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_ALST0) == HAL_CAN_ERROR_TX_ALST0)
        print_log("TX 0 arbitration lost error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_TERR0) == HAL_CAN_ERROR_TX_TERR0)
        print_log("TX 0 transmit error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_ALST1) == HAL_CAN_ERROR_TX_ALST1)
        print_log("TX 1 arbitration lost error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_TERR1) == HAL_CAN_ERROR_TX_TERR1)
        print_log("TX 1 transmit error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_ALST2) == HAL_CAN_ERROR_TX_ALST2)
        print_log("TX 2 arbitration lost error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TX_TERR2) == HAL_CAN_ERROR_TX_TERR2)
        print_log("TX 2 transmit error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_TIMEOUT) == HAL_CAN_ERROR_TIMEOUT)
        print_log("Timeout error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_NOT_INITIALIZED) == HAL_CAN_ERROR_NOT_INITIALIZED)
        print_log("CAN bus not initialized", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_NOT_READY) == HAL_CAN_ERROR_NOT_READY)
        print_log("CAN bus not ready", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_NOT_STARTED) == HAL_CAN_ERROR_NOT_STARTED)
        print_log("CAN bus not started", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_PARAM) == HAL_CAN_ERROR_PARAM)
        print_log("Parameter error", NO_HEADER);
    if ((err_code & HAL_CAN_ERROR_INTERNAL) == HAL_CAN_ERROR_INTERNAL)
        print_log("Internal error", NO_HEADER);

    char buf[50];

    uint16_t rec_val = (uint16_t)((hcan->Instance->ESR && CAN_ESR_REC_Msk) >> CAN_ESR_REC_Pos);
    if (rec_val > 0) {
        sprintf(buf, "REC (Receive Error Counter) %d", rec_val);
        print_log(buf, NO_HEADER);
    }

    uint16_t tec_val = (uint16_t)((hcan->Instance->ESR && CAN_ESR_TEC_Msk) >> CAN_ESR_TEC_Pos);
    if (tec_val > 0) {
        sprintf(buf, "TEC (Transmit Error Counter) %d", tec_val);
        print_log(buf, NO_HEADER);
    }
}

HAL_StatusTypeDef CAN_send_payload(
        CAN_HandleTypeDef *hcan,
        uint16_t msg_id,
        uint8_t payload[static 8],
        uint8_t pay_size) {

    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        _can_error_handler(hcan, "No free mailboxes available\r\n");
        return HAL_ERROR;
    }

    CAN_TxHeaderTypeDef header;
    HAL_StatusTypeDef status;

    uint32_t free_mailbox;

    header.StdId              = msg_id;
    header.IDE                = CAN_ID_STD;
    header.RTR                = CAN_RTR_DATA;
    header.TransmitGlobalTime = DISABLE;
    header.DLC                = pay_size;

    /* Send the message */
    status = HAL_CAN_AddTxMessage(hcan, &header, payload, &free_mailbox);

    if (status != HAL_OK)
        _can_error_handler(hcan, "AddTxMessage failed");

    print_log("CAN_send", CAN_HEADER);

    char buf[100];
    uint8_t buff_off = 0;
    buff_off += snprintf(buf, 100, "0x%02X | ", msg_id);

    for (uint8_t i = 0; i < pay_size; i++)
        snprintf(buf + buff_off, 100 - buff_off, "0x%02X "  , payload[i]);

    print_log(buf, NORM_HEADER);

    return status;
}


static void _can_apply_filter(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef filter;
    filter.FilterMode       = CAN_FILTERMODE_IDMASK;
    filter.FilterIdLow      = 0 << 5;                 // Take all ids from 0
    filter.FilterIdHigh     = ((1U << 11) - 1) << 5;  // to 2^11 - 1
    filter.FilterMaskIdHigh = 0x00 << 5;              // Don't care on can id bits
    filter.FilterMaskIdLow  = 0x00 << 5;              // Don't care on can id bits
    /* HAL considers IdLow and IdHigh not as just the ID of the can message but
        as the combination of: 
        STDID + RTR + IDE + 4 most significant bits of EXTID
    */
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterBank           = 0;
    filter.FilterScale          = CAN_FILTERSCALE_16BIT;
    filter.FilterActivation     = ENABLE;

    if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK)
        _can_error_handler(hcan, "Failed to configure CAN filter");
}
/* USER CODE END 1 */
