/*
 * lcd.h
 * adapted from
 * https://github.com/4ilo/HD44780-Stm32HAL
 *
 *  Created on: May 3, 2021
 *      Author: sam
 */

#ifndef DUINOTECH_LCD_LCD_H_
#define DUINOTECH_LCD_LCD_H_

#include "stm32l4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "main.h"

// #define LCD20xN
#define LCD16xN		// Using 16xN LCD

// For row start addresses??
extern const uint8_t ROW_16[];		// extern extends the visibility of the variable
extern const uint8_t ROW_20[];


/************************************** Command register **************************************/
// 1. Display Clear
// 2. Function Set:
//		DL = 1; 8-bit interface data
//		N = 0; 1-line display
//		F = 0; 8 dot character font
// 3. Display On/Off Control:"
//		D = 0; Display Off
//		C = 0; Cursor Off
//		B = 0; Blinking Off
// 4. Entry Mode Set:
//		I/D = 1; increment by one
//		S = 0; No shift
// Instructions, where
// RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0

#define CLEAR_DISPLAY	0x01		// 0 0 0 0 0 0 0 0 0 1

#define RETURN_HOME		0x02		// 0 0 0 0 0 0 0 0 1 -

#define ENTRY_MODE_SET	0x04		// 0 0 0 0 0 0 0 1 I/D S
#define OPT_S	0x01
#define OPT_INC	0x02

#define DISPLAY_ON_OFF_CONTROL	0x08	// 0 0 0 0 0 0 1 D C B
#define OPT_D	0x04				// Turn on Display
#define OPT_C	0x02				// Turn on Cursor
#define OPT_B	0x01				// Turn on Cursor Blink

#define CURSOR_DISPLAY_SHIFT	0x10	// 0 0 0 0 0 1 S/C R/L - -
#define OPT_SC	0x08
#define OPT_RL 0x04


#define FUNCTION_SET 0x20
#define OPT_DL 0x10						// Set interface data length
#define OPT_N 0x08						// Set number of display lines
#define OPT_F 0x04						// Set alternate font
#define SETCGRAM_ADDR 0x040
#define SET_DDRAM_ADDR 0x80				// Set DDRAM address


/************************************** Helper macros **************************************/
#define DELAY(X) HAL_Delay(X)


/************************************** LCD Defines **************************************/
#define LCD_NIB 4			// ?
#define LCD_BYTE 8			// ?
#define LCD_DATA_REG 1		// ?
#define LCD_COMMAND_REG 0	// ?


/************************************** LCD typedefs **************************************/
#define Lcd_PortType GPIO_TypeDef* // <-- Pointer??
#define Lcd_PinType uint16_t

// ?
typedef enum {
	LCD_4_BIT_MODE,
	LCD_8_BIT_MODE
} Lcd_ModeTypeDef;


// ?
typedef struct {
	Lcd_PortType *data_port;	// <-- Pointer
	Lcd_PinType *data_pin;		// <-- Pointer

	Lcd_PortType rs_port;
	Lcd_PinType rs_pin;

	Lcd_PortType en_port;
	Lcd_PinType en_pin;

	Lcd_ModeTypeDef mode;

} Lcd_HandleTypeDef;


/************************************** Public Functions **************************************/
void Lcd_init(Lcd_HandleTypeDef *lcd);
void Lcd_int(Lcd_HandleTypeDef *lcd, int number);
void Lcd_string(Lcd_HandleTypeDef *lcd, char *string);
void Lcd_cursor(Lcd_HandleTypeDef *lcd, uint8_t row, uint8_t col);
Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin, Lcd_ModeTypeDef mode);
void Lcd_define_char(Lcd_HandleTypeDef *lcd, uint8_t code, uint8_t bitmap[]);
void Lcd_clear(Lcd_HandleTypeDef *lcd);

#endif /* DUINOTECH_LCD_LCD_H_ */
