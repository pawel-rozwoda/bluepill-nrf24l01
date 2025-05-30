/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/
#define NRF_CSN_Pin GPIO_PIN_4
#define NRF_CSN_GPIO_Port GPIOA
#define NRF_CE_Pin GPIO_PIN_1
#define NRF_CE_GPIO_Port GPIOB
#define NRF_IRQ_Pin GPIO_PIN_9
#define NRF_IRQ_GPIO_Port GPIOA
#define NRF_IRQ_EXTI_IRQn EXTI9_5_IRQn

#define LED_Pin GPIO_PIN_13
#define LED_Port GPIOC

#define DHT22_Pin GPIO_PIN_14
#define DHT22_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
