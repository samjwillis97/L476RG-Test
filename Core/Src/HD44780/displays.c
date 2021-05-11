/*
 * displays.c
 *
 *  Created on: May 5, 2021
 *      Author: sam
 */


#include "displays.h"
#include "lcd.h"
#include "main.h"

/************************************** Variables **************************************/

uint8_t no_btn_timout = 30; // s


/************************************** Menu Variables **************************************/
uint8_t menu_type;

// Temporary Variables for pointers
uint8_t u8_temp = 0;

// U8 Pointers
uint8_t *pu8_1;

// U8 Previous
uint8_t u8_1_prev = u8_temp;

/************************************** Menus **************************************/

// Type 0 Menus

void HomeMenu(){
	menu_type = 0;

	*pu8_1 = &counter;
}

/************************************** Function definitions **************************************/
DisplayProcTypeDef Display_init(
		uint8_t *btn_flag,
		Lcd_HandleTypeDef *lcd)
{
	DisplayProcTypeDef display;

	// Pointer to BTN Flag
	display.btn_flag = btn_flag;

	// Pointer to LCD
	display.lcd = lcd;

	// Init Home Screen

	return display;
}

void Display_update(DisplayProcTypeDef *display) {
	// Button Checker
	if (*display->btn_flag == 1) {
		pu8_1 = &counter;
		*pu8_1 = 0;
//		Lcd_cursor(display->lcd, 1, 0);
//		Lcd_string(display->lcd, "1");
	}

	// Update Display
	if (var_changed()){
		// Update Display
	}

	*display->btn_flag = 0;
}


/************************************** Static function definition **************************************/

bool var_changed(){
	if (*pu8_1 != u8_1_prev){
		u8_1_prev = *pu8_1;
		return true;
	}
}

