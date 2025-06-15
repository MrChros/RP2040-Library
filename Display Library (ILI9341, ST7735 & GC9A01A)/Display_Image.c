/*
 * Display_Image.c
 *
 * Created: Thu Nov 25 2021 13:15:42
 *  Author Chris
 */
#include "Display_Image.h"

#include "Display_Shapes.h"

// ============================================================================================
// Defines


// ============================================================================================
// Variables
volatile Display_Color _Transparent_Color;
const uint8_t IMAGE_DEF_WIDTH 		= 0;
const uint8_t IMAGE_DEF_HEIGHT      = 1;
const uint8_t IMAGE_DEF_DATA_SIZE 	= 2;
const uint8_t IMAGE_DEF_DATA_OFFSET	= 3;


// ============================================================================================
// Function Declarations


/*******************************************************************
	Functions
*******************************************************************/
void Display_Image_Init(Display_Color transparent_color)
{
	_Transparent_Color = transparent_color;
}

void Display_Image_Draw_Color(int16_t x, int16_t y, Image_Color* image)
{
	if(image == NULL) { return; }

	int16_t Height = image[IMAGE_DEF_HEIGHT];
	int16_t Width = image[IMAGE_DEF_WIDTH];
	
	for(int16_t iy = 0; iy < Height ; iy++)
	{
		for (int16_t ix = 0; ix < Width; ix++)
		{
			if(image[IMAGE_DEF_DATA_OFFSET + iy * Width + ix] != _Transparent_Color) {
				Display_Shapes_Draw_Pixel(x + ix, y + iy, image[IMAGE_DEF_DATA_OFFSET + iy * Width + ix]);
			}
		}
	}
}

void Display_Image_Draw_Alpha(int16_t x, int16_t y, Image_Alpha* image, Display_Color color)
{
	if(image == NULL) { return; }

	int16_t Height = image[IMAGE_DEF_HEIGHT];
	int16_t Width = image[IMAGE_DEF_WIDTH];
	
	for(int16_t iy = 0; iy < Height ; iy++)
	{
		for (int16_t ix = 0; ix < Width; ix++)
		{
			if(image[IMAGE_DEF_DATA_OFFSET + iy * Width + ix] == UINT8_MAX) {
				Display_Shapes_Draw_Pixel(x + ix, y + iy, color);
			}
			else if(image[IMAGE_DEF_DATA_OFFSET + iy * Width + ix] > 0) {
				// This is only temporary here...
				// Color needs to be scaled according to alpha value from image array
				// Skipped for now, as only threshold images will be applied
				Display_Shapes_Draw_Pixel(x + ix, y + iy, color);
			}
		}
	}
}

uint16_t Display_Image_Get_Width(Image_Color* image)
{
	if(image == NULL) { return 0; }

	return image[IMAGE_DEF_WIDTH];
}

uint16_t Display_Image_Get_Height(Image_Color* image)
{
	if(image == NULL) { return 0; }

	return image[IMAGE_DEF_HEIGHT];
}

/*******************************************************************
	Internal Functions
*******************************************************************/


