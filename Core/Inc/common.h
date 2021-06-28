/**
 * @file      common.h
 * @author    Simone Ruffini [simone.ruffini@tutanota.com]
 * @date      2021-06-24
 * @updated
 * @ingroup
 * @prefix
 * 
 * @brief     
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H_
#define _COMMON_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MCU_NAME NUCLEO_F4_BENCH

/* Exported macros -----------------------------------------------------------*/

#define M_NAME_TO_STR(X) _STR_(X)
/* Exported functions --------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private Macros -----------------------------------------------------------*/
#define _STR_(X) #X

#endif