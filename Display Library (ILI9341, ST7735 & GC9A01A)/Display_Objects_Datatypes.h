/*
 * Display_Objects_Datatypes.h
 *
 * Created: Sun Apr 04 2021 11:48:15
 *  Author Chris
 */

#ifndef DISPLAY_OBJECTS_DATATYPES_H_
#define DISPLAY_OBJECTS_DATATYPES_H_

// ============================================================================================
// Includes
#include "Display_Config.h"


// ============================================================================================
// Defines
#define	PADDING_TOP			0
#define	PADDING_RIGHT		1
#define	PADDING_BOTTOM		2
#define	PADDING_LEFT		3

#define HEIGHT_CONTENT		0
#define WIDTH_CONTENT		0

#define NO_STYLE			-1
#define NO_ANIMATION		-1

#define PADDING_1(__ALL__)						__ALL__,__ALL__,__ALL__,__ALL__
#define PADDING_2(__HOR__, __VER__)				__HOR__,__VER__,__HOR__,__VER__
#define PADDING_4(__T__, __R__, __B__, __L__)	__T__,__R__,__B__,__L__

#define SELECTABLE			true
#define NOT_SELECTABLE		false


// ============================================================================================
// Variables / Datatypes

/*******************************************************************
	Generic Types
*******************************************************************/
typedef enum  {
	FLOAT,
	INTEGER,
	TEXT,
	VALUE_BAR,
	GRAPH,
	BUTTON,
	IMAGE,
	BOOLEAN,
	SHAPE,
	CANVAS,
	MESSAGE_BOX
} Object_Type;

typedef enum  {
	RECTANGLE
} Shape_Type;

typedef enum 
{
	CENTER		= 0x00,
	LEFT		= 0x01,
	RIGHT		= 0x02,

	MIDDLE		= 0x00,
	TOP			= 0x10,
	BOTTOM		= 0x20,

	CENTERED	= CENTER | MIDDLE
} Alignment;

typedef enum
{
	LEFT_TOP 		= LEFT | TOP,
	LEFT_MIDDLE 	= LEFT | MIDDLE,
	LEFT_BOTTOM 	= LEFT | BOTTOM,

	CENTER_TOP 		= CENTER | TOP,
	CENTER_MIDDLE 	= CENTER | MIDDLE,
	CENTER_BOTTOM 	= CENTER | BOTTOM,

	RIGHT_TOP 		= RIGHT | TOP,
	RIGHT_MIDDLE 	= RIGHT | MIDDLE,
	RIGHT_BOTTOM 	= RIGHT | BOTTOM
} Anchor_Point;

typedef enum {
	BOTH_IN_PIXEL,
	X_IN_PIXEL_Y_IN_PERCENT,
	X_IN_PERCENT_Y_IN_PIXEL,
	BOTH_IN_PERCENT
} Coordinates_Type;

typedef enum {
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	BOTTOM_TO_TOP,
	TOP_TO_BOTTOM
} Orientation;

typedef enum {
	START_WITH_PREVIOUS,
	START_AFTER_PREVIOUS
} Animation_Order;

typedef enum {
	NO_STARTED,
	DELAYING,
	MOVING,
	COMPLETE
} Animation_State;

typedef enum {
	CANVAS_UPDATE_OK	= 0,
	OBJECT_NOT_FOUND 	= 1,
	OBJECT_WRONG_TYPE	= 2,
	X_OUT_OF_RANGE 		= 3,
	Y_OUT_OF_RANGE 		= 4
} Canvas_Update_Error;

typedef struct {
	int16_t X;
	int16_t Y;
} Coordinates;

typedef struct {
	uint16_t Width;
	uint16_t Height;
} Dimension;

typedef int Object_ID;
typedef int Style_ID;
typedef int Animation_ID;
typedef int Font_ID;

typedef enum {
	TRANSITION_NONE,
	TRANSITION_LEFT,
	TRANSITION_RIGHT,
	TRANSITION_UP,
	TRANSITION_DOWN
} Screen_Transition_Direction;


/*******************************************************************
	Font Type
*******************************************************************/
typedef struct {
	const unsigned char 	*Font;
	uint8_t					Character_Spacing;
} Font;


/*******************************************************************
	Different Format Types
*******************************************************************/
typedef struct {
	float* 			Value;
	char 			Format[64];
	Font* 			Font;
	Display_Color 	Color;
} Object_Float;

typedef struct {
	int* 			Value;
	char 			Format[64];
	Font* 			Font;
	Display_Color 	Color;
} Object_Integer;

typedef struct {
	char 			Text[64];
	uint8_t			Length;
	Font*			Font;
	Display_Color 	Color;
} Object_Text;

typedef struct {
	uint*			Value;
	uint			Max;
	uint			Min;
	Orientation		Orientation;
	Display_Color 	Color;
	Dimension		Dimension;
} Object_Value_Bar;

typedef struct {
	uint16_t*		Data;
	uint16_t		Data_Length;
	uint			Max;
	uint			Min;
	Display_Color 	Color;
	Dimension		Dimension;
} Object_Graph;

typedef struct {
	char 			Text[64];
	uint8_t			Length;
	Font*			Font;
	uint			Return_Value;
	Display_Color 	Color;
	Dimension		Dimension;
} Object_Button;

typedef const uint16_t 	Image_Color;
typedef const uint8_t 	Image_Alpha;

typedef struct {
	bool*			Value;			
	char 			Text_True[16];
	uint8_t			Length_True;
	Display_Color 	Color_True;
	char 			Text_False[16];
	uint8_t			Length_False;
	Display_Color 	Color_False;
	Font*			Font;
} Object_Bool;

typedef struct {
	Shape_Type 		Type;
	Display_Color 	Color;
	Dimension		Dimension;
} Object_Shape;

typedef struct {
	 Dimension		Dimension;
	 uint16_t*		Data;
} Object_Canvas;


typedef enum {
	MESSAGE_BOX_TEXT_BAR_DISTANCE 	= 2,
	MESSAGE_BOX_BAR_HEIGHT 			= 3,
	MESSAGE_BOX_TEXT_ICON_DISTANCE 	= 2
} Object_Message_Box_Properties;

typedef enum {
	MESSAGE_BOX_ICON_NONE,
	MESSAGE_BOX_ICON_CIRCLE_ADD,
	MESSAGE_BOX_ICON_CIRCLE_ALERT,
	MESSAGE_BOX_ICON_CRCLE_BAN,
	MESSAGE_BOX_ICON_CHATBUBBLE,
	MESSAGE_BOX_ICON_CIRCLE_CHECKMARK,
	MESSAGE_BOX_ICON_CIRCLE_CLOSE,
	MESSAGE_BOX_ICON_CIRCLE_INFORMATION,
	MESSAGE_BOX_ICON_EMOJI_HAPPY,
	MESSAGE_BOX_ICON_EMOJI_SAD,
	MESSAGE_BOX_ICON_EMOJI_SKULL
} Object_Message_Box_Icon;

typedef struct {
	char 					Text[64];
	uint8_t					Length;
	Font*					Font;
	Display_Color 			Color;
	Object_Message_Box_Icon Icon;

	uint Show_Ticks_Max;
	uint Show_Ticks_Left;
} Object_Message_Box;


/*******************************************************************
	Style Type
*******************************************************************/
typedef struct {
	Display_Color 	Background_Color;
	Display_Color	Border_Color;
	uint			Border_Thickness;
	uint			Border_Radius;
	uint			Padding[4];
	float			Width_Height_Ratio;
} Style;


/*******************************************************************
	Animation Type
*******************************************************************/
typedef struct {
	Coordinates 	Offset;
	uint			Tick_Delay;
	uint			Tick_Duration;
	Animation_Order	Animation_Start;
} Animation;

typedef struct {
	Coordinates			Target;
	Animation_State		State;
	uint				Tick_Counter;
} Animation_Status;


/*******************************************************************
	Main Object Type
*******************************************************************/
typedef struct {
	Object_Type			Type;
	Coordinates			Coordinates;
	Coordinates			Content_Offset;
	Dimension			Dimension;
	bool				Selectable;
	bool				Focused;
	void* 				Data;
	Style*				Style;
	Animation*			Animation;
	Animation_Status* 	Animation_Status;
} Display_Object;

#endif /* DISPLAY_OBJECTS_DATATYPES_H_ */