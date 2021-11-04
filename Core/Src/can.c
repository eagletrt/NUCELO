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
#include "../Lib/can-cicd/includes_generator/Primary/ids.h"
#include "../Lib/can-cicd/includes_generator/Primary/utils.h"
#include "../Lib/can-cicd/includes_generator/Secondary/ids.h"
#include "../Lib/can-cicd/includes_generator/Secondary/utils.h"
#include "../Lib/can-cicd/includes_generator/bms/ids.h"
#include "../Lib/can-cicd/includes_generator/bms/utils.h"
#include "../Lib/can-cicd/naked_generator/Primary/c/Primary.h"
#include "../Lib/can-cicd/naked_generator/Secondary/c/Secondary.h"
#include "../Lib/can-cicd/naked_generator/bms/c/bms.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

#define CAN_1MBIT_PRE 3
#define CAN_1MBIT_BS1 CAN_BS1_11TQ
#define CAN_1MBIT_BS2 CAN_BS2_2TQ

#define CAN_125KBIT_PRE 42
#define CAN_125KBIT_BS1 CAN_BS1_13TQ
#define CAN_125KBIT_BS2 CAN_BS2_2TQ

static void _can_error_handler(CAN_HandleTypeDef *hcan, char *msg);
static void _can_apply_filter(CAN_HandleTypeDef *hcan);
static void _return_id_string_name(uint16_t id, char buf[static 50]);

static CAN_NetTypeDef active_net  = CAN_NET_PRIM;
static CAN_Bitrate active_bitrate = CAN_BITRATE_1MBIT;

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
    sprintf(buf, "Code: %lu", (uint32_t)hcan->ErrorCode);
    _can_error_handler(hcan, buf);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    print_log("FUCKING RX1", CAN_HEADER);
    CAN_RxHeaderTypeDef h;
    uint8_t data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &h, data);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    print_log("FUCKING RX2", CAN_HEADER);
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
    uint16_t msg_name,
    uint8_t payload[static 8],
    uint8_t pay_size) {
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        _can_error_handler(hcan, "No free mailboxes available\r\n");
        return HAL_ERROR;
    }

    CAN_TxHeaderTypeDef header;
    HAL_StatusTypeDef status;

    uint32_t free_mailbox;

    header.StdId              = msg_name;
    header.IDE                = CAN_ID_STD;
    header.RTR                = CAN_RTR_DATA;
    header.TransmitGlobalTime = DISABLE;
    header.DLC                = pay_size;
    /* Send the message */
    status = HAL_CAN_AddTxMessage(hcan, &header, payload, &free_mailbox);
    if (status != HAL_OK)
        _can_error_handler(hcan, "AddTxMessage failed");

    print_log("CAN_send", CAN_HEADER);
    return status;
}

HAL_StatusTypeDef CAN_send(CAN_HandleTypeDef *hcan, uint16_t msg_name) {
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        _can_error_handler(hcan, "No free mailboxes available\r\n");
        return HAL_ERROR;
    }

    CAN_TxHeaderTypeDef header;
    HAL_StatusTypeDef status;

    uint8_t data[8] = {};
    uint32_t free_mailbox;

    header.StdId              = msg_name;
    header.DLC                = 1;
    header.IDE                = CAN_ID_STD;
    header.RTR                = CAN_RTR_DATA;
    header.TransmitGlobalTime = DISABLE;
    /* Send the message */
    status = HAL_CAN_AddTxMessage(hcan, &header, data, &free_mailbox);
    if (status != HAL_OK)
        _can_error_handler(hcan, "AddTxMessage failed");

    print_log("CAN_send", CAN_HEADER);
    return status;
}

void CAN_get(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *rxheader, uint8_t data[static 8]) {
    char buf[100]    = {};
    char id_name[50] = {};
    _return_id_string_name(rxheader->StdId, id_name);
    sprintf(buf, "ID: x%03X, MSG_NAME: %s", (uint16_t)rxheader->StdId, id_name);
    print_log(buf, CAN_HEADER);
    sprintf(
        buf,
        "data: x%02X x%02X x%02X x%02X, DLC: %lu, RTR: %lu",
        data[1] << 8 | data[0],
        data[3] << 8 | data[2],
        data[5] << 8 | data[4],
        data[7] << 8 | data[6],
        rxheader->DLC,
        rxheader->RTR);
    print_log(buf, NO_HEADER);
}
void CAN_set_network(CAN_NetTypeDef net) {
    if (net >= 0U || net < NUM_CAN_NET) {
        active_net = net;
    }
}
CAN_NetTypeDef CAN_get_network() {
    return active_net;
}

void CAN_change_bitrate(CAN_HandleTypeDef *hcan, CAN_Bitrate bitrate) {
    if (bitrate == active_bitrate) {
        print_log("Bitrate changed", CAN_HEADER);
    }
    /* De initialize CAN*/
    HAL_CAN_DeInit(hcan);
    switch (bitrate) {
        case CAN_BITRATE_1MBIT:
            hcan->Init.Prescaler = CAN_1MBIT_PRE;
            hcan->Init.TimeSeg1  = CAN_1MBIT_BS1;
            hcan->Init.TimeSeg2  = CAN_1MBIT_BS2;
            break;
        case CAN_BITRATE_125KBIT:
            hcan->Init.Prescaler = CAN_125KBIT_PRE;
            hcan->Init.TimeSeg1  = CAN_125KBIT_BS1;
            hcan->Init.TimeSeg2  = CAN_125KBIT_BS2;
            break;
    }
    active_bitrate = bitrate;
    if (HAL_CAN_Init(hcan) != HAL_OK) {
        print_log("Initialization error going in Error_Handler", CAN_ERR_HEADER);
        Error_Handler();
    }
    _can_apply_filter(hcan);

    char buf[50];
    sprintf(buf, "Bitrate changed to %s", bitrate == CAN_BITRATE_1MBIT ? "1MBit" : "125KBit");
    print_log(buf, CAN_HEADER);
}
CAN_Bitrate CAN_GetCurrentBitrate(CAN_HandleTypeDef *hcan) {
    return active_bitrate;
};

static void _return_id_string_name(uint16_t id, char buf[static 50]) {
    switch (active_net) {
        case CAN_NET_PRIM:
            Primary_msgname_from_id(id, buf);
            break;
        case CAN_NET_SEC:
            Secondary_msgname_from_id(id, buf);
            break;
        case CAN_NET_BMS:
            bms_msgname_from_id(id, buf);
            break;
        case CAN_NO_NET:
        default:
            buf[0] = '\0';
            break;
    }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
