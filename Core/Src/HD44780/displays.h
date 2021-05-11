/*
 * HD44780_Displays.h
 *
 *  Created on: May 5, 2021
 *      Author: sam
 */

#ifndef SRC_HD44780_DISPLAYS_H_
#define SRC_HD44780_DISPLAYS_H_

#include "stm32l4xx_hal.h"
#include "lcd.h"
#include "main.h"



/************************************** Variables **************************************/

// General


/************************************** Display typedefs **************************************/
//typedef void (*pfn)(void);

typedef struct {
	// Pointer to Button Flag for presses
	uint8_t *btn_flag;

	// Pointer to LCD
	Lcd_HandleTypeDef *lcd;

}DisplayProcTypeDef;


/************************************** Public Functions **************************************/
// Function to Action on Buttons
DisplayProcTypeDef Display_init(
		uint8_t *btn_flag,
		Lcd_HandleTypeDef *lcd);
void Display_update(
		DisplayProcTypeDef *display);

#endif /* SRC_HD44780_DISPLAYS_H_ */
