#include "presence_monitor.h"

PresenceMonitor::PresenceMonitor() {}

PresenceMonitor::~PresenceMonitor()
{
	for (Source* source : m_pSources)
	{
		delete source;
	}
	m_pSources.clear();

	for (MeasuredValues* pMeasuredValues : measurements)
	{
		delete pMeasuredValues;
	}
	m_pSources.clear();
}

void PresenceMonitor::Init(LcdI2c* lcdi2c)
{
	m_pLcdI2cHandler = lcdi2c;
}

void PresenceMonitor::RegisterMeasurement(MeasuredValues& mv)
{
	if (mv.remote <= measurements.size())
	{
		*measurements.at(mv.remote) = mv;
		ClearMissing(mv.remote);
	}
}

void PresenceMonitor::Update()
{
	for (Source *source : m_pSources)
	{
		source->Update();
	}
}

void PresenceMonitor::UpdateScreen()
{
	for (uint8_t i = 0; i < m_pSources.size(); i++)
	{
		if (m_pSources[i]->GetCounter() >= 3)
		{
			m_pLcdI2cHandler->Hd44780SetCursor(3, i);
			m_pLcdI2cHandler->Hd44780PrintStr("-----            ");
		}
		else
		{
			PrintMeasurements(*measurements.at(i));
		}
	}
}

void PresenceMonitor::ClearMissing(uint8_t index)
{
	if (m_pSources.size() > index)
	{
		m_pSources[index]->Reset();
	}
}

void PresenceMonitor::AddSource(Source* source)
{
	m_pLcdI2cHandler->Hd44780SetCursor(0, m_pSources.size());
	m_pLcdI2cHandler->Hd44780PrintStr(source->GetName());
	m_pSources.push_back(source);

	MeasuredValues* temp = new MeasuredValues();
	temp->remote = SourceName::INVALID;
	temp->humidity = -200.;
	temp->temperature = -200.;
	temp->co2 = UINT16_NAN;
	measurements.push_back(new MeasuredValues());
}

void PresenceMonitor::PrintMeasurements(MeasuredValues& measured_values)
{
	char temp_str[8];

	if (m_pSources.size() > measured_values.remote)
	{
		m_pLcdI2cHandler->Hd44780SetCursor(0, measured_values.remote);
		m_pLcdI2cHandler->Hd44780PrintStr("                    ");

		m_pLcdI2cHandler->Hd44780SetCursor(0, measured_values.remote);
		m_pLcdI2cHandler->Hd44780PrintStr(m_pSources[measured_values.remote]->GetName());

		if (measured_values.temperature > -30. and measured_values.temperature < 80)
		{
			sprintf(temp_str, "%.1fC", measured_values.temperature);
			m_pLcdI2cHandler->Hd44780SetCursor(3, measured_values.remote);
			m_pLcdI2cHandler->Hd44780PrintStr((char *)temp_str);
		}

		if (measured_values.humidity >= 0. and measured_values.humidity <= 100.)
		{
			sprintf(temp_str, "%.0f%%", measured_values.humidity);
			m_pLcdI2cHandler->Hd44780SetCursor(10, measured_values.remote);
			m_pLcdI2cHandler->Hd44780PrintStr((char *)temp_str);
		}

		if (measured_values.co2 >= 0 and measured_values.co2 <= 5000)
		{
			sprintf(temp_str, "%d", measured_values.co2);
			m_pLcdI2cHandler->Hd44780SetCursor(14, measured_values.remote);
			m_pLcdI2cHandler->Hd44780PrintStr((char *)temp_str);
		}
	}
}
