/*
 * Screen_Menu_Main.c
 *
 * Created: Fri Apr 02 2021 14:34:01
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "Screens_Display.h"
#include "Display.h"
#include "Display_Objects.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables
extern const unsigned char 	_Font_Victor_Mono_Regular_8x19[];


// ============================================================================================
// Function Declarations
void Screen_Display_Setup_Menu_Main();

static void Screen_Display_Tick					(void);
static void Screen_Display_Click				(uint button_return_value);
static void Screen_Display_Touch_Event			(int16_t x, int16_t y);
static void Screen_Display_Action_CW			(Object_ID object_id);
static void Screen_Display_Action_CCW			(Object_ID object_id);
static void Screen_Display_On_Object_Focused	(Object_ID object_id);
static void Screen_Display_On_Object_Defocused	(Object_ID object_id);
static void Screen_Display_On_Object_Select		(Object_ID object_id);
static void Screen_Display_On_Object_Deselect	(Object_ID object_id);

static Object_ID _Object_Rect_1;
static Object_ID _Object_Rect_2;
static Object_ID _Object_Rect_3;

static int _Counter;
static int _Counter_Direction;


/*******************************************************************
	Functions
*******************************************************************/
void Screen_Display_Setup_Menu_Main()
{
	_Screen_Display_Tick					= Screen_Display_Tick;
	_Screen_Display_Click					= Screen_Display_Click;
	_Screen_Display_Touch_Event				= Screen_Display_Touch_Event;
	_Screen_Display_Action_CW				= Screen_Display_Action_CW;
	_Screen_Display_Action_CCW				= Screen_Display_Action_CCW;
	_Screen_Display_On_Objects_Focused 		= Screen_Display_On_Object_Focused;
	_Screen_Display_On_Objects_Defocused 	= Screen_Display_On_Object_Defocused;
	_Screen_Display_On_Object_Select		= Screen_Display_On_Object_Select;
	_Screen_Display_On_Object_Deselect		= Screen_Display_On_Object_Deselect;

	Display_Objects_Clear();


	Font_ID Font_Victor = Display_Objects_Add_Font(_Font_Victor_Mono_Regular_8x19, 2);
	
	//////////////////////////////
	// Add Display Objects here //
	//////////////////////////////
	_Object_Rect_1 = Display_Objects_Add_Rectangle(CENTER_MIDDLE, BOTH_IN_PERCENT, 25, 25, false, DISPLAY_COLOR_RED	, 40, 40, NO_STYLE, NO_ANIMATION);
	_Object_Rect_2 = Display_Objects_Add_Rectangle(CENTER_MIDDLE, BOTH_IN_PERCENT, 50, 50, false, DISPLAY_COLOR_GREEN, 40, 40, NO_STYLE, NO_ANIMATION);
	_Object_Rect_3 = Display_Objects_Add_Rectangle(CENTER_MIDDLE, BOTH_IN_PERCENT, 75, 75, false, DISPLAY_COLOR_BLUE, 40, 40, NO_STYLE, NO_ANIMATION);

	Display_Objects_Add_Text(CENTER_MIDDLE, BOTH_IN_PERCENT, 50, 70, false, "Hello World", Font_Victor, DISPLAY_COLOR_WHITE, NO_STYLE, NO_ANIMATION);

	Display_Select_First_Object();
	Display_Unselect_Object();

	 _Counter = 0;
	 _Counter_Direction = 0;
}

void Screen_Display_Tick(void)
{
	if(_Counter_Direction == 0)
	{
		if(_Counter == 50) {
			_Counter_Direction = 1;
		} else {
			_Counter++;
		}
	}
	else {
		if(_Counter == 0) {
			_Counter_Direction = 0;
		} else {
			_Counter--;
		}
	}

	Display_Objects_Update_Coordinates(_Object_Rect_1, CENTER_MIDDLE, BOTH_IN_PERCENT, 25, 25 + _Counter);
	Display_Objects_Update_Coordinates(_Object_Rect_3, CENTER_MIDDLE, BOTH_IN_PERCENT, 75, 75 - _Counter);
}

void Screen_Display_Click(uint button_return_value)
{

}

void Screen_Display_Touch_Event(int16_t x, int16_t y)
{

}

void Screen_Display_Action_CW(Object_ID object_id)
{
	
}

void Screen_Display_Action_CCW(Object_ID object_id)
{
	
}

void Screen_Display_On_Object_Focused(Object_ID object_id)
{
	
}

void Screen_Display_On_Object_Defocused(Object_ID object_id)
{
	
}

void Screen_Display_On_Object_Select(Object_ID object_id)
{

}

void Screen_Display_On_Object_Deselect(Object_ID object_id)
{

}

/*******************************************************************
	Internal Functions
*******************************************************************/
