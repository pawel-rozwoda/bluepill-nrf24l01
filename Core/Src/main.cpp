//#define TRANSMITTER
#define RECEIVER
#define USE_CO2

#include "main.h"
#include "message.h"
#include "dht.h"
#include <nrf24l01.h>
#include <stdio.h>
#include <lcd_i2c.h>
#include "utils.h"
#include "mhz19.h"

#ifdef RECEIVER
uint8_t RxAddress[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
#endif

#ifdef TRANSMITTER
uint8_t TxAddress[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
#endif

I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

DataPacket g_TransferValues;

Dht g_DhtHandler;
Nrf24 g_NrfHandler;

#ifdef RECEIVER
LcdI2c g_LcdHandler;
PresenceMonitor g_PresenceMonitor;
MeasuredValues g_LocalMeasurements = {SourceName::INSIDE, (float)-200., (float)-200., MY_UINT16_MAX};
volatile bool g_remoteCall = false;
volatile bool g_localCall = false;
#endif

#ifdef USE_CO2
Mhz19 g_Mhz19Handler;
#endif

uint8_t g_nrfConfigData[50];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
void measure(MeasuredValues &measured_values);
void print_to_lcd(LcdI2c& lcdHandler, uint8_t line, MeasuredValues &measured_values);

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);

  g_NrfHandler.Nrf24Init(hspi1);
  g_DhtHandler.Init(htim1, DHT22_Port, DHT22_Pin);

#ifdef RECEIVER
  g_NrfHandler.Nrf24RxMode(RxAddress, 10);
  g_LcdHandler.Hd44780Init(hi2c1, 4);
  g_LcdHandler.Hd44780Clear();
  g_PresenceMonitor.Init(&g_LcdHandler);

  Source *p_line1 = new Source((char *)"i");
  Source *p_line2 = new Source((char *)"o");
  Source *p_line3 = new Source((char *)"r");
  Source *p_line4 = new Source((char *)"d");

  g_PresenceMonitor.AddSource(p_line1);
  g_PresenceMonitor.AddSource(p_line2);
  g_PresenceMonitor.AddSource(p_line3);
  g_PresenceMonitor.AddSource(p_line4);
#endif

#ifdef TRANSMITTER
  g_NrfHandler.Nrf24TxMode(TxAddress, 10);
#endif

  g_NrfHandler.Nrf24ReadAll(g_nrfConfigData);
  HAL_Delay(100);

  while (1)
  {
#ifdef RECEIVER
	if (g_remoteCall)
	  {
		if (g_NrfHandler.IsDataAvailable(2) == 1)
		{
		  toggle_led();
		  g_NrfHandler.Nrf24Receive(g_TransferValues.str);
		  g_PresenceMonitor.ClearMissing(g_TransferValues.measured_values.remote);
		  print_to_lcd(g_LcdHandler, g_TransferValues.measured_values.remote, g_TransferValues.measured_values);
		}
		g_remoteCall = false;
	  }

    if (g_localCall)
    {
      double_toggle_led();
      measure(g_LocalMeasurements);
      print_to_lcd(g_LcdHandler, 0, g_LocalMeasurements);
      g_PresenceMonitor.ClearMissing(0);
      g_PresenceMonitor.Update();
      g_PresenceMonitor.ShowMissingRemote();

      g_localCall = false;
    }


#endif


#ifdef TRANSMITTER
	measure(g_TransferValues.measured_values);
	if (g_NrfHandler.Nrf24Transmit(g_TransferValues.str) == 1)
	{
		double_toggle_led();
	}
	HAL_Delay(3000);
#endif
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#ifdef RECEIVER
  if (GPIO_Pin == NRF_IRQ_Pin)
  {
    g_remoteCall = true;
  }
#endif
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 10000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/*
 * @brief SPI1 Initialization Function
 */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 30000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NRF_CE_Pin | DHT22_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED PIN */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NRF_CSN_Pin */
  GPIO_InitStruct.Pin = NRF_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(NRF_CSN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : NRF_CE_Pin DHT22_Pin */
  GPIO_InitStruct.Pin = NRF_CE_Pin | DHT22_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : NRF_IRQ_Pin */
  GPIO_InitStruct.Pin = NRF_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NRF_IRQ_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);

  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef RECEIVER
  if (htim == &htim2)
  {
	  g_localCall = true;
  }
#endif

}

void measure(MeasuredValues &measured_values)
{
  float humidity, temperature;
  if (g_DhtHandler.GetValues(humidity, temperature))
  {
	measured_values.humidity = humidity;
	measured_values.temperature = temperature;
  }
  else
  {
	  g_TransferValues.measured_values.temperature = -200.;
	  g_TransferValues.measured_values.humidity = -200.;
  }

#ifdef USE_CO2
	uint16_t co2;
	g_Mhz19Handler.ReadConcentrationCmd(co2);
	g_TransferValues.measured_values.co2 = co2;
#endif

}

void print_to_lcd(LcdI2c& lcdHandler, uint8_t line, MeasuredValues &measured_values)
{
#ifdef RECEIVER
  char temp_str[8];

  if (g_PresenceMonitor.sources.size() > line)
  {
	g_LcdHandler.Hd44780SetCursor(0, line);
	g_LcdHandler.Hd44780PrintStr("                    ");

	g_LcdHandler.Hd44780SetCursor(0, line);
	g_LcdHandler.Hd44780PrintStr(g_PresenceMonitor.sources[line] -> GetName());

	if (measured_values.temperature > -30. and measured_values.temperature < 80)
	{
	  sprintf(temp_str, "%.1fC", measured_values.temperature);
	  g_LcdHandler.Hd44780SetCursor(3, line);
	  g_LcdHandler.Hd44780PrintStr((char *)temp_str);
	}

	if (measured_values.humidity >= 0. and measured_values.humidity <= 100.)
	{
	sprintf(temp_str, "%.0f%%", measured_values.humidity);
	g_LcdHandler.Hd44780SetCursor(10, line);
	g_LcdHandler.Hd44780PrintStr((char *)temp_str);
	}

	if (measured_values.co2 >= 0 and measured_values.co2 <= 5000)
	{
		sprintf(temp_str, "%d", measured_values.co2);
		g_LcdHandler.Hd44780SetCursor(14, line);
		g_LcdHandler.Hd44780PrintStr((char *)temp_str);
	}
  }

#endif
}


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

#ifdef USE_FULL_ASSERT
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
