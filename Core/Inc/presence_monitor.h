#ifndef PRESENCE_MONITOR_H
#define PRESENCE_MONITOR_H

#include <vector>
#include <lcd_i2c.h>
#include "message.h"

class PresenceMonitor
{
public:
	PresenceMonitor();
	~PresenceMonitor();
	void Init(LcdI2c* lcdi2c);
	void RegisterMeasurement(MeasuredValues& mv);
	void Update();
	void UpdateScreen();
	void ClearMissing(uint8_t index);
	void AddSource(Source* source);
	void PrintMeasurements(MeasuredValues& measured_values);

	LcdI2c* m_pLcdI2cHandler;
	std::vector<Source*> m_pSources;
	std::vector<MeasuredValues*> measurements;
};

#endif
