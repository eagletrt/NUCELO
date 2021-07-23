/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdbool.h"

/* USER CODE BEGIN Includes */
typedef enum { CAN_NET_PRIM = 0U, CAN_NET_SEC, CAN_NET_BMS, CAN_NO_NET, NUM_CAN_NET } CAN_NetTypeDef;
typedef enum { CAN_BITRATE_1MBIT, CAN_BITRATE_125KBIT } CAN_Bitrate;
static char *CAN_NET_NAMES_G[NUM_CAN_NET] = {"CAN_NET_PRIM", "CAN_NET_SEC", "CAN_NET_BMS", "CAN_NO_NET"};

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_CAN1_Init(void);

/* USER CODE BEGIN Prototypes */
/**
 * @brief  Send a can message with specified payload.
 *         WARNING:  Blocking function 
 * 
 * @param  hcan can handle of type @ref CAN_HancldeTypeDef
 * @param  msg_name Can message name  
 * @param  payload The payaload of the message. WARNING: array of at least 8 data
 * @param  pay_size The size (number of uint8_t ) uesed by payload
 *  
 * @return Send result, values of type @ref HAL_StatusTypeDef 
 */
HAL_StatusTypeDef CAN_send_payload(
    CAN_HandleTypeDef *hcan,
    uint16_t msg_name,
    uint8_t payload[static 8],
    uint8_t pay_size);

/**
 * @brief     Send a can message, all data is infered from the message name.
 *            WARNING:  Blocking function 
 * 
 * @param     hcan can handle of type @ref CAN_HancldeTypeDef
 * @param     msg_name Can message name  
 * @return    Send result, values of type @ref HAL_StatusTypeDef 
 */
HAL_StatusTypeDef CAN_send(CAN_HandleTypeDef *hcan, uint16_t msg_name);
/**
 * @brief     GET data from a can payload 
 * 
 * @param     hcan CAN_HandleTypeDef pointer
 * @param     rxheader Pointer to the received message rxheader
 * @param     data pointer to the received message payload
 */
void CAN_get(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *rxheader, uint8_t data[static 8]);

/**
 * @brief     Change the way ids are interpreted by changing the network they 
 *            belong to.
 * 
 * @param     net The network name to change into, a value form @ref CAN_NetTypeDef 
 */
void CAN_set_network(CAN_NetTypeDef net);
/**
 * @brief     Get the current network on wich the reader is encoding messages
 * 
 * @return    Active netowrk, can be a value of @ref CAN_NetTypeDef
 */
CAN_NetTypeDef CAN_get_network();

/**
 * @brief  Bitrate of the can peripheral
 * 
 * @param hcan Can handle pointer
 * @param bitrate @arg of CAN_Bitrate
 */
void CAN_change_bitrate(CAN_HandleTypeDef *hcan, CAN_Bitrate bitrate);

/**
 * @brief  Get current bitrate for can handle
 * 
 * @param hcan Can peripheral handle
 * 
 * @return CAN_Bitrate 
 */
CAN_Bitrate CAN_GetCurrentBitrate(CAN_HandleTypeDef *hcan);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
