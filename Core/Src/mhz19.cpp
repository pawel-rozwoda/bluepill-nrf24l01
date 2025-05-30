#include "mhz19.h"

static uint8_t mhz19_cmdIndex = 0;
uint8_t mhz19LastResponse[9];
wchar_t *statusString = L"xx";
static const uint8_t GAS_CONCENTRATION_READ[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
static uint8_t CALIBRATE_ZERO[] = {0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

void Mhz19::Init(UART_HandleTypeDef* huart)
{
  m_huart = huart;
}

void Mhz19::ReadConcentrationCmd(uint16_t& co2)
{
  uint8_t mhz_response[9];
  HAL_UART_Transmit(m_huart, GAS_CONCENTRATION_READ, 9, 100);
  mhz19_cmdIndex = 0;

  HAL_UART_Receive(m_huart, mhz_response, 9, 1000);

  co2 = (mhz_response[2] * 256) + mhz_response[3];
}

void Mhz19::CalibrateZero()
{
  for (int i = 0; i < 9; i++)
    HAL_UART_Transmit(m_huart, &CALIBRATE_ZERO[i], 1, 1);
}

uint16_t Mhz19::LastConcentration(uint16_t calibrated)
{
  MHZ19_RESPONSE *r = (MHZ19_RESPONSE *)mhz19LastResponse;
  return r->HH * 256 + r->LL - calibrated;
}

uint16_t Mhz19::LastTempCelsius()
{
  MHZ19_RESPONSE* r = (MHZ19_RESPONSE*)mhz19LastResponse;
  return r->TT - 40;
}

uint8_t* Mhz19::LastResp()
{
  return mhz19LastResponse;
}

uint8_t* Mhz19::LastStatus()
{
  MHZ19_RESPONSE* r = (MHZ19_RESPONSE*)mhz19LastResponse;
  statusString = L"........";
  uint8_t ss = r->SS;
  for (uint8_t i = 0; i < 8; i++)
  {
    if (ss & 0x01)
      statusString[7 - i] = L'l';
    else
      statusString[7 - i] = L'0';
    ss >>= 1;
  }
  return (uint8_t*)statusString;
}

uint8_t Mhz19::CalcLastCrc()
{
  uint8_t result = 0xff - (mhz19LastResponse[1] + mhz19LastResponse[2] + mhz19LastResponse[3] + mhz19LastResponse[4] + mhz19LastResponse[5] + mhz19LastResponse[6] + mhz19LastResponse[7]) + 0x01;
  return result;
}
