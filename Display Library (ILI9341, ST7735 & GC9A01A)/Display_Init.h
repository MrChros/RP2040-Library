/*
 * Display_Init.h
 *
 * Created: Sun Mar 21 2021 12:51:58
 *  Author Chris
 */
#ifndef DISPLAY_INIT_H_
#define DISPLAY_INIT_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/types.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Init_GPIOs(void);
void Display_Init_Reset(void);
void Display_Init_WakeUp(void);
void Display_Init_Registers(void);


#endif /* DISPLAY_INIT_H_ */