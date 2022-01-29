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
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "common.h"
#include "stdio.h"
#include "../Lib/can-cicd/includes_generator/Primary/ids.h"
#include "../Lib/can-cicd/naked_generator/Primary/c/Primary.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    CMD_NONE                = 0U,
    CMD_ROTATE_ACTIVE_NET   = 'n',
    CMD_TOGGLE_CAN_OUTPUT   = 'c',
    CMD_TOGGLE_LOG          = 'l',
    CMD_CAN_SEND_BOOT_SYNC  = 'b',
    CMD_CAN_SEND_BOOT_GETID = 'g',
    CMD_INV_ACT_NOTI        = 'i',
    CMD_INV_DISABLE         = '1',
    CMD_INV_ENABLE          = '2',
    CMD_TSON_REQ            = '3',
    CMD_TSOFF_REQ           = '4',
    CMD_TSPRECHRG           = 'p',
    CMD_TSON_CONF           = '5',
    CMD_TSOFF_CONF          = '6',
    CMD_INVON_CONF          = '7',
    CMD_INVOFF_CONF         = '8',
    CMD_PCU_IMP_ERR         = '9',
    CMD_PCU_PLUS            = '+',
    CMD_PCU_MINUS           = '-',
    CMD_TOGGLE_CAN_BITRATE  = '@',
    CMD_GET_CAN_BITRATE     = '#',
    CMD_NEWLINE             = ' ',
    CMD_HELP                = '?'
} CMD_TypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CAN_MSG_BOOT_SYNC  0x79U
#define CAN_MSG_BOOT_GETID 0x02U

#define CAN_INV_NOTI_EN_PAYLOAD  ((uint8_t[]){0x3dU, 0x51U, 0x64U, 0x00, 0x00, 0x00, 0x00, 0x00})
#define CAN_INV_NOTI_EN_PAY_SIZE 3

#define CAN_INV_DISABLE_PAYLOAD  ((uint8_t[]){0x51U, 0x04U, 0x00U, 0x00, 0x00, 0x00, 0x00, 0x00})
#define CAN_INV_DISABLE_PAY_SIZE 3

#define CAN_INV_ENABLE_PAYLOAD  ((uint8_t[]){0x51U, 0x08U, 0x00U, 0x00, 0x00, 0x00, 0x00, 0x00})
#define CAN_INV_ENABLE_PAY_SIZE 3

#define CAN_INVON_CONF_PAYLOAD  ((uint8_t[]){0x40, 0x01, 0x00, 0x00, 0x00})
#define CAN_INVON_CONF_PAY_SIZE 5

#define CAN_INVOFF_CONF_PAYLOAD  ((uint8_t[]){0x40, 0x00, 0x00, 0x00, 0x00})
#define CAN_INVOFF_CONF_PAY_SIZE 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t PCU_accel_val = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void _compute_command(char character);
static void _cmd_help();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_CAN1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    for (uint8_t i = 0; i < 3; i++) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(200);
    }
    print_log("Initialization success", NORM_HEADER);

#if 0
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
        print_log("Failed to activate a CAN RX interrupts",CAN_ERR_HEADER);
#endif
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_OVERRUN | CAN_IT_RX_FIFO1_OVERRUN) != HAL_OK)
        print_log("Failed to activate CAN_IT_RX_FIFOx_OVERRUN interrupt", CAN_ERR_HEADER);

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR_WARNING) != HAL_OK)
        print_log("Failed to activate CAN_IT_ERROR_WARNING interrupt", CAN_ERR_HEADER);

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR_PASSIVE) != HAL_OK)
        print_log("Failed to activate CAN_IT_ERROR_PASSIVE interrupt", CAN_ERR_HEADER);

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_BUSOFF) != HAL_OK)
        print_log("Failed to activate CAN_IT_BUSOFF interrupt", CAN_ERR_HEADER);

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_LAST_ERROR_CODE) != HAL_OK)
        print_log("Failed to activate CAN_IT_LAST_ERROR_CODE interrupt", CAN_ERR_HEADER);

    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR) != HAL_OK)
        print_log("Failed to activate CAN_IT_ERROR interrupt", CAN_ERR_HEADER);

    /* Start CAN peripheral */
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
        print_log("CAN_Start failed", CAN_ERR_HEADER);

    print_log("CAN started", CAN_HEADER);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    volatile char character = 0;
    HAL_UART_Receive_IT(&LOG_HUART, (uint8_t *)&character, sizeof(character));
    while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
            uint8_t data[8]              = {};
            CAN_RxHeaderTypeDef rxheader = {};
            if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxheader, data) == HAL_OK) {
                CAN_get(&hcan1, &rxheader, data);
                HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            } else {
                print_log("CAN GetRxMessage failed", CAN_ERR_HEADER);
            }
        }

        if (character != 0U) {
            char buf[2];
            sprintf(buf, "User input: %c", character);
            print_log(buf, NO_HEADER);
            
            _compute_command(character);
            character = 0U;
            HAL_UART_Receive_IT(&LOG_HUART, (uint8_t *)&character, sizeof(character));
        };
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// 0x201 0x3d, 0x51,0x64
void _compute_command(char character) {
    char buf[40];
    uint8_t msg_data[8];
    size_t msg_dlc;

    switch (character) {
        case CMD_NONE:
            break;
        case CMD_ROTATE_ACTIVE_NET:
            CAN_set_network((CAN_NetTypeDef)((CAN_get_network() + 1U) % NUM_CAN_NET));
            sprintf(buf, "Listening to %s", CAN_NET_NAMES_G[CAN_get_network()]);
            print_log(buf, CAN_HEADER);
            break;
        case CMD_TOGGLE_LOG:
            UART_is_log_on() ? UART_disable_log() : UART_enable_log();
            break;
        case CMD_CAN_SEND_BOOT_SYNC:
            CAN_send(&hcan1, CAN_MSG_BOOT_SYNC);
            break;
        case CMD_CAN_SEND_BOOT_GETID:
            CAN_send(&hcan1, CAN_MSG_BOOT_GETID);
            break;
        case CMD_INV_ACT_NOTI:
            CAN_send_payload(&hcan1, 0x201, CAN_INV_NOTI_EN_PAYLOAD, CAN_INV_NOTI_EN_PAY_SIZE);
            break;
        case CMD_INV_DISABLE:
            CAN_send_payload(&hcan1, 0x201, CAN_INV_DISABLE_PAYLOAD, CAN_INV_DISABLE_PAY_SIZE);
            break;
        case CMD_INV_ENABLE:
            CAN_send_payload(&hcan1, 0x201, CAN_INV_ENABLE_PAYLOAD, CAN_INV_ENABLE_PAY_SIZE);
            break;

        case CMD_TSON_REQ:
            msg_dlc = serialize_Primary_SET_CAR_STATUS(msg_data, Primary_Car_Status_Set_RUN);
            CAN_send_payload(&hcan1, ID_SET_CAR_STATUS, msg_data, msg_dlc);
            break;
        case CMD_TSOFF_REQ:
            msg_dlc = serialize_Primary_SET_CAR_STATUS(msg_data, Primary_Car_Status_Set_IDLE);
            CAN_send_payload(&hcan1, ID_SET_CAR_STATUS, msg_data, msg_dlc);
            break;
        case CMD_TSON_CONF:
            msg_dlc = serialize_Primary_TS_STATUS(msg_data, Primary_Ts_Status_ON);
            CAN_send_payload(&hcan1, ID_TS_STATUS, msg_data, msg_dlc);
            break;
        case CMD_TSPRECHRG:
            msg_dlc = serialize_Primary_TS_STATUS(msg_data, Primary_Ts_Status_PRECHARGE);
            CAN_send_payload(&hcan1, ID_TS_STATUS, msg_data, msg_dlc);
            break;
        case CMD_TSOFF_CONF:
            msg_dlc = serialize_Primary_TS_STATUS(msg_data, Primary_Ts_Status_OFF);
            CAN_send_payload(&hcan1, ID_TS_STATUS, msg_data, msg_dlc);
            break;
        case CMD_INVON_CONF:
            CAN_send_payload(&hcan1, 0x181, CAN_INVON_CONF_PAYLOAD, CAN_INVON_CONF_PAY_SIZE);
            break;
        case CMD_INVOFF_CONF:
            CAN_send_payload(&hcan1, 0x181, CAN_INVOFF_CONF_PAYLOAD, CAN_INVOFF_CONF_PAY_SIZE);
            break;
        case CMD_PCU_IMP_ERR:
            print_log("Setting PCU IMPLAUSIBILITY error flag", PCU_HEADER);
            
            Primary_pcu_flags w, e;
            setBit(e, Primary_pcu_flags_IMPLAUSIBILITY, 1);
            msg_dlc = serialize_Primary_PCU_STATUS(msg_data, w, e);
            CAN_send_payload(&hcan1, ID_PCU_STATUS, msg_data, msg_dlc);
            break;
        case CMD_PCU_PLUS:
            if (PCU_accel_val <= 250)
                PCU_accel_val += 5;
            sprintf(buf, "Sending PCU accelerator value: %d/255", PCU_accel_val);
            print_log(buf, PCU_HEADER);

            msg_dlc = serialize_Primary_ACCELERATOR_PEDAL_VAL(msg_data, PCU_accel_val);
            CAN_send_payload(&hcan1, ID_ACCELERATOR_PEDAL_VAL, msg_data, msg_dlc);
            break;
        case CMD_PCU_MINUS:
            if (PCU_accel_val >= 5)
                PCU_accel_val -= 5;
            sprintf(buf, "Sending PCU accelerator value: %d/255", PCU_accel_val);
            print_log(buf, PCU_HEADER);

            msg_dlc = serialize_Primary_ACCELERATOR_PEDAL_VAL(msg_data, PCU_accel_val);
            CAN_send_payload(&hcan1, ID_ACCELERATOR_PEDAL_VAL, msg_data, msg_dlc);
            break;

        case CMD_TOGGLE_CAN_BITRATE:
            CAN_GetCurrentBitrate(&hcan1) == CAN_BITRATE_1MBIT ? CAN_change_bitrate(&hcan1, CAN_BITRATE_125KBIT)
                                                               : CAN_change_bitrate(&hcan1, CAN_BITRATE_1MBIT);
            break;
        case CMD_GET_CAN_BITRATE:
            sprintf(
                buf, "Current bitrate = %s", CAN_GetCurrentBitrate(&hcan1) == CAN_BITRATE_1MBIT ? "1MBit" : "125KBit");
            print_log(buf, CAN_HEADER);
            break;
        case CMD_NEWLINE:
            print_log("", NO_HEADER);
            break;
        case CMD_HELP:
            _cmd_help();
            break;
        default:
            break;
    }
}
static void _cmd_help() {
    char buf[100];
    print_log("Help", NORM_HEADER);
    sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_NONE), CMD_NONE);
#define make_help_msg(_cmd_name_)                                      \
    do {                                                               \
        print_log(buf, NO_HEADER);                                     \
        sprintf(buf, "%s: %c", M_NAME_TO_STR(_cmd_name_), _cmd_name_); \
    } while (0)

    make_help_msg(CMD_ROTATE_ACTIVE_NET);
    make_help_msg(CMD_TOGGLE_CAN_OUTPUT);
    make_help_msg(CMD_TOGGLE_LOG);
    make_help_msg(CMD_CAN_SEND_BOOT_SYNC);
    make_help_msg(CMD_CAN_SEND_BOOT_GETID);
    make_help_msg(CMD_INV_ACT_NOTI);
    make_help_msg(CMD_INV_DISABLE);
    make_help_msg(CMD_INV_ENABLE);
    make_help_msg(CMD_TSON_REQ);
    make_help_msg(CMD_TSOFF_REQ);
    make_help_msg(CMD_TSON_CONF);
    make_help_msg(CMD_TSOFF_CONF);
    make_help_msg(CMD_INVON_CONF);
    make_help_msg(CMD_INVOFF_CONF);
    make_help_msg(CMD_PCU_IMP_ERR);
    make_help_msg(CMD_PCU_PLUS);
    make_help_msg(CMD_PCU_MINUS);
    make_help_msg(CMD_TOGGLE_CAN_BITRATE);
    make_help_msg(CMD_GET_CAN_BITRATE);
    make_help_msg(CMD_NEWLINE);
    make_help_msg(CMD_HELP);

#undef make_help_msg

    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_ROTATE_ACTIVE_NET), CMD_ROTATE_ACTIVE_NET);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_TOGGLE_CAN_OUTPUT), CMD_TOGGLE_CAN_OUTPUT);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_TOGGLE_LOG), CMD_TOGGLE_LOG);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_CAN_SEND_BOOT_SYNC), CMD_CAN_SEND_BOOT_SYNC);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_CAN_SEND_BOOT_GETID), CMD_CAN_SEND_BOOT_GETID);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_INV_ACT_NOTI), CMD_INV_ACT_NOTI);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_INV_DISABLE), CMD_INV_DISABLE);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_INV_ENABLE), CMD_INV_ENABLE);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_NEWLINE), CMD_NEWLINE);
    // print_log(buf, NO_HEADER);
    // sprintf(buf, "%s: %c", M_NAME_TO_STR(CMD_HELP), CMD_HELP);
    // print_log(buf, NO_HEADER);
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
    while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
