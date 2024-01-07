/*
 * Screens.h
 *
 * Created: Fri Apr 02 2021 13:46:52
 *  Author Chris
 */

#ifndef SCREENS_H_
#define SCREENS_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// ============================================================================================
// Defines


// ============================================================================================
// Variables
extern void (*_Screen_Tick)(void);
extern void (*_Screen_Action_CW)(void);
extern void (*_Screen_Action_CCW)(void);
extern void (*_Screen_Action_Click)(void);


// ============================================================================================
// List of all Screen Setups as extern
extern void Screen_Setup_0(uint transition_animation);
extern void Screen_Setup_1(uint transition_animation);


#endif /* SCREENS_H_ */