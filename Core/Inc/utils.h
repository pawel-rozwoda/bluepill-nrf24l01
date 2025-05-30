#ifndef INC_UTILS_H_
#define INC_UTILS_H_
#include "main.h"

#define MY_UINT16_MAX (UINT16_MAX - 1)

void toggle_led()
{
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);
}

void double_toggle_led()
{
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);
}

#endif /* INC_UTILS_H_ */
