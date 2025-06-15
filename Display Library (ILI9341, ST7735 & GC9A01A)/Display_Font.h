/*
 * Display_Font.h
 *
 * Created: Mon Jul 19 2021 17:46:43
 *  Author Chris
 */
#ifndef DISPLAY_FONT_H_
#define DISPLAY_FONT_H_

// ============================================================================================
// Includes
#include <stdint.h>

#include "Display_Config.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Font_Init				(void);
void Display_Font_Set_Font			(const unsigned char* font);
uint Display_Font_Get_Font_Height	(void);
uint Display_Font_Width_Char		(char c);
uint Display_Font_Width_String		(char string[], uint8_t string_length, uint8_t char_spacing);
uint Display_Font_Print_Char		(int16_t x, int16_t y, char c, Display_Color color);
void Display_Font_Print_String		(int16_t x, int16_t y, char* string, uint8_t string_length, uint8_t char_spacing, Display_Color color);


#endif /* DISPLAY_FONT_H_ */
