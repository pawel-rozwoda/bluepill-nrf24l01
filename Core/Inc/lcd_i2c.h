#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include "stm32f1xx_hal.h"

/* Command */
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

/* Entry Mode */
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/* Display On/Off */
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00

/* Cursor Shift */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

/* Function Set */
#define LCD_8BITMODE    0x10
#define LCD_4BITMODE    0x00
#define LCD_2LINE       0x08
#define LCD_1LINE       0x00
#define LCD_5x10DOTS    0x04
#define LCD_5x8DOTS     0x00

/* Backlight */
#define LCD_BACKLIGHT   0x08
#define LCD_NOBACKLIGHT 0x00

/* Enable Bit */
#define ENABLE 0x04

/* Read Write Bit */
#define RW 0x0

/* Register Select Bit */
#define RS 0x01

/* Device I2C Address */
#define DEVICE_ADDR (0x27 << 1)

class LcdI2c
{
public:
	LcdI2c() {}
	void Hd44780Init(I2C_HandleTypeDef _hi2c, uint8_t rows);
	void Hd44780Clear();
	void Hd44780Home();
	void Hd44780NoDisplay();
	void Hd44780Display();
	void Hd44780NoBlink();
	void Hd44780Blink();
	void Hd44780NoCursor();
	void Hd44780Cursor();
	void Hd44780ScrollDisplayLeft();
	void Hd44780ScrollDisplayRight();
	void Hd44780PrintLeft();
	void Hd44780PrintRight();
	void Hd44780LeftToRight();
	void Hd44780RightToLeft();
	void Hd44780ShiftIncrement();
	void Hd44780ShiftDecrement();
	void Hd44780NoBacklight();
	void Hd44780Backlight();
	void Hd44780AutoScroll();
	void Hd44780NoAutoScroll();
	void Hd44780CreateSpecialChar(uint8_t, uint8_t[]);
	void Hd44780PrintSpecialChar(uint8_t);
	void Hd44780SetCursor(uint8_t, uint8_t);
	void Hd44780SetBacklight(uint8_t new_val);
	void Hd44780LoadCustomCharacter(uint8_t char_num, uint8_t *rows);
	void Hd44780PrintStr(const char[]);

private:
	I2C_HandleTypeDef hi2c;

	uint8_t special1[8] = {
		0b00000,
		0b11001,
		0b11011,
		0b00110,
		0b01100,
		0b11011,
		0b10011,
		0b00000};

	uint8_t special2[8] = {
		0b11000,
		0b11000,
		0b00110,
		0b01001,
		0b01000,
		0b01001,
		0b00110,
		0b00000};

	uint8_t m_dpFunction;
	uint8_t m_dpControl;
	uint8_t m_dpMode;
	uint8_t m_dpRows;
	uint8_t m_dpBacklight;

	void SendCommand(uint8_t);
	void SendChar(uint8_t);
	void Send(uint8_t, uint8_t);
	void Write4Bits(uint8_t);
	void ExpanderWrite(uint8_t);
	void PulseEnable(uint8_t);
	void DelayInit(void);
	void DelayUS(uint32_t);
};

#endif /* LCD_I2C_H_ */
