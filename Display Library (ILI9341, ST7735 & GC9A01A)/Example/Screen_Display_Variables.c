/*
 * Screen_Variables.c
 *
 * Created: Fri Apr 02 2021 13:46:29
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "Screens_Display.h"
#include "pico/types.h"


// ============================================================================================
// Variables
void (*_Screen_Display_Tick)(void);
void (*_Screen_Display_Click)(uint button_return_value);
void (*_Screen_Display_Touch_Event)(int16_t x, int16_t y);
void (*_Screen_Display_Action_CW)(Object_ID object_id);
void (*_Screen_Display_Action_CCW)(Object_ID object_id);
void (*_Screen_Display_On_Objects_Focused)(Object_ID object_id);
void (*_Screen_Display_On_Objects_Defocused)(Object_ID object_id);
void (*_Screen_Display_On_Object_Select)(Object_ID object_id);
void (*_Screen_Display_On_Object_Deselect)(Object_ID object_id);