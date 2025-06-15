/*
 * Display_Shapes.h
 *
 * Created: Mon Jul 19 2021 17:55:37
 *  Author Chris
 */
#ifndef DISPLAY_SHAPES_H_
#define DISPLAY_SHAPES_H_

// ============================================================================================
// Includes
#include "Display_Config.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Shapes_Init					(Display_Image_Buffer* image_buffer);
void Display_Shapes_Fill_Screen				(Display_Color color);
void Display_Shapes_Draw_Pixel				(int16_t x, int16_t y, Display_Color color);
void Display_Shapes_Draw_HLine				(int16_t x, int16_t y, uint16_t width, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_VLine				(int16_t x, int16_t y, uint16_t height, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_Line				(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_Rect_Frame			(int16_t x, int16_t y, uint16_t width, uint16_t heigh, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_Rect_Filled		(int16_t x, int16_t y, uint16_t width, uint16_t height, Display_Color color);
void Display_Shapes_Draw_Circle_Frame		(int16_t x, int16_t y, uint16_t radius, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_Circle_Filled		(int16_t x, int16_t y, uint16_t radius, Display_Color color);
void Display_Shapes_Draw_Round_Rect_Frame	(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t thickness, Display_Color color);
void Display_Shapes_Draw_Round_Rect_Filled	(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, Display_Color color);


#endif /* DISPLAY_SHAPES_H_ */
