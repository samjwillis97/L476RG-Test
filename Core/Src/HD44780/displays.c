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
// Pointer Specific
bool has_var_changed();
void reset_vars();

// Variable Specific
void reset_counter();

// History Stack
bool history_isempty();
bool history_isfull();
pfn history_pop();
void history_push(pfn fnc);

// Menus
void HomeMenu();
void CounterSettings();
void ResetCounter();
void ChangeIncrement();

// Main Functions

void checkButtons();
void updateNewMenu();
void updateVarChange();

void doNothing();


/************************************** GLOBALS **************************************/

#define LCD_STRING_MAX 9

/************************************** Flags **************************************/

bool update_display = false;	// set true to make sure display updates next cycle


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
pfn pfnc_3;
pfn pfnc_4;

// U8 Pointers
uint8_t *pu8_1;

// U16 Pointers
uint16_t *pu16_1;

// U16 Variables
uint16_t u16_1;
uint16_t u16_2;

// Strings
char str_1[LCD_STRING_MAX];
char str_2[LCD_STRING_MAX];
char str_3[LCD_STRING_MAX];
char str_4[LCD_STRING_MAX];

// U8 Previous
uint8_t u8_1_prev;

// U16 Previous
uint16_t u16_1_prev;

// U16 Temporary Storage
uint16_t u16_1_temp;


/************************************** Menus **************************************/
// 	TYPE 0: Simple String and U16 Display
//
//		****************
//	1   str_1               3
//	2             pu16_1    4
//		****************
//
//	Button 1 goes to pfnc_1
//	Display and Updates pu16_1
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
//	Button 1 returns to Previous
//	Button 2 goes to pfnc_2
//	Button 3 goes to pfnc_3
//	Button 4 goes to pfnc_4
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
	pfnc_4 = ChangeIncrement;

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
// 	Both Buttons 3 and 4, return to previous menu
//	Button 3 Confirms + Runs pfnc_1, Returns to Previous Menu
//	Button 4 Returns to Previous Menu
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


// 	TYPE 3: U16 Change Value
//
//		****************
//	1   pu16_1    u16_1+	3
//	2   OK    	  u16_2-	4
//		****************
//
// 	Button 1 goes back and does not apply change
//	Button 2 confirms change and goes back
//  Button 3 increments by value pu16_2
// 	Button 4 decrements by value pu16_3
//
void ChangeIncrement() {
	// Every Menu Must Have, once pointers make this a function
	reset_vars();
	new_menu = true;

	// Setting Current State
	pCurrentMenu = ChangeIncrement;
	menu_type = 3;

	// Setting Required Pointers
	pu16_1 = &increment;

	// Setting Required Variables
	u16_1 = 1;
	u16_2 = 1;

}

// NEED TO ADD SOME SORT OF DECORATOR LIKE FUNCTIONS
// DO TO STUFF ON ENTRY/EXIT

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
				// Go To pfnc_1
				if (*display->btn_flag == 1) {
					history_push(pCurrentMenu);
					pfnc_1();
				}

				break;
			case 1:
				// Back
				if (*display->btn_flag == 1) {
					GoTo = history_pop();
					GoTo();

				}
				// Go To pfnc_2
				if (*display->btn_flag == 2) {
					history_push(pCurrentMenu);
					pfnc_2();
				}
				// Go To pfnc_3
				if (*display->btn_flag == 3) {
					history_push(pCurrentMenu);
					pfnc_3();
				}
				// Go To pfnc_4
				if (*display->btn_flag == 4) {
					history_push(pCurrentMenu);
					pfnc_4();
				}

				break;
			case 2:
				// Back, Do Nothing
				if (*display->btn_flag == 1) {
					GoTo = history_pop();
					GoTo();
				}
				// Yes
				if (*display->btn_flag == 2) {
					pfnc_1(); // Do This Better, Callback? Sub Routine or something?
					GoTo = history_pop();
					GoTo();
				}
				// No
				if (*display->btn_flag == 4) {
					GoTo = history_pop();
					GoTo();
				}

				break;
			case 3:
				// Back, Do Nothing
				if (*display->btn_flag == 1) {
					GoTo = history_pop();
					GoTo();
				}
				// OK, Confirm Changes
				if (*display->btn_flag == 2) {
					*pu16_1 = u16_1_temp;

					GoTo = history_pop();
					GoTo();
				}
				// Increment by U16_1
				if (*display->btn_flag == 3) {
					u16_1_temp += u16_1;
					update_display = true;
				}
				// Increment by U16_2
				if (*display->btn_flag == 4) {
					u16_1_temp -= u16_2;
				}

				break;
			default:
				break;
		}
		update_display = true;
		*display->btn_flag = 0;
	}


	// Update Display (New Menu), Do entry Actions?
	if (new_menu == true){
		// Update Display
		Lcd_clear(display->lcd);
		switch (menu_type) {
			case 0:
				// On Entry Actions

				// Writing Initial Display for Testing
				// Top Left
				Lcd_cursor(display->lcd, 0, 0);
				Lcd_string(display->lcd, str_1);
				// Lower Right
				Lcd_u16_right_aligned(display->lcd, *pu16_1, 1, 15);

				break;
			case 1:
				// On Entry Actions

				// Write Entry Display
				// Top Left
				Lcd_cursor(display->lcd, 0, 0);
				Lcd_string(display->lcd, str_1);
				// Lower Left
				Lcd_cursor(display->lcd, 1, 0);
				Lcd_string(display->lcd, str_2);
				// Top Right
				Lcd_string_right_aligned(display->lcd, str_3, 0, 15);
				// Lower Right
				Lcd_string_right_aligned(display->lcd, str_4, 1, 15);

				break;
			case 2:
				// On Entry Actions

				// Write Entry Display
				// Centered
				Lcd_string_centered(display->lcd, "Are You Sure?", 0);
				// Lower Left
				Lcd_cursor(display->lcd, 1, 0);
				Lcd_string(display->lcd, "Yes");
				// Lower Right
				Lcd_string_right_aligned(display->lcd, "No", 1, 15);

				break;
			case 3:
				// On Entry Actions
				u16_1_temp = *pu16_1;

				// Write Entry Display
				// Top Left
				Lcd_u16_left_aligned(display->lcd, *pu16_1, 0, 0);
				// Lower Left
				Lcd_cursor(display->lcd, 1, 0);
				Lcd_string(display->lcd, "Ok?");
				// Top Right
				Lcd_u16_right_aligned(display->lcd, u16_1, 0, 14);
				Lcd_cursor(display->lcd, 0, 15);
				Lcd_string(display->lcd, "+");
				// Lower Right
				Lcd_u16_right_aligned(display->lcd, u16_2, 1, 14);
				Lcd_cursor(display->lcd, 1, 15);
				Lcd_string(display->lcd, "-");

				break;
			default:
				// do nothing
				break;
		}
		// set new_menu false once handled
		new_menu = false;
	}


	if (has_var_changed() == true || update_display == true) {
		// Update Display
		update_display = false;

		switch (menu_type) {
			case 0:
				// Lower Right
				Lcd_u16_right_aligned(display->lcd, *pu16_1, 1, 15);

				break;
			case 3:
				// Top Left
				Lcd_u16_left_aligned(display->lcd, u16_1_temp, 0, 0);

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
// Main Functions

// Check Buttons and Act
void checkButtons() {

}

void doNothing() {
	;
}

// Pointer Specific Functions

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
	// Function Pointers
	pfnc_1 = doNothing;
	pfnc_2 = doNothing;
	pfnc_3 = doNothing;
	pfnc_4 = doNothing;

	// U8 Pointers
	pu8_1 = NULL;

	// U16 Pointers
	pu16_1 = NULL;

	// U16 Variables
	u16_1 = 0;
	u16_2 = 0;

	// Strings
	strcpy(str_1, "");
	strcpy(str_2, "");
	strcpy(str_3, "");
	strcpy(str_4, "");
}

// Variable Specific Functions

void reset_counter () {
	counter = 0;
}

// HISTORY STACK IMPLEMENTATION
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
