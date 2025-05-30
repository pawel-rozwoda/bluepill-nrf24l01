#include "node.h"

RecipientNode::RecipientNode() {}

RecipientNode::~RecipientNode()
{
	delete m_pNrfHandler;
	delete m_pLcdHandler;
	delete m_pMeasurementsHandler;
}

void RecipientNode::Execute()
{
	DataPacket transfer_values;
	if (m_pNrfHandler->IsDataAvailable(2) == 1)
	{
		m_pNrfHandler->Nrf24Receive(transfer_values.str);
	}
	m_PresenceMonitor.RegisterMeasurement(transfer_values.measured_values);
}

void RecipientNode::LocalCall()
{
	m_pMeasurementsHandler->ExecuteMeasurements(m_InsideMeasurements);
	m_PresenceMonitor.RegisterMeasurement(m_InsideMeasurements);
	m_PresenceMonitor.UpdateScreen();
	m_PresenceMonitor.Update();
}

void RecipientNode::Init(LcdI2c* pLcdHandler, Nrf24* pNrfHandler, MeasurementsController* pMeasurementHandler)
{
	m_pLcdHandler = pLcdHandler;
	m_pNrfHandler = pNrfHandler;
	m_pMeasurementsHandler = pMeasurementHandler;

	Source *p_line1 = new Source((char *)"i");
	Source *p_line2 = new Source((char *)"o");
	Source *p_line3 = new Source((char *)"r");
	Source *p_line4 = new Source((char *)"d");

	m_PresenceMonitor.Init(pLcdHandler);
	m_PresenceMonitor.AddSource(p_line1);
	m_PresenceMonitor.AddSource(p_line2);
	m_PresenceMonitor.AddSource(p_line3);
	m_PresenceMonitor.AddSource(p_line4);
	m_pNrfHandler->Nrf24RxMode(RxTxAddress, 10);
	m_InsideMeasurements = {SourceName::INSIDE, (float)-200., (float)-200., UINT16_NAN};
}
