#include "lcd_i2c.h"

void LcdI2c::Hd44780Init(I2C_HandleTypeDef _hi2c, uint8_t rows)
{
  hi2c = _hi2c;

  m_dpRows = rows;

  m_dpBacklight = LCD_BACKLIGHT;

  m_dpFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  if (m_dpRows > 1)
  {
    m_dpFunction |= LCD_2LINE;
  }
  else
  {
    m_dpFunction |= LCD_5x10DOTS;
  }

  /* Wait for initialization */
  DelayInit();
  HAL_Delay(50);

  ExpanderWrite(m_dpBacklight);
  HAL_Delay(1000);

  /* 4bit Mode */
  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x03 << 4);
  DelayUS(4500);

  Write4Bits(0x02 << 4);
  DelayUS(100);

  /* Display Control */
  SendCommand(LCD_FUNCTIONSET | m_dpFunction);

  m_dpControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  Hd44780Display();
  Hd44780Clear();

  /* Display Mode */
  m_dpMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  SendCommand(LCD_ENTRYMODESET | m_dpMode);
  DelayUS(4500);

  Hd44780CreateSpecialChar(0, special1);
  Hd44780CreateSpecialChar(1, special2);

  Hd44780Home();
}

void LcdI2c::Hd44780Clear()
{
  SendCommand(LCD_CLEARDISPLAY);
  DelayUS(2000);
}

void LcdI2c::Hd44780Home()
{
  SendCommand(LCD_RETURNHOME);
  DelayUS(2000);
}

void LcdI2c::Hd44780SetCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row >= m_dpRows)
  {
    row = m_dpRows - 1;
  }
  SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LcdI2c::Hd44780NoDisplay()
{
  m_dpControl &= ~LCD_DISPLAYON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780Display()
{
  m_dpControl |= LCD_DISPLAYON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780NoCursor()
{
  m_dpControl &= ~LCD_CURSORON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780Cursor()
{
  m_dpControl |= LCD_CURSORON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780NoBlink()
{
  m_dpControl &= ~LCD_BLINKON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780Blink()
{
  m_dpControl |= LCD_BLINKON;
  SendCommand(LCD_DISPLAYCONTROL | m_dpControl);
}

void LcdI2c::Hd44780ScrollDisplayLeft(void)
{
  SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LcdI2c::Hd44780ScrollDisplayRight(void)
{
  SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LcdI2c::Hd44780LeftToRight(void)
{
  m_dpMode |= LCD_ENTRYLEFT;
  SendCommand(LCD_ENTRYMODESET | m_dpMode);
}

void LcdI2c::Hd44780RightToLeft(void)
{
  m_dpMode &= ~LCD_ENTRYLEFT;
  SendCommand(LCD_ENTRYMODESET | m_dpMode);
}

void LcdI2c::Hd44780AutoScroll(void)
{
  m_dpMode |= LCD_ENTRYSHIFTINCREMENT;
  SendCommand(LCD_ENTRYMODESET | m_dpMode);
}

void LcdI2c::Hd44780NoAutoScroll(void)
{
  m_dpMode &= ~LCD_ENTRYSHIFTINCREMENT;
  SendCommand(LCD_ENTRYMODESET | m_dpMode);
}

void LcdI2c::Hd44780CreateSpecialChar(uint8_t location, uint8_t charmap[])
{
  location &= 0x7;
  SendCommand(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++)
  {
    SendChar(charmap[i]);
  }
}

void LcdI2c::Hd44780PrintSpecialChar(uint8_t index)
{
  SendChar(index);
}

void LcdI2c::Hd44780LoadCustomCharacter(uint8_t char_num, uint8_t* rows)
{
  Hd44780CreateSpecialChar(char_num, rows);
}

void LcdI2c::Hd44780PrintStr(const char c[])
{
  while (*c)
    SendChar(*c++);
}

void LcdI2c::Hd44780SetBacklight(uint8_t new_val)
{
  if (new_val)
    Hd44780Backlight();
  else
    Hd44780NoBacklight();
}

void LcdI2c::Hd44780NoBacklight(void)
{
  m_dpBacklight = LCD_NOBACKLIGHT;
  ExpanderWrite(0);
}

void LcdI2c::Hd44780Backlight(void)
{
  m_dpBacklight = LCD_BACKLIGHT;
  ExpanderWrite(0);
}

void LcdI2c::SendCommand(uint8_t cmd)
{
  Send(cmd, 0);
}

void LcdI2c::SendChar(uint8_t ch)
{
  Send(ch, RS);
}

void LcdI2c::Send(uint8_t value, uint8_t mode)
{
  uint8_t highnib = value & 0xF0;
  uint8_t lownib = (value << 4) & 0xF0;
  Write4Bits((highnib) | mode);
  Write4Bits((lownib) | mode);
}

void LcdI2c::Write4Bits(uint8_t value)
{
  ExpanderWrite(value);
  PulseEnable(value);
}

void LcdI2c::ExpanderWrite(uint8_t _data)
{
  uint8_t data = _data | m_dpBacklight;
  HAL_I2C_Master_Transmit(&hi2c, DEVICE_ADDR, (uint8_t*)&data, 1, 10);
}

void LcdI2c::PulseEnable(uint8_t _data)
{
  ExpanderWrite(_data | ENABLE);
  DelayUS(20);

  ExpanderWrite(_data & ~ENABLE);
  DelayUS(20);
}

void LcdI2c::DelayInit(void)
{
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // 0x00000001;

  DWT->CYCCNT = 0;

  /* 3 NO OPERATION instructions */
  __ASM volatile("NOP");
  __ASM volatile("NOP");
  __ASM volatile("NOP");
}

void LcdI2c::DelayUS(uint32_t us)
{
  uint32_t cycles = (SystemCoreClock / 1000000L) * us;
  uint32_t start = DWT->CYCCNT;
  volatile uint32_t cnt;

  do
  {
    cnt = DWT->CYCCNT - start;
  } while (cnt < cycles);
}
