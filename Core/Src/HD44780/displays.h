/*
 * HD44780_Displays.h
 *
 *  Created on: May 5, 2021
 *      Author: sam
 */

#ifndef SRC_HD44780_DISPLAYS_H_
#define SRC_HD44780_DISPLAYS_H_

#include "lcd.h"


/************************************** Home Menu typedefs **************************************/
typedef struct {
	Lcd_HandleTypeDef *lcd;

	float *distance;

} Home_menuTypeDef;


/************************************** Public Functions **************************************/


#endif /* SRC_HD44780_DISPLAYS_H_ */
