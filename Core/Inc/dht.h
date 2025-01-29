#ifndef DHT_H
#define DHT_H

#include "stm32f1xx_hal.h"

class Dht{

public:
	Dht();
	void init(TIM_HandleTypeDef htim1, GPIO_TypeDef* port, uint16_t pin);
	bool get_values(float& humidity, float& temperature);

private:
	uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
	uint32_t pMillis, cMillis;
	float temp_Celsius = 0;
	float temp_Fahrenheit = 0;
	float Humidity = 0;
	uint8_t hum_integral, hum_decimal, tempC_integral, tempC_decimal, tempF_integral, tempF_decimal;
	TIM_HandleTypeDef htim;
	uint16_t dht_pin;
	GPIO_TypeDef* dht_port;

	void microDelay (uint16_t delay);
	uint8_t DHT22_Start (void);
	uint8_t DHT22_Read (void);

};

#endif //DHT_H
