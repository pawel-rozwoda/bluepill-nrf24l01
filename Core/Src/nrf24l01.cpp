#include <nrf24l01.h>
#include "stm32f1xx_hal.h"

#define EVENT_RX_DR 6

void Nrf24::CsSelect(void)
{
	HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
}

void Nrf24::CsUnselect(void)
{
	HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
}

void Nrf24::CeEnable(void)
{
	HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
}

void Nrf24::CeDisable(void)
{
	HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
}

// write a single byte to the particular register
void Nrf24::Nrf24WriteReg(uint8_t Reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = Reg | 1 << 5;
	buf[1] = Data;

	// Pull the CS Pin LOW to select the device
	CsSelect();

	HAL_SPI_Transmit(&m_hspi, buf, 2, 1000);

	// Pull the CS HIGH to release the device
	CsUnselect();
}

// write multiple bytes starting from a particular register
void Nrf24::Nrf24WriteRegMulti(uint8_t Reg, uint8_t* data, int size)
{
	uint8_t buf[2];
	buf[0] = Reg | 1 << 5;

	CsSelect();

	HAL_SPI_Transmit(&m_hspi, buf, 1, 100);
	HAL_SPI_Transmit(&m_hspi, data, size, 1000);

	CsUnselect();
}

uint8_t Nrf24::Nrf24ReadReg(uint8_t Reg)
{
	uint8_t data = 0;

	// Pull the CS Pin LOW to select the device
	CsSelect();

	HAL_SPI_Transmit(&m_hspi, &Reg, 1, 100);
	HAL_SPI_Receive(&m_hspi, &data, 1, 100);

	// Pull the CS HIGH to release the device
	CsUnselect();

	return data;
}

/* Read multiple bytes from the register */
void Nrf24::Nrf24ReadRegMulti(uint8_t Reg, uint8_t* data, int size)
{
	// Pull the CS Pin LOW to select the device
	CsSelect();

	HAL_SPI_Transmit(&m_hspi, &Reg, 1, 100);
	HAL_SPI_Receive(&m_hspi, data, size, 1000);

	// Pull the CS HIGH to release the device
	CsUnselect();
}

// send the command to the NRF
void Nrf24::NrfSendCmd(uint8_t cmd)
{
	// Pull the CS Pin LOW to select the device
	CsSelect();

	HAL_SPI_Transmit(&m_hspi, &cmd, 1, 100);

	// Pull the CS HIGH to release the device
	CsUnselect();
}

void Nrf24::Nrf24Reset(uint8_t REG)
{
	if (REG == STATUS)
	{
		Nrf24WriteReg(STATUS, 0x00);
	}

	else if (REG == FIFO_STATUS)
	{
		Nrf24WriteReg(FIFO_STATUS, 0x11);
	}

	else
	{
		Nrf24WriteReg(CONFIG, 0x08);
		Nrf24WriteReg(EN_AA, 0x3F);
		Nrf24WriteReg(EN_RXADDR, 0x03);
		Nrf24WriteReg(SETUP_AW, 0x03);
		Nrf24WriteReg(SETUP_RETR, 0x03);
		Nrf24WriteReg(RF_CH, 0x02);
		Nrf24WriteReg(RF_SETUP, 0x0E);
		Nrf24WriteReg(STATUS, 0x00);
		Nrf24WriteReg(OBSERVE_TX, 0x00);
		Nrf24WriteReg(CD, 0x00);
		uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
		Nrf24WriteRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
		uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
		Nrf24WriteRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
		Nrf24WriteReg(RX_ADDR_P2, 0xC3);
		Nrf24WriteReg(RX_ADDR_P3, 0xC4);
		Nrf24WriteReg(RX_ADDR_P4, 0xC5);
		Nrf24WriteReg(RX_ADDR_P5, 0xC6);
		uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
		Nrf24WriteRegMulti(TX_ADDR, tx_addr_def, 5);
		Nrf24WriteReg(RX_PW_P0, 0);
		Nrf24WriteReg(RX_PW_P1, 0);
		Nrf24WriteReg(RX_PW_P2, 0);
		Nrf24WriteReg(RX_PW_P3, 0);
		Nrf24WriteReg(RX_PW_P4, 0);
		Nrf24WriteReg(RX_PW_P5, 0);
		Nrf24WriteReg(FIFO_STATUS, 0x11);
		Nrf24WriteReg(DYNPD, 0);
		Nrf24WriteReg(FEATURE, 0);
	}
}

void Nrf24::Nrf24Init(SPI_HandleTypeDef hspi)
{
	m_hspi = hspi;
	// disable the chip before configuring the device
	CeDisable();

	// reset everything
	Nrf24Reset(0);

	//	nrf24_WriteReg(CONFIG, 1<<5);

	Nrf24WriteReg(CONFIG, 0); // will be configured later

	Nrf24WriteReg(EN_AA, 0); // No Auto ACK

	Nrf24WriteReg(EN_RXADDR, 0); // Not Enabling any data pipe right now

	Nrf24WriteReg(SETUP_AW, 0x03); // 5 Bytes for the TX/RX address

	Nrf24WriteReg(SETUP_RETR, 0); // No retransmission

	Nrf24WriteReg(RF_CH, 0); // will be setup during Tx or RX

	Nrf24WriteReg(RF_SETUP, 0x0E); // Power= 0db, data rate = 2Mbps

	// Enable the chip after configuring the device
	CeEnable();
}

// set up the Tx mode

void Nrf24::Nrf24TxMode(uint8_t* Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CeDisable();

	Nrf24WriteReg(RF_CH, channel); // select the channel

	Nrf24WriteRegMulti(TX_ADDR, Address, 5); // Write the TX address

	// power up the device
	uint8_t config = Nrf24ReadReg(CONFIG);
	config = config | (1 << 1); // write 1 in the PWR_UP bit
	config = config & (0xF2);	// write 0 in the PRIM_RX, and 1 in the PWR_UP, and all other bits are masked
	Nrf24WriteReg(CONFIG, config);

	// Enable the chip after configuring the device
	CeEnable();
}

// transmit the data

uint8_t Nrf24::Nrf24Transmit(uint8_t* data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CsSelect();

	// payload command
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(&m_hspi, &cmdtosend, 1, 100);

	// send the payload
	HAL_SPI_Transmit(&m_hspi, data, PACKET_LENGTH, 1000);

	// Unselect the device
	CsUnselect();

	HAL_Delay(1);

	uint8_t fifostatus = Nrf24ReadReg(FIFO_STATUS);

	// check the fourth bit of FIFO_STATUS to know if the TX fifo is empty
	if ((fifostatus & (1 << 4)) && (!(fifostatus & (1 << 3))))
	{
		cmdtosend = FLUSH_TX;
		NrfSendCmd(cmdtosend);

		// reset FIFO_STATUS
		Nrf24Reset(FIFO_STATUS);

		return 1;
	}

	return 0;
}

void Nrf24::Nrf24RxMode(uint8_t* Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CeDisable();

	Nrf24Reset(STATUS);

	Nrf24WriteReg(RF_CH, channel); // select the channel

	// select data pipe 2
	uint8_t en_rxaddr = Nrf24ReadReg(EN_RXADDR);
	en_rxaddr = en_rxaddr | (1 << 2);
	Nrf24WriteReg(EN_RXADDR, en_rxaddr);

	/* We must write the address for Data Pipe 1, if we want to use any pipe from 2 to 5
	 * The Address from DATA Pipe 2 to Data Pipe 5 differs only in the LSB
	 * Their 4 MSB Bytes will still be same as Data Pipe 1
	 *
	 * For Eg->
	 * Pipe 1 ADDR = 0xAABBCCDD11
	 * Pipe 2 ADDR = 0xAABBCCDD22
	 * Pipe 3 ADDR = 0xAABBCCDD33
	 *
	 */
	Nrf24WriteRegMulti(RX_ADDR_P1, Address, 5); // Write the Pipe1 address
	Nrf24WriteReg(RX_ADDR_P2, 0xEE);			// Write the Pipe2 LSB address

	Nrf24WriteReg(RX_PW_P2, 32); // 32 bit payload size for pipe 2

	// power up the device in Rx mode
	uint8_t config = Nrf24ReadReg(CONFIG);
	config = config | (1 << 1) | (1 << 0);
	Nrf24WriteReg(CONFIG, config);

	// Enable the chip after configuring the device

	Nrf24WriteReg(STATUS, 1 << 6);

	CeEnable();
}

uint8_t Nrf24::IsDataAvailable(int pipenum)
{
	uint8_t status = Nrf24ReadReg(STATUS);

	if ((status & (1 << 6)) && (status & (pipenum << 1)))
	{

		Nrf24WriteReg(STATUS, (1 << 6));

		return 1;
	}

	return 0;
}

void Nrf24::Nrf24Receive(uint8_t* data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CsSelect();

	// payload command
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(&m_hspi, &cmdtosend, 1, 100);

	// Receive the payload
	HAL_SPI_Receive(&m_hspi, data, PACKET_LENGTH, 1000);

	// Unselect the device
	CsUnselect();

	HAL_Delay(1);

	cmdtosend = FLUSH_RX;
	NrfSendCmd(cmdtosend);
}

// Read all the Register data
void Nrf24::Nrf24ReadAll(uint8_t* data)
{
	for (int i = 0; i < 10; i++)
	{
		*(data + i) = Nrf24ReadReg(i);
	}

	Nrf24ReadRegMulti(RX_ADDR_P0, (data + 10), 5);

	Nrf24ReadRegMulti(RX_ADDR_P1, (data + 15), 5);

	*(data + 20) = Nrf24ReadReg(RX_ADDR_P2);
	*(data + 21) = Nrf24ReadReg(RX_ADDR_P3);
	*(data + 22) = Nrf24ReadReg(RX_ADDR_P4);
	*(data + 23) = Nrf24ReadReg(RX_ADDR_P5);

	Nrf24ReadRegMulti(RX_ADDR_P0, (data + 24), 5);

	for (int i = 29; i < 38; i++)
	{
		*(data + i) = Nrf24ReadReg(i - 12);
	}
}
