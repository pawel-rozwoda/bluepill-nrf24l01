#ifndef MHZ19_H
#define MHZ19_H

#include <stddef.h>
#include "stm32f1xx_hal.h"

class Mhz19
{
public:
	void Init(UART_HandleTypeDef* huart);
	void ReadConcentrationCmd(uint16_t &co2);

private:
	uint16_t LastConcentration(uint16_t calibrated);
	uint16_t LastTempCelsius();
	uint8_t* LastResp();
	uint8_t* LastStatus();
	uint8_t CalcLastCrc();
	void CalibrateZero();

	typedef struct
	{
		uint8_t CMD;
		uint8_t CM;
		uint8_t HH;
		uint8_t LL;
		uint8_t TT;
		uint8_t SS;
		uint8_t Uh;
		uint8_t Ul;
		uint8_t CS;
	} MHZ19_RESPONSE;

	UART_HandleTypeDef *m_huart;
};

#endif // MHZ19_H
