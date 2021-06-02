/*
 * lcd.c
 * adapted from
 * https://github.com/4ilo/HD44780-Stm32HAL
 *
 *  Created on: May 3, 2021
 *      Author: sam
 */


#include "lcd.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};


/************************************** Static declarations **************************************/
static void lcd_write_data(Lcd_HandleTypeDef *lcd, uint8_t data);
static void lcd_write_command(Lcd_HandleTypeDef *lcd, uint8_t command);
static void lcd_write_nib_command(Lcd_HandleTypeDef *lcd, uint8_t command);
static void lcd_write(Lcd_HandleTypeDef *lcd, uint8_t data, uint8_t len);


/************************************** Function definitions **************************************/
/**
 * Create new Lcd_HandleTypeDef and Initialize the LCD
 */
Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin,
		Lcd_ModeTypeDef mode,
		bool cursor,
		bool blink)
{

	Lcd_HandleTypeDef lcd;

	lcd.mode = mode;

	lcd.en_pin = en_pin;
	lcd.en_port = en_port;

	lcd.rs_pin = rs_pin;
	lcd.rs_port = rs_port;

	lcd.data_pin = pin;
	lcd.data_port = port;

	lcd.cursor = cursor;
	lcd.blink = blink;

	Lcd_init(&lcd);		// Some sort of pointer to the lcd type def just init

	return lcd;
}

/**
 * Init 16 x 2 LCD without cursor
 */
void Lcd_init(Lcd_HandleTypeDef *lcd) {
	if (lcd->mode == LCD_4_BIT_MODE)	// gets mode from the struct that lcd points to
	{
		HAL_Delay(50);
		lcd_write_nib_command(lcd, 0x30);		// Func Set
		HAL_Delay(5);
		lcd_write_nib_command(lcd, 0x30);		// Func Set
		HAL_Delay(1);
		lcd_write_nib_command(lcd, 0x30);		// Func Set

		lcd_write_nib_command(lcd, FUNCTION_SET);			// Func Set 4-Bit
		lcd_write_command(lcd, FUNCTION_SET | OPT_N);	// Func Set 4-Bit Mode, 2 Lines

		lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL);	// Display OFF
	}
	else {
		lcd_write_command(lcd, FUNCTION_SET | OPT_DL | OPT_N); // ?
	}

	lcd_write_command(lcd, CLEAR_DISPLAY);						// Clear Display
	lcd_write_command(lcd, ENTRY_MODE_SET | OPT_INC);			// Increment Cursor?

	// Checking for Cursor and Blink, could change and use a variable
	if ((lcd->blink == true) && (lcd->cursor == true)) {
		lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
	}
	else if ((lcd->blink == true) && (lcd->cursor == false)) {
		lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_B);
	}
	else if ((lcd->blink == false) && (lcd->cursor == true)) {
		lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C);
	}
	else {
		lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);
	}
}

/**
 * Write a number on the current position
 */
void Lcd_int(Lcd_HandleTypeDef *lcd, int number) {
	char buffer[11];
	sprintf(buffer, "%d", number);

	Lcd_string(lcd, buffer);
}

void Lcd_u8_right_aligned(Lcd_HandleTypeDef *lcd, uint8_t number, uint8_t row, uint8_t col) {
	char buffer[4];
	char input[5];
	uint8_t i;

	uint8_t len = snprintf(NULL, 0, "%d", number);

	for (i = 0; i < (3 - len); i++) {
		input[i] = 32;	// space
	}
	input[i] = 37;	// percentage
	input[i+1] = 100;	// d
	input[i+2] = '\0';	// end of string

	sprintf(buffer, input, number);

	Lcd_cursor(lcd, row, (col - 2));
	Lcd_string(lcd, buffer);
}


void Lcd_u16_left_aligned(Lcd_HandleTypeDef *lcd, uint16_t number, uint8_t row, uint8_t col) {
	char buffer[6];
	char input[7];
	uint8_t i;

	uint8_t len = snprintf(NULL, 0, "%d", number);

	input[0] = 37;	// percentage
	input[1] = 100;	// d

	for (i = 0; i < (7 - len); i++) {
		input[i+2] = 32;	// space
	}

	input[i] = '\0';	// end of string

	sprintf(buffer, input, number);

	Lcd_cursor(lcd, row, col);
	Lcd_string(lcd, buffer);
}


void Lcd_u16_right_aligned(Lcd_HandleTypeDef *lcd, uint16_t number, uint8_t row, uint8_t col) {
	char buffer[6];
	char input[7];
	uint8_t i;

	uint8_t len = snprintf(NULL, 0, "%d", number);

	for (i = 0; i < (5 - len); i++) {
		input[i] = 32;	// space
	}
	input[i] = 37;	// percentage
	input[i+1] = 100;	// d
	input[i+2] = '\0';	// end of string

	sprintf(buffer, input, number);

	Lcd_cursor(lcd, row, (col - 4));
	Lcd_string(lcd, buffer);
}

/**
 * Write String in current cursor position
 */
void Lcd_string(Lcd_HandleTypeDef *lcd, char *string) {
	for (uint8_t i = 0; i < strlen(string); i++) {
		lcd_write_data(lcd, string[i]);
	}
}

void Lcd_string_centered (Lcd_HandleTypeDef *lcd, char *string, uint8_t row) {
	uint8_t len = strlen(string) - 1;
	uint8_t left_spacing = (15 - len) / 2;
	Lcd_cursor(lcd, row, left_spacing);
	Lcd_string(lcd, string);
}

void Lcd_string_right_aligned (Lcd_HandleTypeDef *lcd, char *string, uint8_t row, uint8_t col) {
	uint8_t len = strlen(string) - 1;
	Lcd_cursor(lcd, row, (col - len));
	Lcd_string(lcd, string);
}
/**
 * Set the cursor position
 */
void Lcd_cursor(Lcd_HandleTypeDef *lcd, uint8_t row, uint8_t col) {

	#ifdef LCD20xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_20[row] + col);
	#endif

	#ifdef LCD16xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_16[row] + col);
	#endif
}

/**
 * Clear the screen
 */
void Lcd_clear(Lcd_HandleTypeDef *lcd) {
	lcd_write_command(lcd, CLEAR_DISPLAY);
}

/**
 * ??
 */
void Lcd_define_char(Lcd_HandleTypeDef *lcd, uint8_t code, uint8_t bitmap[]){
	lcd_write_command(lcd, SETCGRAM_ADDR + (code << 3));
	for(uint8_t i=0;i<8;++i){
		lcd_write_data(lcd, bitmap[i]);
	}

}

/**
 * Toggle the Cursor
 */
void Lcd_toggle_cursor(Lcd_HandleTypeDef *lcd) {
	if (lcd->cursor == true) {
		if (lcd->blink == true) {
			// Turn off cursor, keeping blink on
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_B);
		}
		else {
			// Turn off cursor, keeping blink off
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D );
		}
		lcd->cursor = false;
	}
	else if (lcd->cursor == false) {
		if (lcd->blink == true) {
			// Turn on cursor, keeping blink on
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_B | OPT_C);
		}
		else {
			// Turn on cursor, keeping blink off
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C);
		}
		lcd->cursor = true;
	}
}

/**
 * Toggle Blinking
 */
void Lcd_toggle_blink(Lcd_HandleTypeDef *lcd) {
	if (lcd->blink == true) {
		if (lcd->cursor == true) {
			// Turn off blink, keeping cursor on
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C);
		}
		else {
			// Turn off blink, keeping cursor off
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);
		}
		lcd->blink = false;
	}
	else if (lcd->blink == false) {
		if (lcd->cursor == true) {
			// Turn on blink, keeping cursor on
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_C | OPT_B);
		}
		else {
			// Turn on blink, keeping cursor off
			lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D | OPT_B);
		}
		lcd->blink = true;
	}
}


/************************************** Static function definition **************************************/
/**
 * Write a byte to the command register
 */
void lcd_write_command(Lcd_HandleTypeDef *lcd, uint8_t command) {
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);	// Write to Command Register

	if (lcd->mode == LCD_4_BIT_MODE) {
		// Writing Command in two seperate nibs due to 4 Bit Mode
		lcd_write(lcd, (command >> 4), LCD_NIB);	// Send 4 MSB
		lcd_write(lcd, command & 0x0F, LCD_NIB);	// Send 4 LSB
	}
	else {
		// Write Command in one go with 8 bit
		lcd_write(lcd, command, LCD_BYTE);
	}
}

/**
 * Write a byte to the data register
 */
void lcd_write_data(Lcd_HandleTypeDef *lcd, uint8_t data) {
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_DATA_REG);			// Write to data register

	// Writing Command in two seperate nibs due to 4 Bit Mode
	if(lcd->mode == LCD_4_BIT_MODE) {
		lcd_write(lcd, data >> 4, LCD_NIB);		// Send 4 MSB
		lcd_write(lcd, data & 0x0F, LCD_NIB);	// Send 4 LSB
	}
	else {
		// Write Command in one go with 8 bit
		lcd_write(lcd, data, LCD_BYTE);
	}
}


/**
 * Set len bits on the bus toggle the enable line
 */
void lcd_write(Lcd_HandleTypeDef *lcd, uint8_t data, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		// Right shifts data, then gets the least significant bit to write
		HAL_GPIO_WritePin(lcd->data_port[i], lcd->data_pin[i], (data >> i) & 0x01);
	}

	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 0); 		// Data receive on falling edge
}

/**
 * Writes a single 4-bit (NIB) command using the MSB of the uint
 */
void lcd_write_nib_command(Lcd_HandleTypeDef *lcd, uint8_t command) {

	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);	// Write to Command Register

	if (lcd->mode == LCD_4_BIT_MODE) {
		// Writing Command in two seperate nibs due to 4 Bit Mode
		lcd_write(lcd, (command >> 4), LCD_NIB);	// Send 4 MSB
	}
	else {
		// Do Nothing
	}
}

