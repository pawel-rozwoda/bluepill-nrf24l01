#include "measurements_controller.h"

MeasurementsController::~MeasurementsController()
{
	delete m_pDhtHandler;
	delete m_pMhz19Handler;
}

void MeasurementsController::Init(Dht* pDhtHandler,
								  Mhz19* pMhz19Handler)
{
	m_pDhtHandler = pDhtHandler;
	m_pMhz19Handler = pMhz19Handler;
}

void MeasurementsController::ExecuteMeasurements(MeasuredValues& rMeasuredValues)
{
	float humidity, temperature;
	uint16_t co2;

	if (m_pDhtHandler->GetValues(humidity, temperature))
	{
		rMeasuredValues.humidity = humidity;
		rMeasuredValues.temperature = temperature;
	}
	else
	{
		rMeasuredValues.temperature = 200.;
		rMeasuredValues.humidity = 200.;
	}

	m_pMhz19Handler->ReadConcentrationCmd(co2);
	rMeasuredValues.co2 = co2;
}
