
#define UINT16_NAN ((uint16_t)0xFFFF)
#include <vector>
#include "lcd_i2c.h"
#include <nrf24l01.h>

enum SourceName : unsigned char
{
	INSIDE,
	OUTSIDE,
	REMOTE1,
	REMOTE2,
	INVALID,
};

class Source
{
private:
	char *m_name;
	uint8_t m_missingCounter;

public:
	Source(char *name) : m_name(name) {}

	char* GetName()
	{
		return m_name;
	}

	uint8_t GetCounter()
	{
		return m_missingCounter;
	}

	void Update()
	{
		if (m_missingCounter < 3)
			m_missingCounter += 1;
	}

	void Reset()
	{
		m_missingCounter = 0;
	}
};

struct MeasuredValues
{
public:
	SourceName remote;
	float temperature;
	float humidity;
	uint16_t co2;
};

union DataPacket
{
	MeasuredValues measured_values;
	uint8_t str[PACKET_LENGTH];
};

class PresenceMonitor
{
public:
	LcdI2c* m_lcdi2c;
	std::vector<Source *> sources;
	void Init(LcdI2c* lcdi2c)
	{
		m_lcdi2c = lcdi2c;
	}

	void Update()
	{
		for (Source *source : sources)
		{
			source->Update();
		}
	}
	void ShowMissingRemote()
	{
		for (uint8_t i = 0; i < sources.size(); i++)
		{
			if (sources[i]->GetCounter() >= 3)
			{
				m_lcdi2c->Hd44780SetCursor(3, i);
				m_lcdi2c->Hd44780PrintStr("-----            ");
			}
		}
	}

	void ClearMissing(uint8_t index)
	{
		if (sources.size() > index)
			sources[index]->Reset();
	}

	void AddSource(Source *source)
	{
		m_lcdi2c->Hd44780SetCursor(0, sources.size());
		m_lcdi2c->Hd44780PrintStr(source->GetName());
		sources.push_back(source);
	}
};
