/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "lwip.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "traffic_light.h"
#include "hardware/button.h"
#include "command.h"
#include "menu.h"
#include "eth/lwip_tcp.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_BUF_SIZE 128
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t uartBuf[UART_BUF_SIZE];
static uint8_t uartBufLast = 0;
static uint8_t hasLine = 0;
static Command *commands[5];

static uint32_t durations[3] = {
        [RED] = 6000,
        [YELLOW] = 1500,
        [GREEN] = 1500
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static char * getTrafficLightInfo();
static char * changeButtonMode(const uint32_t *intArg);
static char * changeInterruptionMode(const uint32_t *intArg);
static char * changeRedTimeout(const uint32_t *intArg);
static void onButtonClick();
static void sendMessage(const char * msg);
static void receiveAndSendChar();
static void sendCommandResult(const char * commandResult);
void jumpToBootloader();
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
  commands[0] = commandCreate("?", (CommandAction) getTrafficLightInfo, NONE);
  commands[1] = commandCreate("set mode", (CommandAction) changeButtonMode, INT);
  commands[2] = commandCreate("set interrupts", (CommandAction) changeInterruptionMode, INT);
  commands[3] = commandCreate("set timeout", (CommandAction) changeRedTimeout, INT);
  commands[4] = commandCreate("boot", (CommandAction) jumpToBootloader, NONE);
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
  MX_USART6_UART_Init();
  MX_TIM7_Init();
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */
  lwip_tcp_init();
  uartInit();

  trafficLightInit(LED_RED_GPIO_Port, LED_RED_Pin, LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, LED_GREEN_GPIO_Port, LED_GREEN_Pin);
  trafficLightSetDuration(RED, durations[RED]);
  trafficLightSetDuration(GREEN, durations[GREEN]);
  trafficLightSetDuration(YELLOW, durations[YELLOW]);
  trafficLightStart();

  buttonInit(BTN_N_GPIO_Port, BTN_N_Pin);
  buttonSetOnClick(&onButtonClick);
  buttonEnable();

  menuInit(commands, 5);
  sendMessage("Started\n");
  HAL_TIM_Base_Start_IT(&htim7);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)  {
	  WRITE_REG(IWDG->KR, 0x0000AAAAU);
	  MX_LWIP_Process();

	  trafficLightUpdate();
	  buttonUpdateState();
	  uartPooling();

	  if (uartHasNext()) {
		  receiveAndSendChar();
	  }

	  if (hasLine) {
		  uartBuf[uartBufLast] = '\0';
		  const char *commandResult = menuExecuteCommand((char *) uartBuf);
		  sendMessage("\n");
		  sendCommandResult(commandResult);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/* USER CODE BEGIN 4 */
void jumpToBootloader() {
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)0x08104000, 0);
	HAL_FLASH_Lock();
	NVIC_SystemReset();
}

static void onButtonClick() {
    trafficLightSetShortRed(durations[RED] >> 2);
}

static char cmdBuf[128];
static char * getTrafficLightInfo() {
    char *bufLast = cmdBuf;
    uint8_t isBlinking = trafficLightIsBlinking();
    char *currentColor = colorGetName(trafficLightGetColor());
    bufLast += sprintf(bufLast, "Light: %s%s\n",
            isBlinking ? "blinking " : "",
            currentColor
    );
    bufLast += sprintf(bufLast, "Button mode: mode %" PRIu32 " (%s)\n",
                       buttonIsEnabled() + 1,
                       buttonIsEnabled() ? "enabled" : "disabled"
    );
    bufLast += sprintf(bufLast, "Red timeout: %" PRIu32 " s\n",
                       durations[RED] / (uint32_t) 1000
    );
    bufLast += sprintf(bufLast, "Interrupts mode: %s\n\r",
                       uartIsInterruptionEnabled()  ? "I" : "P"
    );
    return cmdBuf;
}

static char * changeButtonMode(const uint32_t *intArg) {
    if (*intArg == 2) {
    	buttonEnable();
        return "Enable button\n";
    } else if (*intArg == 1) {
        buttonDisable();
        return "Disable button\n";
    } else {
        return "The int argument must be 1 or 2\n";
    }
}

static char * changeInterruptionMode(const uint32_t *intArg) {
    if (*intArg == 1) {
        uartEnableInterruption();
        return "Enable int\n";
    } else if (*intArg == 2) {
        uartDisableInterruption();
        return "Disable int\n";
    } else {
        return "The argument must be 1 or 2\n";
    }
}

static char * changeRedTimeout(const uint32_t *intArg) {
    durations[RED] = *intArg * 1000;
    trafficLightSetDuration(RED, durations[RED]);
    return "OK\n";
}

static void sendMessage(const char * msg) {
    uartTransmit((uint8_t *) msg, strlen(msg));
}

static void receiveAndSendChar() {
    if (uartReceive(uartBuf + uartBufLast, 1)) {
        uint8_t received = uartBuf[uartBufLast];
        uartBufLast++;
        uartTransmit(&received, 1);

        if (received == '\r')
            hasLine = 1;
    }
}

static void sendCommandResult(const char * commandResult) {
    uartTransmit((uint8_t *) commandResult, strlen(commandResult));
    uartBufLast = 0;
    hasLine = 0;
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
  while (1)
  {
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
