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
#include "../Lib/can/lib/primary/primary_network.h"
#include "../Lib/can/lib/secondary/secondary_network.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define CMD_NONE 0U
#define CMD_NEWLINE '\n'
#define CMD_HELP 'h'
#define CMD_SET_TS_ON 'T'
#define CMD_SET_TS_OFF 't'
#define CMD_SET_CAR_IDLE 'c'
#define CMD_SET_CAR_READY 'C'
#define CMD_SET_CAR_DRIVE 'D'

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void _eval_command(char character);
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
                HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            } else {
                print_log("CAN GetRxMessage failed", CAN_ERR_HEADER);
            }
        }

        if (character != 0U) {
            if (character == CMD_NEWLINE)
                print_log("", NO_HEADER);
            else {
                // char buf[20] = { 0U };
                // sprintf(buf, "User input: %c", character);
                // print_log(buf, NO_HEADER);
                
                _eval_command(character);
                character = 0U;
            }

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
void _eval_command(char character) {
    const uint8_t CAN_BUF_LEN = 8;
    uint8_t msg_data[CAN_BUF_LEN];
    size_t msg_dlc;

    switch (character) {
        case CMD_NONE:
            break;
        case CMD_HELP:
            _cmd_help();
            break;
        case CMD_SET_TS_ON:
            print_log("Setting TS ON", NO_HEADER);
            primary_set_ts_status_das_t set_ts_on = { .ts_status_set = primary_set_ts_status_das_ts_status_set_ON };
            msg_dlc = primary_set_ts_status_das_pack(msg_data, &set_ts_on, CAN_BUF_LEN);
            CAN_send_payload(&hcan1, PRIMARY_SET_TS_STATUS_DAS_FRAME_ID, msg_data, msg_dlc);
            break;
        case CMD_SET_TS_OFF:
            primary_set_ts_status_das_t set_ts_off = { .ts_status_set = primary_set_ts_status_das_ts_status_set_OFF };
            msg_dlc = primary_set_ts_status_das_pack(msg_data, &set_ts_off, CAN_BUF_LEN);
            CAN_send_payload(&hcan1, PRIMARY_SET_TS_STATUS_DAS_FRAME_ID, msg_data, msg_dlc);
            break;
        case CMD_SET_CAR_IDLE:
            primary_set_car_status_t set_car_idle = { .car_status_set = primary_set_car_status_car_status_set_IDLE };
            msg_dlc = primary_set_car_status_pack(msg_data, &set_car_idle, CAN_BUF_LEN);
            CAN_send_payload(&hcan1, PRIMARY_SET_CAR_STATUS_FRAME_ID, msg_data, msg_dlc);
            break;
        case CMD_SET_CAR_READY:
            primary_set_car_status_t set_car_ready = { .car_status_set = primary_set_car_status_car_status_set_READY };
            msg_dlc = primary_set_car_status_pack(msg_data, &set_car_ready, CAN_BUF_LEN);
            CAN_send_payload(&hcan1, PRIMARY_SET_CAR_STATUS_FRAME_ID, msg_data, msg_dlc);
            break;
        case CMD_SET_CAR_DRIVE:
            primary_set_car_status_t set_car_drive = { .car_status_set = primary_set_car_status_car_status_set_DRIVE };
            msg_dlc = primary_set_car_status_pack(msg_data, &set_car_drive, CAN_BUF_LEN);
            CAN_send_payload(&hcan1, PRIMARY_SET_CAR_STATUS_FRAME_ID, msg_data, msg_dlc);
            break;
        default:
            break;
    }
}

static void _cmd_help() {
    print_log("\r\n+----|    H e l p    |---------------------------", NO_HEADER);

#define print_help_msg(cmd_char, cmd_desc)             \
    do {                                               \
        char buf[100];                                 \
        sprintf(buf, "| %c : %s", cmd_char, cmd_desc); \
        print_log(buf, NO_HEADER);                     \
    } while (0)

    print_help_msg(CMD_HELP, "Print help message");
    print_help_msg(CMD_SET_TS_ON, "Send: SetTsStatus = ON");
    print_help_msg(CMD_SET_TS_OFF, "Send: SetTsStatus = OFF");
    print_help_msg(CMD_SET_CAR_IDLE, "Send: SetCarStatus = IDLE");
    print_help_msg(CMD_SET_CAR_READY, "Send: SetCarStatus = READY");
    print_help_msg(CMD_SET_CAR_DRIVE, "Send: SetCarStatus = DRIVE");
#undef make_help_msg

    print_log("+------------------------------------------------\r\n", NO_HEADER);
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
