/*
 * Display_Objects.h
 *
 * Created: Fri Apr 02 2021 13:58:11
 *  Author Chris
 */

#ifndef DISPLAY_OBJECTS_H_
#define DISPLAY_OBJECTS_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Display_Config.h"
#include "Display_Objects_Datatypes.h"


// ============================================================================================
// Defines
#define STYLE_WIDTH_HEIGHT_RATIO_AUTO	0


// ============================================================================================
// Variables / Datatypes
// See "Display_Objects_Datatypes.h"


// ============================================================================================
// Function Declarations
void Display_Objects_Init(Display_Color background_color);

void 			Display_Objects_Background_Color_Set(Display_Color color);
Display_Color 	Display_Objects_Background_Color_Get(void);
void 			Display_Objects_Clear(void);

Style_ID		Display_Objects_Add_Style(		Display_Color background_color,
												Display_Color border_color, uint border_thickness, uint border_radius,
												uint padding_top, uint padding_right, uint padding_bottom, uint padding_left,
												float width_height_ratio);

Animation_ID	Display_Objects_Add_Animation(	int offset_x,
												int offset_y,
												uint tick_delay,
												uint tick_duration,
												Animation_Order start_condition);

Font_ID			Display_Objects_Add_Font(		const unsigned char *font,
												uint8_t character_spacing);

size_t			Display_Objects_Count(void);
Display_Object*	Display_Objects_Get_By_ID(Object_ID id);

size_t		Display_Objects_Get_Count_Canvas(void);
Object_ID	Display_Objects_Get_ID_From_Canvas_Number(uint canvas_number);

Object_ID	Display_Objects_Add_Float		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, float* value, char* format, 	Font_ID font					, Display_Color color								, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Integer		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, int* value, char* format, 	Font_ID font					, Display_Color color								, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Text		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, char* text, 					Font_ID font					, Display_Color color								, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Value_Bar	(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, uint* value, uint max, uint min, Orientation orientation 		, Display_Color color, uint width, uint height		, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Graph		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, uint16_t* data, uint16_t data_length, uint max, uint min		, Display_Color color			 , uint height		, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Button		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, char* text, 					Font_ID font, uint return_value	, Display_Color color, uint width, uint height		, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Image		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Image_Color* image																								, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Bool		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, bool* value, char* text_true, Display_Color color_true, char* text_false, Display_Color color_false, Font_ID font	, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Rectangle	(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Display_Color color, uint width, uint height																		, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Canvas		(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Display_Color color, uint width, uint height																		, Style_ID style_id, Animation_ID animation_id);
Object_ID	Display_Objects_Add_Message_Box (Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y,				  char* text, Font_ID font, Object_Message_Box_Icon icon		, Display_Color color								, Style_ID style_id);

void	Display_Objects_Update_Coordinates	(Object_ID id, Anchor_Point anchor_point,  Coordinates_Type coordinates_type, int16_t x, int16_t y);
void 	Display_Objects_Update_Color		(Object_ID id, Display_Color color);
void	Display_Objects_Update_Text			(Object_ID id, char* text);
void	Display_Objects_Update_Image		(Object_ID id, Image_Color* new_image);
void 	Display_Objects_Update_Max_Min		(Object_ID id, uint max, uint min);
int		Display_Objects_Update_Pixel		(Object_ID id, int16_t x, int16_t y, Display_Color color);
void	Display_Objects_Show_Message_Box	(Object_ID id, uint ticks);

#endif /* DISPLAY_OBJECTS_H_ */
