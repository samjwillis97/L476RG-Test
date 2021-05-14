/*
 * displays.c
 *
 *  Created on: May 5, 2021
 *      Author: sam
 */


#include "displays.h"
#include "lcd.h"
#include "main.h"

/************************************** Static function declarations **************************************/
bool has_var_changed();
void reset_vars();

void reset_counter();

bool history_isempty();
bool history_isfull();
pfn history_pop();
void history_push(pfn fnc);

void HomeMenu();
void CounterSettings();
void ResetCounter();


/************************************** GLOBALS **************************************/

#define LCD_STRING_MAX 9


/************************************** Variables **************************************/

uint8_t no_btn_timeout = 10; // s, if greater than 0 use to return home, clear history
uint32_t last_btn;

uint8_t HISTORY_MAX_SIZE = 10;
pfn history[10];
uint8_t history_top = -1;


/************************************** Menu Variables **************************************/
uint8_t menu_type;

// Menu Function Pointers
pfn pHomeMenu;
pfn pCurrentMenu;
pfn GoTo;

bool new_menu = false;

// Void Function Pointers;
pfn pfnc_1;
pfn pfnc_2;

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


/************************************** Menus **************************************/
// 	TYPE 0: Simple String and U16 Display
//
//		****************
//	1   str_1               3
//	2             pu16_1    4
//		****************
//
void HomeMenu() {
	// Every Menu Must Have, once pointers make this a function
	reset_vars();
	new_menu = true;

	// Setting Current State
	pCurrentMenu = HomeMenu;
	menu_type = 0;

	// Setting Required Pointers
	pu16_1 = &counter;

	// Functions
	// pfnc_1 always goes to a settings menu
	pfnc_1 = CounterSettings;

	// Setting Required Strings
	strcpy(str_1, "Counter:");
}

// 	TYPE 1: Three Sub Menu's
//
//		****************
//	1   str_1      str_3    3
//	2   str_2      str_4    4
//		****************
//
void CounterSettings() {
	// Every Menu Must Have, once pointers make this a function
	reset_vars();
	new_menu = true;

	// Setting Current State
	pCurrentMenu = CounterSettings;
	menu_type = 1;

	// Setting Required Pointers
	// btn 1 will go back
	// pfnc_2 -> 4 always go to another setting screen
	pfnc_2 = ResetCounter;

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
// 	Both Buttons 3 and 4, return to pfnc_1
//
void ResetCounter() {
	// Every Menu Must Have, once pointers make this a function
	reset_vars();
	new_menu = true;

	// Setting Current State
	pCurrentMenu = ResetCounter;
	menu_type = 2;

	// Setting Required Pointers
	// pfnc_1 is run if yes is pressed
	pfnc_1 = reset_counter;

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
	pHomeMenu = HomeMenu;
	HomeMenu();

	return display;
}

void Display_update(DisplayProcTypeDef *display) {

	// Button Checker - Make Better, add timeout to return home
	if (*display->btn_flag != 0) {
		last_btn = HAL_GetTick();
		switch (menu_type) {
			case 0:
				if (*display->btn_flag == 1) {
					history_push(pCurrentMenu);
					pfnc_1();
				}

				break;
			case 1:
				if (*display->btn_flag == 1) {
					GoTo = history_pop();
					GoTo();

				}
				if (*display->btn_flag == 2) {
					history_push(pCurrentMenu);
					pfnc_2();
				}

				break;
			case 2:
				// Back, Do Nothing
				if (*display->btn_flag == 1) {
					GoTo = history_pop();
					GoTo();
				}
				// No
				if (*display->btn_flag == 4) {
					GoTo = history_pop();
					GoTo();
				}
				// Yes
				if (*display->btn_flag == 2) {
					pfnc_1(); // Do This Better, Callback? Sub Routine or something?
					GoTo = history_pop();
					GoTo();
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

	if (pCurrentMenu != pHomeMenu) {
		if ((no_btn_timeout) > 0 && ((HAL_GetTick() - last_btn) > (no_btn_timeout * 1000))) {
			// ONLY IF NOT ON HOME SCREEN
			history_top = -1; 	// essentially clears history
			pHomeMenu();
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
	// Func Pointers
	pfnc_1 = NULL;
	pfnc_2 = NULL;

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

void reset_counter () {
	counter = 0;
}

// https://www.tutorialspoint.com/data_structures_algorithms/stack_program_in_c.htm

bool history_isempty() {
	if (history_top == -1) {
		return true;
	} else {
		return false;
	}
}


bool history_isfull() {
	if (history_top == HISTORY_MAX_SIZE) {
		return true;
	} else {
		return false;
	}
}


pfn history_peek() {
   return history[history_top];
}


pfn history_pop() {
	pfn data;

	if (history_isempty() == false) {
		data = history[history_top];
		history_top = history_top - 1;
		return data;
	}
	return pHomeMenu;
}


void history_push(pfn fnc) {
	if (history_isfull() == false) {
		history_top = history_top + 1;
		history[history_top] = fnc;
	}
}
