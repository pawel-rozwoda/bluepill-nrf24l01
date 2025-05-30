#ifndef MESSAGE_H_
#define MESSAGE_H_

#define UINT16_NAN ((uint16_t)0xFFFF)
#include <vector>
#include <stdio.h>
#include <lcd_i2c.h>
#include <nrf24l01.h>

enum SourceName : unsigned char
{
	INSIDE,
	OUTSIDE,
	REMOTE1,
	REMOTE2,
	INVALID,
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

class Source
{
private:
	char* m_pName;
	uint8_t m_MissingCounter;

public:
	~Source() { delete m_pName; }
	Source(char *name) : m_pName(name), m_MissingCounter(3) {}

	char* GetName()
	{
		return m_pName;
	}

	uint8_t GetCounter()
	{
		return m_MissingCounter;
	}

	void Update()
	{
		if (m_MissingCounter < 3)
			m_MissingCounter += 1;
	}

	void Reset()
	{
		m_MissingCounter = 0;
	}
};

#endif
