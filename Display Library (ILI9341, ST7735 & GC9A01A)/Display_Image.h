/*
 * Display_Image.h
 *
 * Created: Thu Nov 25 2021 13:15:17
 *  Author Chris
 */
#ifndef DISPLAY_IMAGE_H_
#define DISPLAY_IMAGE_H_

// ============================================================================================
// Includes
#include <stdint.h>

#include "Display_Config.h"
#include "Display_Objects_Datatypes.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Image_Init(Display_Color transparent_color);

void Display_Image_Draw_Color(int16_t x, int16_t y, Image_Color* image);
void Display_Image_Draw_Alpha(int16_t x, int16_t y, Image_Alpha* image, Display_Color color);

uint16_t Display_Image_Get_Width(Image_Color* image);
uint16_t Display_Image_Get_Height(Image_Color* image);

#endif /* DISPLAY_IMAGE_H_ */
