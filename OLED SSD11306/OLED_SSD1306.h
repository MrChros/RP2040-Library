/*
 * OLED.h
 *
 * Created: 22/01/2020 18:36:25
 *  Author: Chris
 */ 
#ifndef OLED_SSD1306_H_
#define OLED_SSD1306_H_

// ============================================================================================
// Includes
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "pico/types.h"
#include "pico/stdlib.h"


// ============================================================================================
// Defines
#define SSD1306_TEXT(_TEXT_)		_TEXT_, sizeof(_TEXT_)-1

#define SSD1306_HEIGHT_32PX			0b000000
#define SSD1306_HEIGHT_64PX			0b100000

#define SSD1306_HEIGHT				SSD1306_HEIGHT_64PX

#ifndef SSD1306_HEIGHT
#error SSD1306: Height for display has not been defined
#endif

#define ANIMATION_NONE				0
#define ANIMATION_DIRECTION_LEFT	1
#define ANIMATION_DIRECTION_RIGHT	2
#define ANIMATION_DIRECTION_UP		3
#define ANIMATION_DIRECTION_DOWN	4


// ============================================================================================
// Function Declarations
void SSD1306_Init				();
bool SSD1306_Get_Update_Pending	();
void SSD1306_Transmit_Buffer	();
void SSD1306_Transmit_Page		(uint8_t start_column, uint8_t length, uint8_t page_start);
bool SSD1306_Transmit_Completed	();

void SSD1306_Animation_Start	(uint direction);
void SSD1306_Animation_Tick		();

void SSD1306_Set_Font			(const uint8_t* font);
uint SSD1306_Get_Font_Width		(void);
void SSD1306_Set_Invert_Enabled	(bool invert_enabled);
uint SSD1306_Get_Width			(void);
uint16_t SSD1306_Get_Display_Data(int16_t page, int16_t x);

void SSD1306_Clear				(bool skip_while_animation);
void SSD1306_Fill_Pattern		(int16_t x, int16_t y, uint length, uint8_t pattern);
void SSD1306_Fill_Array			(int16_t x, int16_t y, uint length, uint8_t* array);

void SSD1306_Write_Text			(int16_t x, int16_t y, char* text, uint8_t length);
void SSD1306_Write_Text_Centered(int16_t y, char* text, uint8_t length);
void SSD1306_Write_Int_Dec		(int16_t x, int16_t y, int32_t val, uint8_t numbers, bool display_signed, bool skip_leading_zero);
void SSD1306_Write_Int_Hex		(int16_t x, int16_t y, uint val, uint8_t numbers, uint8_t write_0x);
void SSD1306_Write_Unit			(int16_t x, int16_t y, uint8_t numbers, uint8_t extra_space, char* text, uint8_t length);

void SSD1306_Draw_Bitmap		(int16_t x, int16_t y, const uint8_t* bitmap);
void SSD1306_Draw_Bitmap_Centered(int16_t y, const uint8_t* bitmap);

void SSD1306_Draw_Point			(int16_t x, int16_t y);
void SSD1306_Draw_HLine			(int16_t x, int16_t y, uint16_t width, uint16_t thickness);
void SSD1306_Draw_VLine			(int16_t x, int16_t y, uint16_t height, uint16_t thickness);
void SSD1306_Draw_Line			(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t thickness);
void SSD1306_Draw_Rect_Frame	(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t thickness);
void SSD1306_Draw_Rect_Filled	(int16_t x, int16_t y, uint16_t width, uint16_t height);
void SSD1306_Draw_Circle_Frame	(int16_t x, int16_t y, uint16_t radius, uint16_t thickness);
void SSD1306_Draw_Circle_Filled	(int16_t x, int16_t y, uint16_t radius);

#endif // OLED_SSD1306_H_