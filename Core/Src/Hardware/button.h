/*
 * button.h
 *
 *  Created on: May 9, 2021
 *      Author: sam
 */

#ifndef SRC_HARDWARE_BUTTON_H_
#define SRC_HARDWARE_BUTTON_H_

#include "stm32l4xx_hal.h"


/************************************** Display typedefs **************************************/
typedef enum {
	NO_BUTTON,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4
} ButtonIDTypeDef;


typedef enum {
	NONE,
	PRESSED,
	RELEASED,
} ButtonStateTypeDef;


typedef struct {
	ButtonIDTypeDef button;
	ButtonStateTypeDef state;
	uint8_t time;
} ButtonTypeDef;


typedef struct {
	uint16_t btn_pin;
} ButtonMapKeyTypeDef;


typedef struct {
	ButtonMapKeyTypeDef key;
	ButtonIDTypeDef value;
} ButtonMapTypeDef;

/************************************** Public Functions **************************************/


#endif /* SRC_HARDWARE_BUTTON_H_ */
