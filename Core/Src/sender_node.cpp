#include "node.h"

SenderNode::SenderNode() {}

SenderNode::~SenderNode()
{
	delete m_pNrfHandler;
	delete m_pLcdHandler;
	delete m_pMeasurementsHandler;
}

void SenderNode::Execute() {}

void SenderNode::LocalCall()
{
	m_pMeasurementsHandler->ExecuteMeasurements(m_TransferPacket.measured_values);
	m_pNrfHandler->Nrf24Transmit(m_TransferPacket.str);
};

void SenderNode::Init(LcdI2c* pLcdHandler, Nrf24* pNrfHandler, MeasurementsController* pMeasurementHandler)
{
	m_pNrfHandler = pNrfHandler;
	m_pMeasurementsHandler = pMeasurementHandler;
	m_pNrfHandler->Nrf24TxMode(RxTxAddress, 10);
	m_TransferPacket = {SourceName::OUTSIDE, (float)-200., (float)-200., UINT16_NAN};
}
