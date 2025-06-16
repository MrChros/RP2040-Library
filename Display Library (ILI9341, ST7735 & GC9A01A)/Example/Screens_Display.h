/*
 * Screens.h
 *
 * Created: Fri Apr 02 2021 13:46:52
 *  Author Chris
 */

#ifndef SCREENS_DISPLAY_H_
#define SCREENS_DISPLAY_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Display_Objects_Datatypes.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables
extern void (*_Screen_Display_Tick)(void);
extern void (*_Screen_Display_Click)(uint button_return_value);
extern void (*_Screen_Display_Touch_Event)(int16_t x, int16_t y);
extern void (*_Screen_Display_Action_CW)(Object_ID object_id);
extern void (*_Screen_Display_Action_CCW)(Object_ID object_id);
extern void (*_Screen_Display_On_Objects_Focused)(Object_ID object_id);
extern void (*_Screen_Display_On_Objects_Defocused)(Object_ID object_id);
extern void (*_Screen_Display_On_Object_Select)(Object_ID object_id);
extern void (*_Screen_Display_On_Object_Deselect)(Object_ID object_id);


// ============================================================================================
// List of all Screen Setups as extern
extern void Screen_Display_Setup_Menu_Main();

#endif /* SCREENS_DISPLAY_H_ */