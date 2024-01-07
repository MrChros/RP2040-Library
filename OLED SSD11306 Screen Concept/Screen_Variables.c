/*
 * Screen_Variables.c
 *
 * Created: Fri Apr 02 2021 13:46:29
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "Screens.h"
#include "pico/types.h"


// ============================================================================================
// Variables
void (*_Screen_Tick)(void);
void (*_Screen_Action_CW)(void);
void (*_Screen_Action_CCW)(void);
void (*_Screen_Action_Click)(void);