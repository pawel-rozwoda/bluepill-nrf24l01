#ifndef NODE_H_
#define NODE_H_

#include "message.h"
#include "lcd_i2c.h"
#include "dht.h"
#include "mhz19.h"
#include "presence_monitor.h"
#include "measurements_controller.h"

static uint8_t RxTxAddress[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

class Node
{
public:
	virtual void Execute() = 0;
	virtual void LocalCall() = 0;
	virtual void Init(LcdI2c* pLcdHandler, Nrf24* pNrfHandler, MeasurementsController* pMeasurementHandler) = 0;

	MeasurementsController *m_pMeasurementsHandler;
	Nrf24 *m_pNrfHandler;
	LcdI2c *m_pLcdHandler;
};

class SenderNode : public Node
{
public:
	SenderNode();
	~SenderNode();
	void Execute();
	void LocalCall();
	virtual void Init(LcdI2c* pLcdHandler, Nrf24* pNrfHandler, MeasurementsController* pMeasurementHandler);

	DataPacket m_TransferPacket;
};

class RecipientNode : public Node
{
public:
	RecipientNode();
	~RecipientNode();
	void Execute();
	void LocalCall();
	virtual void Init(LcdI2c* pLcdHandler, Nrf24* pNrfHandler, MeasurementsController* pMeasurementHandler);

private:
	PresenceMonitor m_PresenceMonitor;
	MeasuredValues m_InsideMeasurements;
};

#endif
