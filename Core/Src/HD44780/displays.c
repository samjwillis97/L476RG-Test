/*
 * displays.c
 *
 *  Created on: May 5, 2021
 *      Author: sam
 */


#include "displays.h"
#include "lcd.h"
#include "main.h"

/************************************** GLOBALS **************************************/

#define LCD_STRING_MAX 9


/************************************** Variables **************************************/

uint8_t no_btn_timout = 30; // s


/************************************** Menu Variables **************************************/
char menu_name[LCD_STRING_MAX];
char prev_menu_name[LCD_STRING_MAX];

uint8_t menu_type;

bool new_menu = false;

// U8 Pointers
uint8_t *pu8_1;

// U16 Pointers
uint16_t *pu16_1;

// Strings
char str_1[LCD_STRING_MAX];
char str_2[LCD_STRING_MAX];
char str_3[LCD_STRING_MAX];
char str_4[LCD_STRING_MAX];

// U8 Previous
uint8_t u8_1_prev;

// U16 Previous
uint16_t u16_1_prev;

/************************************** Static function declarations **************************************/
bool has_var_changed();
void reset_vars();

void HomeMenu();
void CounterSettings();
void ResetCounter();

/************************************** Menus **************************************/
// 	TYPE 0: Simple String and U16 Display
//
//		****************
//	1   str_1               3
//	2             pu16_1    4
//		****************
//
void HomeMenu() {
	// Every Menu Must Have
	reset_vars();
	new_menu = true;

	// Setting Current State
	menu_type = 0;
	strcpy(menu_name, "Home");

	// Setting Required Pointers
	pu16_1 = &counter;

	// Setting Required Strings
	strcpy(str_1, "U16 Counter:");
}

// 	TYPE 1: Three Sub Menu's
//
//		****************
//	1   str_1      str_3    3
//	2   str_2      str_4    4
//		****************
//
void CounterSettings() {
	// Every Menu Must Have
	reset_vars();
	new_menu = true;

	// Setting Current State
	menu_type = 1;
	strcpy(menu_name, "CountSet");

	// Setting Required Pointers

	// Setting Required Strings
	strcpy(str_1, "Counter");
	strcpy(str_2, "Reset");
	strcpy(str_3, "Next");
	strcpy(str_4, "Modify");
}

// 	TYPE 2: Are You Sure?
//
//		****************
//	1    Are You Sure?      3
//	2   Yes           No    4
//		****************
//
void ResetCounter() {
	// Every Menu Must Have
	reset_vars();
	new_menu = true;

	// Setting Current State
	menu_type = 2;
	strcpy(menu_name, "RstCount");

	// Setting Required Pointers
	pu16_1 = &counter;

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

	// Initialize as Home Screen
	HomeMenu();

	return display;
}

void Display_update(DisplayProcTypeDef *display) {

	// Button Checker - Make Better, add timeout to return home
	if (*display->btn_flag != 0) {
		switch (menu_type) {
			case 0:
				if (*display->btn_flag == 1) {
					CounterSettings();		// Change to POINTER
				}

				break;
			case 1:
				if (*display->btn_flag == 1) {
					HomeMenu();		// Change to POINTER (PREVIOUS MENU)
				}
				if (*display->btn_flag == 2) {
					ResetCounter();	// Change to POINTER
				}

				break;
			case 2:
				// No
				if (*display->btn_flag == 4) {
					CounterSettings(); // Change to Pointer (PREVIOUS MENU)
				}
				// Yes
				if (*display->btn_flag == 2) {
					*pu16_1 = 0;
					CounterSettings(); // Change to Pointer (PREVIOUS MENU)
				}

				break;
			default:
				break;
		}
		*display->btn_flag = 0;
	}


	// Update Display (New Menu)
	if (new_menu == true){
		// Update Display
		Lcd_clear(display->lcd);
		switch (menu_type) {
			case 0:
				// Writing Initial Display for Testing
				Lcd_cursor(display->lcd, 0, 0);
				Lcd_string(display->lcd, str_1);

				break;
			case 1:
				Lcd_cursor(display->lcd, 0, 0);
				Lcd_string(display->lcd, str_1);
				Lcd_cursor(display->lcd, 1, 0);
				Lcd_string(display->lcd, str_2);
				Lcd_string_right_aligned(display->lcd, str_3, 0, 15);
				Lcd_string_right_aligned(display->lcd, str_4, 1, 15);

				break;
			case 2:
				Lcd_string_centered(display->lcd, "Are You Sure?", 0);
				Lcd_cursor(display->lcd, 1, 0);
				Lcd_string(display->lcd, "Yes");
				Lcd_string_right_aligned(display->lcd, "No", 1, 15);

				break;
			default:
				// do nothing
				break;
		}
		// set new_menu false once handled
		new_menu = false;
	}


	// Update Display (Continuous)
	if (has_var_changed() == true){
		// Update Display
		switch (menu_type) {
			case 0:
				Lcd_u16_right_aligned(display->lcd, *pu16_1, 1, 15);
				break;
			default:
				// do nothing
				break;
		}
	}

}


/************************************** Static function definition **************************************/

bool has_var_changed() {
	if (*pu8_1 != u8_1_prev) {
		u8_1_prev = *pu8_1;
		return true;
	}
	if (*pu16_1 != u16_1_prev) {
		u16_1_prev = *pu16_1;
		return true;
	}
	return false;
}

void reset_vars() {
	// U8 Pointers
	pu8_1 = NULL;

	// U16 Pointers
	pu16_1 = NULL;

	// Strings
	strcpy(str_1, "");
	strcpy(str_2, "");
	strcpy(str_3, "");
	strcpy(str_4, "");

}

