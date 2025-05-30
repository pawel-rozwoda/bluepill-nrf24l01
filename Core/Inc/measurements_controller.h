#ifndef MEASUREMENTS_CONTROLLER_H_
#define MEASUREMENTS_CONTROLLER_H_

#include "message.h"
#include "dht.h"
#include "mhz19.h"

class MeasurementsController
{
public:
	~MeasurementsController();

	void Init(Dht* pDhtHandler,
			  Mhz19* pMhz19Handler);

	void ExecuteMeasurements(MeasuredValues &rMeasuredValues);

private:
	Dht* m_pDhtHandler;
	Mhz19* m_pMhz19Handler;
};

#endif /* INC_MEASUREMENTS_CONTROLLER_H_ */
