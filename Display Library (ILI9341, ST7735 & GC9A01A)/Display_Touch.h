/*
 * Display_Touch.h
 *
 * Created: Mon Jun 05 2023 2:38:44 PM
 *  Author: MrChros
 */
#ifndef DISPLAY_TOUCH_H_
#define DISPLAY_TOUCH_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/types.h"

#include "Display_Objects_Datatypes.h"


// ============================================================================================
// Defines
#define DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT			0
#define DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT		1


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Touch_Init();
void Display_Touch_Set_Reference_Point(uint point, uint x_touch, uint y_touch);
void Display_Touch_Detect(uint average_of_measurements);

int Display_Touch_Get_Reference_Point_Count();
Coordinates* Display_Touch_Get_Reference_Point(int point_number);

uint Display_Touch_Get_Marker_Count();
Coordinates* Display_Touch_Get_Marker_Coordinates(uint index);

uint Display_Touch_Get_Last_X();
uint Display_Touch_Get_Last_Y();
uint Display_Touch_Get_Last_Z1();
uint Display_Touch_Get_Last_Z2();
float Display_Touch_Get_Last_Pressure();


#endif /* DISPLAY_TOUCH_H_ */
