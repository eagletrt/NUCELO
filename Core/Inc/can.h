/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
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

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

