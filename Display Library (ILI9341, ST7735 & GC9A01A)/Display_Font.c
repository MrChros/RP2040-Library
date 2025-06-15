/*
 * Display_Font.c
 *
 * Created: Mon Jul 19 2021 17:48:37
 *  Author Chris
 */
#include "Display_Font.h"
#include "Display_Shapes.h"

#include <string.h>


// ============================================================================================
// Defines


// ============================================================================================
// Variables
volatile const unsigned char* _Font;
const uint8_t FONT_DEF_LENGTH	   = 0;	
const uint8_t FONT_DEF_FIXED_WIDTH = 2;
const uint8_t FONT_DEF_HEIGHT      = 3;
const uint8_t FONT_DEF_FIRST_CHAR  = 4;
const uint8_t FONT_DEF_CHAR_COUNT  = 5;
const uint8_t FONT_DEF_WIDTH_TABLE = 6;


// ============================================================================================
// Function Declarations


/*******************************************************************
	Functions
*******************************************************************/
void Display_Font_Init(void)
{
	_Font = NULL;
}

void Display_Font_Set_Font(const unsigned char* font)
{
	_Font = font;
}

uint Display_Font_Get_Font_Height(void)
{
	return _Font[FONT_DEF_HEIGHT];
}

uint Display_Font_Width_Char(char c)
{
	if((c < ' ') || (c > '~') || (_Font == NULL))
	{
		return 0;
	}

	if((_Font[FONT_DEF_LENGTH] == 0) && (_Font[FONT_DEF_LENGTH + 1] == 0))
	{
		return _Font[FONT_DEF_FIXED_WIDTH];
	}
	else
	{
		return _Font[FONT_DEF_WIDTH_TABLE + c - _Font[FONT_DEF_FIRST_CHAR]];
	}
}

uint Display_Font_Width_String(char string[], uint8_t string_length, uint8_t char_spacing)
{
	uint Width = 0;

	// if((_Font[FONT_DEF_LENGTH] == 0) && (_Font[FONT_DEF_LENGTH + 1] == 0))
	// {
	// 	return string_length * _Font[FONT_DEF_FIXED_WIDTH] + (string_length-1) * char_spacing;
	// }

	for(uint8_t i=0;i<string_length;i++)
	{
		Width += Display_Font_Width_Char(string[i]);
		if(i<string_length-1) {
			Width += char_spacing;
		}
	}

	return Width;
}

uint Display_Font_Print_Char(int16_t x, int16_t y, char c, Display_Color color)
{
	if((c < ' ') || (c > '~') || (_Font == NULL)) { return 0; }

	int16_t Width;
	int16_t Height     	= _Font[FONT_DEF_HEIGHT];
	int16_t H_Steps    	= (Height + 7 ) / 8;
	uint16_t Char_Count = _Font[FONT_DEF_CHAR_COUNT];
	uint16_t Index      = 0;

	c -= _Font[FONT_DEF_FIRST_CHAR];

	if((_Font[FONT_DEF_LENGTH] == 0) && (_Font[FONT_DEF_LENGTH + 1] == 0))
	{
		Width = _Font[FONT_DEF_FIXED_WIDTH];

		// c 					= Character normalized to first available character
		// (H_Steps * Width) 	= How many bytes per Character
		// FONT_DEF_WIDTH_TABLE = Byte offset for the first byte in case of fixed width
		Index = c * (H_Steps * Width) + FONT_DEF_WIDTH_TABLE;
	}
	else
	{
		// Read width data, to get the index
		for(uint i = 0; i < c; i++)
		{
			Index += _Font[FONT_DEF_WIDTH_TABLE + i];
		}
		
		Index = Index * H_Steps + Char_Count + FONT_DEF_WIDTH_TABLE;
		Width = _Font[FONT_DEF_WIDTH_TABLE + c];
	}
	
	// Print Character
	for(int32_t h = 0; h < H_Steps; h++)
	{
		int32_t Page = h * Width;			// Width == Character Width
		for(int32_t w = 0; w < Width; w++)
		{
			uint8_t Data = _Font[Index + Page + w];

			// if(Height < (h+1)*8)
			// {
			// 	Data >>= (h+1) * 8 - Height;
			// }

			for(int32_t t = 0; t < 8; t++)
			{
				if((Data & 0x01) > 0)
				{
					Display_Shapes_Draw_Pixel(x + w, y + t + (h * 8), color);
				}
				Data >>= 1;
			}
		}
	}
	return Width;
}

void Display_Font_Print_String(int16_t x, int16_t y, char* string, uint8_t string_length, uint8_t char_spacing, Display_Color color)
{
	uint16_t Width;

	// No idea what this piece of code was used for, but it is causing issues when adding objects outside the visible display area
	//
	// if(y > DISPLAY_WIDTH - _Font[FONT_DEF_HEIGHT])
	// {
	// 	x = 0;
	// 	y = 0;
	// }

	for(uint8_t i=0;i<string_length;i++)
	{		
		Width = Display_Font_Print_Char(x , y, string[i], color);
		x += (Width + char_spacing);
	}
}

/*******************************************************************
	Internal Functions
*******************************************************************/


