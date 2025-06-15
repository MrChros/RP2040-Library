/*
 * Display_Shapes.c
 *
 * Created: Mon Jul 19 2021 17:57:03
 *  Author Chris
 */
#include "Display_Shapes.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// ============================================================================================
// Defines
#define CORNER_TOP_LEFT			0x01
#define CORNER_TOP_RIGHT		0x02
#define CORNER_BOTTOM_LEFT		0x08
#define CORNER_BOTTOM_RIGHT		0x04


// ============================================================================================
// Variables
static Display_Image_Buffer* _Image_Buffer;


// ============================================================================================
// Function Declarations
void Display_Shapes_Draw_Circle_Helper(int16_t x0, int16_t y0, uint16_t radius, uint16_t thickness, uint8_t cornername, Display_Color color);
void Display_Shapes_Draw_Circle_Helper_Filled(int16_t x0, int16_t y0, uint16_t radius, uint8_t corners, int16_t delta, Display_Color color);


/*******************************************************************
	Functions
*******************************************************************/
void Display_Shapes_Init(Display_Image_Buffer* image_buffer)
{
	_Image_Buffer = image_buffer;
}

void Display_Shapes_Fill_Screen(Display_Color color)
{
	switch(color)
	{
		case DISPLAY_COLOR_BLACK:
			memset((void*)_Image_Buffer->Dim_1, 0x00, DISPLAY_IMAGE_BUFFER_BYTE_SIZE);
			break;

		case DISPLAY_COLOR_WHITE:
			memset((void*)_Image_Buffer->Dim_1, 0xFF, DISPLAY_IMAGE_BUFFER_BYTE_SIZE);
			break;

		default:
			for(uint i=0;i<DISPLAY_IMAGE_BUFFER_PIXEL_SIZE;i++) { _Image_Buffer->Dim_1[i] = color; }
			break;
	}

}

void Display_Shapes_Draw_Pixel(int16_t x, int16_t y, Display_Color color)
{
	if(x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT)
	{
		_Image_Buffer->Dim_2[x][y] = color;
	}
}

void Display_Shapes_Draw_HLine(int16_t x, int16_t y, uint16_t width, uint16_t thickness, Display_Color color)
{
	Display_Shapes_Draw_Rect_Filled(x, y, width, thickness, color);
}

void Display_Shapes_Draw_VLine(int16_t x, int16_t y, uint16_t height, uint16_t thickness, Display_Color color)
{
	Display_Shapes_Draw_Rect_Filled(x, y, thickness, height, color);
}

void Display_Shapes_Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t thickness, Display_Color color)
{
	int16_t Temp;
	
	if(y0 == y1)
	{
		if(x1 > x0)
		{
			Display_Shapes_Draw_HLine(x0, y0, x1 - x0 + 1, thickness, color);
		}
		else if (x1 < x0)
		{
			Display_Shapes_Draw_HLine(x1, y0, x0 - x1 + 1, thickness, color);
		}
		else
		{
			Display_Shapes_Draw_Circle_Filled(x0, y0, thickness, color);
		}
		return;
	}
	else if(x0 == x1)
	{
		if(y1 > y0)
		{
			Display_Shapes_Draw_VLine(x0, y0, y1 - y0 + 1, thickness, color);
		}
		else
		{
			Display_Shapes_Draw_VLine(x0, y1, y0 - y1 + 1, thickness, color);
		}
		return;
	}

	bool Steep = abs(y1 - y0) > abs(x1 - x0);
	if(Steep == true)
	{
		Temp = x0; x0 = y0; y0 = Temp;
		Temp = x1; x1 = y1; y1 = Temp;
	}
	if(x0 > x1)
	{
		Temp = x0; x0 = x1; x1 = Temp;
		Temp = y0; y0 = y1; y1 = Temp;
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else 
	{
		ystep = -1;
	}

	int16_t xbegin = x0;
	if(Steep == true)
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len>1)
				{
					Display_Shapes_Draw_VLine(y0, xbegin, len + 1, thickness, color);
				}
				else
				{
					if(thickness == 1)
						Display_Shapes_Draw_Pixel(y0, x0, color);
					else
						Display_Shapes_Draw_Circle_Filled(y0, x0, thickness >> 1, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		{
			Display_Shapes_Draw_VLine(y0, xbegin, x0 - xbegin, thickness, color);
		}

	}
	else
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len>1)
				{
					Display_Shapes_Draw_HLine(xbegin, y0, len + 1, thickness, color);
				}
				else
				{
					if(thickness == 1)
						Display_Shapes_Draw_Pixel(x0, y0, color);
					else
						Display_Shapes_Draw_Circle_Filled(x0, y0, thickness >> 1, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		{
			Display_Shapes_Draw_HLine(xbegin, y0, x0 - xbegin, thickness, color);
		}
	}
}

void Display_Shapes_Draw_Rect_Frame(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t thickness, Display_Color color)
{
	Display_Shapes_Draw_HLine(x			, y			, width				, thickness, color);
	Display_Shapes_Draw_HLine(x			, y + height, width + thickness	, thickness, color);

	Display_Shapes_Draw_VLine(x			, y			, height			, thickness, color);
	Display_Shapes_Draw_VLine(x + width	, y			, height + thickness, thickness, color);
}

void Display_Shapes_Draw_Rect_Filled(int16_t x, int16_t y, uint16_t width, uint16_t height, Display_Color color)
{
	for(int16_t r=y;r < (y + height) /* && r < DISPLAY_HEIGHT*/ ;r++)
	{
		for(int16_t c=x;c < (x + width) /* && c < DISPLAY_WIDTH*/ ;c++)
		{
			Display_Shapes_Draw_Pixel(c, r, color);
		}
	}	
}

void Display_Shapes_Draw_Circle_Frame(int16_t x, int16_t y, uint16_t radius, uint16_t thickness, Display_Color color)
{
	x = x + radius;
	y = y + radius;
	
	int16_t f 		= 1 - radius;
	int16_t ddF_x 	= 1;
	int16_t ddF_y 	= -2 * radius;
	int16_t X 		= 0;
	int16_t Y 		= radius;

	if(thickness == 1)
	{
		Display_Shapes_Draw_Pixel(x  		, y+radius	, color);
		Display_Shapes_Draw_Pixel(x  		, y-radius	, color);
		Display_Shapes_Draw_Pixel(x+radius	, y			, color);
		Display_Shapes_Draw_Pixel(x-radius	, y			, color);
	}
	else
	{
		Display_Shapes_Draw_Circle_Filled(x  		, y+radius	, thickness >> 1, color);
		Display_Shapes_Draw_Circle_Filled(x  		, y-radius	, thickness >> 1, color);
		Display_Shapes_Draw_Circle_Filled(x+radius	, y  		, thickness >> 1, color);
		Display_Shapes_Draw_Circle_Filled(x-radius	, y  		, thickness >> 1, color);
	}
	
	while (X<Y)
	{
		if (f >= 0)
		{
			Y--;
			ddF_y += 2;
			f += ddF_y;
		}
		X++;
		ddF_x += 2;
		f += ddF_x;

		if(thickness == 1)
		{
			Display_Shapes_Draw_Pixel(x + X, y + Y, color);
			Display_Shapes_Draw_Pixel(x - X, y + Y, color);
			Display_Shapes_Draw_Pixel(x + X, y - Y, color);
			Display_Shapes_Draw_Pixel(x - X, y - Y, color);
			Display_Shapes_Draw_Pixel(x + Y, y + X, color);
			Display_Shapes_Draw_Pixel(x - Y, y + X, color);
			Display_Shapes_Draw_Pixel(x + Y, y - X, color);
			Display_Shapes_Draw_Pixel(x - Y, y - X, color);
		}
		else
		{
			Display_Shapes_Draw_Circle_Filled(x + X, y + Y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x - X, y + Y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x + X, y - Y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x - X, y - Y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x + Y, y + X, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x - Y, y + X, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x + Y, y - X, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x - Y, y - X, thickness >> 1, color);
		}
	}
}

void Display_Shapes_Draw_Circle_Filled(int16_t x, int16_t y, uint16_t radius, Display_Color color)
{
	x = x + radius;
	y = y + radius;
	
	Display_Shapes_Draw_VLine(x, y-radius, 2*radius+1, 1, color);

	uint8_t Corner_Name = 3;
	int16_t Delta		= 0;

	int16_t f     = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t X     = 0;
	int16_t Y     = radius;

	while(X<Y)
	{
		if(f >= 0)
		{
			Y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		X++;
		ddF_x += 2;
		f     += ddF_x;

		if ((Corner_Name & 0x1) > 0)
		{
			Display_Shapes_Draw_VLine(x+X, y-Y, 2*Y+1+Delta, 1, color);
			Display_Shapes_Draw_VLine(x+Y, y-X, 2*X+1+Delta, 1, color);
		}
		if ((Corner_Name & 0x2)>0)
		{
			Display_Shapes_Draw_VLine(x-X, y-Y, 2*Y+1+Delta, 1, color);
			Display_Shapes_Draw_VLine(x-Y, y-X, 2*X+1+Delta, 1, color);
		}
	}
}

void Display_Shapes_Draw_Round_Rect_Frame(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t thickness, Display_Color color)
{
	uint16_t max_radius = ((width < height) ? width : height) / 2;	// 1/2 minor axis
	
	if (radius > max_radius)
	{
		radius = max_radius;
	}

	Display_Shapes_Draw_HLine(x + radius			, y						, width - 2 * radius	, thickness, color);		// Top
	Display_Shapes_Draw_HLine(x + radius			, y + height - thickness, width - 2 * radius	, thickness, color);		// Bottom
	Display_Shapes_Draw_VLine(x						, y + radius			, height - 2 * radius	, thickness, color);		// Left
	Display_Shapes_Draw_VLine(x + width - thickness	, y + radius			, height - 2 * radius	, thickness, color);		// Right

	// Draw Four Corners
	Display_Shapes_Draw_Circle_Helper(x + radius					, y + radius						, radius, thickness, CORNER_TOP_LEFT	, color);
	Display_Shapes_Draw_Circle_Helper(x + width - radius - thickness, y + radius						, radius, thickness, CORNER_TOP_RIGHT	, color);
	Display_Shapes_Draw_Circle_Helper(x + width - radius - thickness, y + height - radius - thickness	, radius, thickness, CORNER_BOTTOM_RIGHT, color);
	Display_Shapes_Draw_Circle_Helper(x + radius					, y + height - radius - thickness	, radius, thickness, CORNER_BOTTOM_LEFT	, color);
}

void Display_Shapes_Draw_Round_Rect_Filled(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, Display_Color color)
{
	int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis

	if (r > max_radius)
	{
		r = max_radius;
	}

	// Smarter Version
	Display_Shapes_Draw_Rect_Filled(x + r, y, w - 2 * r, h, color);

	// Draw Four Corners
	Display_Shapes_Draw_Circle_Helper_Filled(x + w - r - 1	, y + r, r, 1, h - 2 * r - 1, color);
	Display_Shapes_Draw_Circle_Helper_Filled(x + r			, y + r, r, 2, h - 2 * r - 1, color);
}

/*******************************************************************
	Internal Functions
*******************************************************************/
/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void Display_Shapes_Draw_Circle_Helper(int16_t x0, int16_t y0, uint16_t radius, uint16_t thickness, uint8_t cornername, Display_Color color)
{
	int16_t f 		= 1 - radius;
	int16_t ddF_x 	= 1;
	int16_t ddF_y 	= -2 * radius;
	int16_t x 		= 0;
	int16_t y 		= radius;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		if (cornername & CORNER_BOTTOM_RIGHT)
		{
			Display_Shapes_Draw_Circle_Filled(x0 + x, y0 + y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x0 + y, y0 + x, thickness >> 1, color);
		}

		if (cornername & CORNER_TOP_RIGHT)
		{
			Display_Shapes_Draw_Circle_Filled(x0 + x, y0 - y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x0 + y, y0 - x, thickness >> 1, color);
		}

		if (cornername & CORNER_BOTTOM_LEFT)
		{
			Display_Shapes_Draw_Circle_Filled(x0 - x, y0 + y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x0 - y, y0 + x, thickness >> 1, color);
		}

		if (cornername & CORNER_TOP_LEFT)
		{
			Display_Shapes_Draw_Circle_Filled(x0 - x, y0 - y, thickness >> 1, color);
			Display_Shapes_Draw_Circle_Filled(x0 - y, y0 - x, thickness >> 1, color);
		}
	}
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void Display_Shapes_Draw_Circle_Helper_Filled(int16_t x0, int16_t y0, uint16_t radius, uint8_t corners, int16_t delta, Display_Color color)
{
	int16_t f 		= 1 - radius;
	int16_t ddF_x 	= 1;
	int16_t ddF_y 	= -2 * radius;
	int16_t x 		= 0;
	int16_t y 		= radius;
	int16_t px 		= x;
	int16_t py 		= y;

	delta++; // Avoid some +1's in the loop

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		// These checks avoid double-drawing certain lines, important
		// for the SSD1306 library which has an INVERT drawing mode.
		if (x < (y + 1))
		{
			if (corners & CORNER_TOP_LEFT) {
				Display_Shapes_Draw_VLine(x0 + x, y0 - y, 2 * y + delta, 1, color);
				// writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
			}
			if (corners & CORNER_TOP_RIGHT) {
				Display_Shapes_Draw_VLine(x0 - x, y0 - y, 2 * y + delta, 1, color);
				// writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
			}
		}

		if (y != py)
		{
			if (corners & CORNER_TOP_LEFT) {
				Display_Shapes_Draw_VLine(x0 + py, y0 - px, 2 * px + delta, 1, color);
				// writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
			}
			if (corners & CORNER_TOP_RIGHT) {
				Display_Shapes_Draw_VLine(x0 - py, y0 - px, 2 * px + delta, 1, color);
				// writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
			}
			py = y;
		}
		px = x;
	}
}