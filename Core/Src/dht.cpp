#include "dht.h"

Dht::Dht()
{
}

void Dht::Init(TIM_HandleTypeDef htim, GPIO_TypeDef* port, uint16_t pin)
{
	m_dht_port = port;
	m_dht_pin = pin;
	m_htim = htim;
}

void Dht::MicroDelay(uint16_t delay)
{
	__HAL_TIM_SET_COUNTER(&m_htim, 0);
	while (__HAL_TIM_GET_COUNTER(&m_htim) < delay)
		;
}

uint8_t Dht::Dht22Start(void)
{
	uint8_t Response = 0;
	GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
	GPIO_InitStructPrivate.Pin = m_dht_pin;
	GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
	HAL_GPIO_Init((GPIO_TypeDef *)m_dht_port, &GPIO_InitStructPrivate);
	HAL_GPIO_WritePin((GPIO_TypeDef *)m_dht_port, m_dht_pin, (GPIO_PinState)0);
	MicroDelay(1300);
	HAL_GPIO_WritePin((GPIO_TypeDef *)m_dht_port, m_dht_pin, (GPIO_PinState)1);
	MicroDelay(30);
	GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
	HAL_GPIO_Init((GPIO_TypeDef *)m_dht_port, &GPIO_InitStructPrivate);
	MicroDelay(40);
	if (!(HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin)))
	{
		MicroDelay(80);
		if ((HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin)))
			Response = 1;
	}
	pMillis = HAL_GetTick();
	cMillis = HAL_GetTick();
	while ((HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin)) && pMillis + 2 > cMillis)
	{
		cMillis = HAL_GetTick();
	}
	return Response;
}

uint8_t Dht::Dht22Read(void)
{
	uint8_t x, y = 0;
	for (x = 0; x < 8; x++)
	{
		pMillis = HAL_GetTick();
		cMillis = HAL_GetTick();
		while (!(HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin)) && pMillis + 2 > cMillis)
		{
			cMillis = HAL_GetTick();
		}
		MicroDelay(40);
		if (!(HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin))) // if the pin is low
			y &= ~(1 << (7 - x));
		else
			y |= (1 << (7 - x));
		pMillis = HAL_GetTick();
		cMillis = HAL_GetTick();
		while ((HAL_GPIO_ReadPin((GPIO_TypeDef *)m_dht_port, m_dht_pin)) && pMillis + 2 > cMillis)
		{ // wait for the pin to go low
			cMillis = HAL_GetTick();
		}
	}
	return y;
}

bool Dht::GetValues(float& humidity, float& temperature)
{
	if (Dht22Start())
	{
		hum1 = Dht22Read();
		hum2 = Dht22Read();
		tempC1 = Dht22Read();
		tempC2 = Dht22Read();
		SUM = Dht22Read();
		CHECK = hum1 + hum2 + tempC1 + tempC2;
		if (CHECK == SUM)
		{
			if (tempC1 > 127)
			{
				temp_Celsius = (float)tempC2 / 10 * (-1);
			}
			else
			{
				temp_Celsius = (float)((tempC1 << 8) | tempC2) / 10;
			}

			temp_Fahrenheit = temp_Celsius * 9 / 5 + 32;

			Humidity = (float)((hum1 << 8) | hum2) / 10;

			hum_integral = Humidity;
			hum_decimal = Humidity * 10 - hum_integral * 10;

			if (temp_Celsius < 0)
			{
				tempC_integral = temp_Celsius * (-1);
				tempC_decimal = temp_Celsius * (-10) - tempC_integral * 10;
				//	  	  	           sprintf(string,"-%d.%d C   ", tempC_integral, tempC_decimal);
			}
			else
			{
				tempC_integral = temp_Celsius;
				tempC_decimal = temp_Celsius * 10 - tempC_integral * 10;
				//	  	  	           sprintf(string,"%d.%d C   ", tempC_integral, tempC_decimal);
			}

			humidity = Humidity;
			temperature = temp_Celsius;
		}

		return true;
	}

	else
	{
		return false;
	}
}
