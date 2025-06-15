/*
 * Display.c
 *
 * Created: Sun Mar 21 2021 15:52:12
 *  Author Chris
 */
#include "Display.h"

#include <string.h>

#include "Screens_Display.h"
#include "Display_SPI.h"
#include "Display_Init.h"
#include "Display_Font.h"
#include "Display_Image.h"
#include "Display_Touch.h"
#include "Display_Shapes.h"
#include "Display_Objects.h"
#include "Display_Message_Box_Icons.h"


// ============================================================================================
// Defines
#define SCREEN_TRANSITION_STEP_COUNT_HORIZONTAL			sizeof(_Screen_Transition_Steps_Horizontal)
#define SCREEN_TRANSITION_STEP_COUNT_VERTICAL			sizeof(_Screen_Transition_Steps_Vertical)


// ============================================================================================
// Variables
static Display_Image_Buffer _Image_Buffer;

static int _Touched_Button_Return_Value;

static bool _Object_Selected 				= false;
static uint _Frame_Counter 					= 0;
static bool _Touch_Initialized				= false;
static bool _Draw_Touch_Reference_Points 	= false;
static bool _Draw_Center_Lines				= false;
static bool _Debug_Print 					= false;

static Coordinates 					_Screen_Transition_Offset = { .X = 0, .Y = 0 };
static Screen_Transition_Direction 	_Screen_Transition_Direction;
static uint							_Screen_Transition_Step;
static uint8_t 						_Screen_Transition_Steps_Horizontal[] 	= { 20, 20, 18, 18, 16, 14, 12, 9, 6, 6, 4, 4, 3, 3, 2, 2 };
static uint8_t 						_Screen_Transition_Steps_Vertical[] 	= { 22, 20, 20, 16, 12, 10,  8, 6, 4, 3, 2, 2 };


// ============================================================================================
// Function Declarations
void Display_Draw_Style			(Coordinates* coordinates, Style* style, uint content_width, uint content_height, bool do_draw);
void Display_Draw_Value_Bar		(Coordinates* coordinates, Object_Value_Bar* value_bar);
void Display_Draw_Graph			(Coordinates* coordinates, Object_Graph* graph);
void Display_Draw_Button		(Coordinates* coordinates, Object_Button* button);
void Display_Draw_Canvas		(Coordinates* coordinates, Object_Canvas* canvas);
void Display_Draw_Message_Box	(Coordinates* coordinates, Object_Message_Box* message_box, uint16_t width);
void Display_Draw_Focused		(Coordinates* coordinates, uint width, uint height);

Animation_State Display_Animation_Tick(Display_Object* object);

void Display_Draw_Touch_Refernce_Points(Display_Color color);
void Display_Draw_Touch_Marker(Display_Color color);
void Display_Draw_Center_Lines(Display_Color color);

void 			Display_Check_Button_Touch(int16_t x, int16_t y);
void 			Display_Object_Select_Next(void);
void 			Display_Object_Select_Previous(void);
Display_Object* Display_Get_Selected_Object(void); 
int				Display_Get_Index_Of_Object(Display_Object* object);

void Display_Screen_Transition_Tick();
void Display_Buffer_Shift_Left(uint steps);
void Display_Buffer_Shift_Right(uint steps);
void Display_Buffer_Shift_Up(uint steps);
void Display_Buffer_Shift_Down(uint steps);

/*******************************************************************
	Functions
*******************************************************************/
void Display_Init(Display_Color initial_color, bool send_buffer, bool init_touch)
{
	_Touch_Initialized = init_touch;
	
	_Touched_Button_Return_Value = -1;

	Display_Shapes_Init(&_Image_Buffer);
	Display_Font_Init();
	Display_Image_Init(initial_color);

	Display_Init_GPIOs();
	Display_SPI_Init(init_touch);

	Display_Init_Reset();
	Display_Init_Registers();
	Display_Init_WakeUp();

	if(init_touch) {
		Display_Touch_Init();
	}

	Display_Objects_Init(initial_color);

	if(send_buffer) {
		Display_Render_Objects();
	}
}

void Display_Issue_Touch_Event(int16_t x_screen, int16_t y_screen)
{
	// Display_Check_Button_Touch(x_screen, y_screen);
	_Screen_Display_Touch_Event(x_screen, y_screen);
}

void Display_Set_Draw_Touch_Reference_Points(bool do_draw)
{
	_Draw_Touch_Reference_Points = do_draw;
}

void Display_Set_Draw_Center_Lines(bool do_draw)
{
	_Draw_Center_Lines = do_draw;
}

void Display_Screen_Transition_Start(Screen_Transition_Direction direction)
{
	_Screen_Transition_Offset.X = 0;
	_Screen_Transition_Offset.Y = 0;
	_Screen_Transition_Step = 0;
	_Screen_Transition_Direction = TRANSITION_NONE;
	
	switch (direction)
	{
	case TRANSITION_NONE:
		break;

	case TRANSITION_LEFT:
		_Screen_Transition_Offset.X = +DISPLAY_WIDTH;
		_Screen_Transition_Direction = direction;
		break;

	case TRANSITION_RIGHT:
		_Screen_Transition_Offset.X = -DISPLAY_WIDTH;
		_Screen_Transition_Direction = direction;
		break;

	case TRANSITION_UP:
		_Screen_Transition_Offset.Y = +DISPLAY_HEIGHT;
		_Screen_Transition_Direction = direction;
		break;

	case TRANSITION_DOWN:
		_Screen_Transition_Offset.Y = -DISPLAY_HEIGHT;
		_Screen_Transition_Direction = direction;
		break;
	
	default:
		break;
	}
}

void Display_Render_Objects(void)
{
	Object_Float* 		F;
	Object_Integer* 	I;
	Object_Text* 		T;
	Object_Value_Bar* 	V;
	Object_Graph*		G;
	Object_Button*		N;
	Object_Bool*		B;
	Object_Shape*		S;
	Object_Canvas*		C;
	Object_Message_Box* M;
	char 				String[64];
	uint				String_Char_Count, Width;
	Display_Color		Color;

	Animation_State	Animation_State = COMPLETE;

	if(_Screen_Transition_Direction == TRANSITION_NONE) {
		Display_Shapes_Fill_Screen(Display_Objects_Background_Color_Get());
	} else {
		Display_Screen_Transition_Tick();
	}

	for(uint i=0;i<Display_Objects_Count();i++)
	{
		Display_Object* Object 		= Display_Objects_Get_By_ID(i);
		Style*			Style		= Object->Style;
		Animation*		Animation	= Object->Animation;

		if(Animation != NULL) {
			if(Animation->Animation_Start == START_AFTER_PREVIOUS && Animation_State != COMPLETE) {
				continue;
			}

			Animation_State = Display_Animation_Tick(Object);

			if(Animation_State == NO_STARTED || Animation_State == DELAYING) {
				continue;
			}
		}

		Coordinates Coordinates_Object = Object->Coordinates;
		Coordinates_Object.X += Object->Content_Offset.X;
		Coordinates_Object.Y += Object->Content_Offset.Y;

		Coordinates_Object.X += _Screen_Transition_Offset.X;
		Coordinates_Object.Y += _Screen_Transition_Offset.Y;
		
		if(Style != NULL) {
			bool Do_Draw_Style = true;
			
			if(Object->Type == MESSAGE_BOX)
			{
				M = (Object_Message_Box*)(Object->Data);
				if(M->Show_Ticks_Left == 0) {
					Do_Draw_Style = false;
				}
			}

			Coordinates Coordinates_Style = Object->Coordinates;
			Coordinates_Style.X += _Screen_Transition_Offset.X;
			Coordinates_Style.Y += _Screen_Transition_Offset.Y;
			Display_Draw_Style(&Coordinates_Style, Style, Object->Dimension.Width, Object->Dimension.Height, Do_Draw_Style);
		}

		if(Object->Focused == true) {
			Coordinates Coordinates_Focus = Object->Coordinates;
			Coordinates_Focus.X += _Screen_Transition_Offset.X;
			Coordinates_Focus.Y += _Screen_Transition_Offset.Y;

			Display_Draw_Focused(&Coordinates_Focus, Object->Dimension.Width, Object->Dimension.Height);
		}

		switch (Object->Type)
		{
		case FLOAT:
			F = (Object_Float*)Object->Data;
			Display_Font_Set_Font(F->Font->Font);
			String_Char_Count = sprintf(String, F->Format, *(F->Value));

			Display_Font_Print_String(Coordinates_Object.X, Coordinates_Object.Y, String, String_Char_Count, F->Font->Character_Spacing, F->Color);
			break;

		case INTEGER:
			I = (Object_Integer*)Object->Data;
			Display_Font_Set_Font(I->Font->Font);
			String_Char_Count = sprintf(String, I->Format, *(I->Value));

			Display_Font_Print_String(Coordinates_Object.X, Coordinates_Object.Y, String, String_Char_Count, I->Font->Character_Spacing, I->Color);
			break;
		
		case TEXT:
			T = (Object_Text*)Object->Data;
			Display_Font_Set_Font(T->Font->Font);
			sprintf(String, "%s", T->Text);
			String_Char_Count = T->Length;
			
			Display_Font_Print_String(Coordinates_Object.X, Coordinates_Object.Y, String, String_Char_Count, T->Font->Character_Spacing, T->Color);
			break;

		case VALUE_BAR:
			V = (Object_Value_Bar*)Object->Data;
			Display_Draw_Value_Bar(&Coordinates_Object, V);
			break;

		case GRAPH:
			G = (Object_Graph*)Object->Data;
			Display_Draw_Graph(&Coordinates_Object, G);
			break;

		case BUTTON:
			N = (Object_Button*)Object->Data;
			Display_Draw_Button(&Coordinates_Object, N);
			break;

		case IMAGE:
			Display_Image_Draw_Color(Coordinates_Object.X, Coordinates_Object.Y, (Image_Color*)Object->Data);
			break;

		case BOOLEAN:
			B = (Object_Bool*)Object->Data;
			Display_Font_Set_Font(B->Font->Font);

			if(*B->Value == true)
			{
				sprintf(String, "%s", B->Text_True);
				String_Char_Count 	= B->Length_True;
				Color 				= B->Color_True;
			}
			else
			{
				sprintf(String, "%s", B->Text_False);
				String_Char_Count 	= B->Length_False;
				Color 				= B->Color_False;
			}

			Display_Font_Print_String(Coordinates_Object.X, Coordinates_Object.Y, String, String_Char_Count, B->Font->Character_Spacing, Color);
			break;

		case SHAPE:
			S = (Object_Shape*)Object->Data;

			switch (S->Type)
			{
			case RECTANGLE:
				Display_Shapes_Draw_Rect_Filled(Coordinates_Object.X, Coordinates_Object.Y, S->Dimension.Width, S->Dimension.Height, S->Color);
				break;
			}
			break;

		case CANVAS:
			C = (Object_Canvas*)(Object->Data);
			Display_Draw_Canvas(&Coordinates_Object, C);
			break;

		case MESSAGE_BOX:
			M = (Object_Message_Box*)(Object->Data);

			Width = Object->Dimension.Width;
			if(Style != NULL) {
				Width -= ((Style->Border_Thickness << 1) + Style->Padding[PADDING_LEFT] + Style->Padding[PADDING_RIGHT]);
			}

			Display_Draw_Message_Box(&Coordinates_Object, M, Width);
			break;

		default:
			break;
		}
	}

	if(_Draw_Touch_Reference_Points) {
		Display_Draw_Touch_Refernce_Points(DISPLAY_COLOR_GREEN);
	}

	if(_Draw_Center_Lines) {
		Display_Draw_Center_Lines(DISPLAY_COLOR_GREENYELLOW);
	}

	Display_Draw_Touch_Marker(DISPLAY_COLOR_BLUE);
}

void Display_Send_Buffer(void)
{
	Display_SPI_Start_Command(DISPLAY_MEMORY_WRITE);
	Display_SPI_Send_Data((uint8_t *)_Image_Buffer.Dim_1, DISPLAY_IMAGE_BUFFER_BYTE_SIZE, true);
}

bool Display_Send_Buffer_Completed(void)
{
	return Display_SPI_DMA_Transfer_Completed();
}

void Display_Show_Test_Screen(void)
{
	int16_t Width = DISPLAY_WIDTH / 5;
	
	Display_Shapes_Draw_Rect_Filled(0*Width, 0, Width, DISPLAY_HEIGHT, DISPLAY_COLOR_RED);
	Display_Shapes_Draw_Rect_Filled(1*Width, 0, Width, DISPLAY_HEIGHT, DISPLAY_COLOR_GREEN);
	Display_Shapes_Draw_Rect_Filled(2*Width, 0, Width, DISPLAY_HEIGHT, DISPLAY_COLOR_BLUE);
	Display_Shapes_Draw_Rect_Filled(3*Width, 0, Width, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK);
	Display_Shapes_Draw_Rect_Filled(4*Width, 0, Width, DISPLAY_HEIGHT, DISPLAY_COLOR_WHITE);
}

int	Display_Get_Button_Touch_Return_Value(void)
{
	int Return_Value = _Touched_Button_Return_Value;

	_Touched_Button_Return_Value = -1;

	return Return_Value;
}

void Display_Select_First_Object(void)
{
	Display_Action_CW();
}

void Display_Action_CW(void)
{
	if(_Object_Selected == false)
	{
		(*_Screen_Display_On_Objects_Defocused)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
		Display_Object_Select_Next();
		(*_Screen_Display_On_Objects_Focused)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
	else
	{
		(*_Screen_Display_Action_CW)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
}

void Display_Action_CCW(void)
{
	if(_Object_Selected == false)
	{
		(*_Screen_Display_On_Objects_Defocused)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
		Display_Object_Select_Previous();
		(*_Screen_Display_On_Objects_Focused)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
	else
	{
		(*_Screen_Display_Action_CCW)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
}

void Display_Action_SW(void)
{
	if(_Object_Selected == false)
	{
		_Object_Selected = true;
		(*_Screen_Display_On_Object_Select)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
	else
	{
		_Object_Selected = false;
		(*_Screen_Display_On_Object_Deselect)(Display_Get_Index_Of_Object(Display_Get_Selected_Object()));
	}
}

void Display_Select_Object(void)
{
	_Object_Selected = true;
}

void Display_Unselect_Object(void)
{
	_Object_Selected = false;
}

void Display_Inc_Frame_Counter(void)
{
	_Frame_Counter++;
}

uint* Display_Get_Frame_Counter_Reference(void)
{
	return &_Frame_Counter;
}

void Display_Set_Debug_Print(void)
{
	_Debug_Print = true;
}


/*******************************************************************
	Internal Functions
*******************************************************************/
void Display_Draw_Style(Coordinates* coordinates, Style* style, uint content_width, uint content_height, bool do_draw)
{
	/*
		INFO (2021-11-11):
		Width and Height information from the Style-Struct is currently not used. The dimension is stored in the Object
		itself and does not change during runtime. A width or height adapted to the contect size is currently not available.
		It is also not possible to align the content within the style. This has to be done by setting the Padding values
		properly.
		I am not sure if the above mentioned functionality is needed or will be used by at all. This is why I will not 
		implement these functions and keep things simple until I see I really need it.
	*/
	
	uint Height 					= content_height;
	uint Width 						= content_width;
	Display_Color Border_Color 	= style->Border_Color;
	uint Border_Thickness 			= style->Border_Thickness;
	uint Border_Radius				= style->Border_Radius;

	if(!do_draw) {
		return;
	}
	
	if(style->Background_Color != Display_Objects_Background_Color_Get()) {
		if(Border_Radius > 0) {
			Display_Shapes_Draw_Round_Rect_Filled(coordinates->X + Border_Thickness, coordinates->Y + Border_Thickness, Width - 2 * Border_Thickness, Height - 2 * Border_Thickness, Border_Radius, style->Background_Color);
		} else {
			Display_Shapes_Draw_Rect_Filled(coordinates->X + Border_Thickness, coordinates->Y + Border_Thickness, Width - 2 * Border_Thickness, Height - 2 * Border_Thickness, style->Background_Color);
		}
	}

	if(style->Border_Color != Display_Objects_Background_Color_Get() && style->Border_Thickness > 0) {
		Display_Shapes_Draw_Round_Rect_Frame(coordinates->X, coordinates->Y, Width, Height, Border_Radius, Border_Thickness, Border_Color);
	}
}

void Display_Draw_Value_Bar(Coordinates* coordinates, Object_Value_Bar* value_bar)
{
	Object_Value_Bar* V = value_bar;
	
	if(*V->Value >= V->Max)
	{
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y, V->Dimension.Width, V->Dimension.Height, V->Color);
		return;
	}

	if(*V->Value < V->Min)
	{
		return;
	}

	float Positive_Factor = ((float)(*V->Value - V->Min)) / ((float)(V->Max - V->Min));

	uint16_t Target_Value = V->Dimension.Width;
	if(V->Orientation == BOTTOM_TO_TOP || V->Orientation == TOP_TO_BOTTOM)
	{
		Target_Value = V->Dimension.Height;
	}

	uint16_t Positive_Part = Target_Value * Positive_Factor;
	uint16_t Negative_Part = Target_Value - Positive_Part;

	switch (V->Orientation)
	{
	case LEFT_TO_RIGHT:	
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y, Positive_Part, V->Dimension.Height, V->Color);
		break;

	case RIGHT_TO_LEFT:
		Display_Shapes_Draw_Rect_Filled(coordinates->X + Negative_Part, coordinates->Y, Positive_Part, V->Dimension.Height, V->Color);
		break;

	case BOTTOM_TO_TOP:
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y + Negative_Part, V->Dimension.Width, Positive_Part, V->Color);
		break;
	
	case TOP_TO_BOTTOM:
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y, V->Dimension.Width, Positive_Part, V->Color);
		break;
	}
}

void Display_Draw_Graph(Coordinates* coordinates, Object_Graph* graph)
{
	uint16_t 	Value;
	uint16_t	Y;
	
	for(int i=0;i<graph->Data_Length;i++)
	{
		Value = graph->Data[i];

		if(Value < graph->Min) {
			Value = graph->Min;
		}

		if(Value > graph->Max) {
			Value = graph->Max;
		}

		Y = (graph->Dimension.Height * (Value - graph->Min)) / (graph->Max - graph->Min);
		Y = graph->Dimension.Height - Y;

		Display_Shapes_Draw_Pixel(coordinates->X + i, coordinates->Y + Y, graph->Color);
	}
}

void Display_Draw_Button(Coordinates* coordinates, Object_Button* button)
{
	int16_t X = coordinates->X;
	int16_t Y = coordinates->Y;
	
	Display_Shapes_Draw_HLine(X, Y		, button->Dimension.Width  - 1, 1, DISPLAY_COLOR_WHITE);
	Display_Shapes_Draw_VLine(X, Y+1	, button->Dimension.Height - 2, 1, DISPLAY_COLOR_WHITE);

	Display_Shapes_Draw_HLine(X+1, Y+1	, button->Dimension.Width  - 3, 1, DISPLAY_COLOR(216, 216, 216));
	Display_Shapes_Draw_VLine(X+1, Y+2	, button->Dimension.Height - 4, 1, DISPLAY_COLOR(216, 216, 216));

	if(DISPLAY_COLOR(184, 184, 184) != Display_Objects_Background_Color_Get()) {
		Display_Shapes_Draw_Rect_Filled(X+2, Y+2, button->Dimension.Width - 4, button->Dimension.Height - 4, DISPLAY_COLOR(184, 184, 184));
	}

	Display_Font_Set_Font(button->Font->Font);
	uint16_t Text_Width 	= Display_Font_Width_String(button->Text, button->Length, 2);
	uint16_t Text_Height 	= Display_Font_Get_Font_Height();
	int16_t Text_X			= X + (button->Dimension.Width - Text_Width) / 2;
	int16_t Text_Y			= Y + (button->Dimension.Height - Text_Height) / 2;
	Display_Font_Print_String(Text_X, Text_Y, button->Text, button->Length, button->Font->Character_Spacing, button->Color);

	Display_Shapes_Draw_HLine(X+1, Y+button->Dimension.Height-2, button->Dimension.Width  - 2, 1, DISPLAY_COLOR(120, 120, 120));
	Display_Shapes_Draw_VLine(X+button->Dimension.Width-2, Y+1	, button->Dimension.Height - 3, 1, DISPLAY_COLOR(120, 120, 120));

	Display_Shapes_Draw_HLine(X, Y+button->Dimension.Height-1, button->Dimension.Width		, 1, DISPLAY_COLOR_BLACK);
	Display_Shapes_Draw_VLine(X+button->Dimension.Width-1, Y, button->Dimension.Height - 1	, 1, DISPLAY_COLOR_BLACK);
}

void Display_Draw_Canvas(Coordinates* coordinates, Object_Canvas* canvas)
{
	int16_t X = coordinates->X;
	int16_t Y = coordinates->Y;

	for(uint y=0;y<canvas->Dimension.Height;y++)
	{
		for(uint x=0;x<canvas->Dimension.Width;x++)
		{
			Display_Shapes_Draw_Pixel(X+x, Y+y, canvas->Data[y * canvas->Dimension.Width + x]);
		}
	}
}

void Display_Draw_Message_Box(Coordinates* coordinates, Object_Message_Box* message_box, uint16_t width)
{
	Object_Message_Box* M = message_box;
	char String[64];

	if(M->Show_Ticks_Left > 0)
	{
		Display_Font_Set_Font(M->Font->Font);
		sprintf(String, "%s", M->Text);
		int String_Char_Count = M->Length;
		
		Coordinates Coordinates_Offset_Text = { 0, 0 };
		Coordinates Coordinates_Offset_Bar = { 0, 0 };
		Coordinates_Offset_Bar.Y = MESSAGE_BOX_TEXT_BAR_DISTANCE + Display_Font_Get_Font_Height();

		if(M->Icon != MESSAGE_BOX_ICON_NONE) {
			Coordinates_Offset_Text.X = MESSAGE_BOX_TEXT_ICON_DISTANCE + Display_Message_Box_Icons_Get_Icon_Width(M->Icon);
			Display_Image_Draw_Alpha(coordinates->X, coordinates->Y, Display_Message_Box_Icons_Get_Icon_Ptr(M->Icon), M->Color);

			if(Display_Message_Box_Icons_Get_Icon_Height(M->Icon) > Display_Font_Get_Font_Height()) {
				Coordinates_Offset_Text.Y = (Display_Message_Box_Icons_Get_Icon_Height(M->Icon) - Display_Font_Get_Font_Height()) >> 1;
				Coordinates_Offset_Bar.Y = MESSAGE_BOX_TEXT_BAR_DISTANCE + Display_Message_Box_Icons_Get_Icon_Height(M->Icon);
			}
		}
		
		Display_Font_Print_String(coordinates->X + Coordinates_Offset_Text.X, coordinates->Y + Coordinates_Offset_Text.Y, String, String_Char_Count, M->Font->Character_Spacing, M->Color);
		
		uint16_t Bar_Width = (width * M->Show_Ticks_Left) / M->Show_Ticks_Max;
		Display_Shapes_Draw_Rect_Filled(coordinates->X + width - Bar_Width, coordinates->Y + Coordinates_Offset_Bar.Y, Bar_Width, MESSAGE_BOX_BAR_HEIGHT, M->Color);

		M->Show_Ticks_Left--;
	}
}

void Display_Draw_Focused(Coordinates* coordinates, uint width, uint height)
{
	if(_Object_Selected == true)
	{
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y, 3, 3, DISPLAY_COLOR_RED);
	}
	else
	{
		Display_Shapes_Draw_Rect_Filled(coordinates->X, coordinates->Y, 3, 3, DISPLAY_COLOR_GREEN);
	}
}

Animation_State Display_Animation_Tick(Display_Object* object)
{
	Animation* 			Animation 			= object->Animation;
	Animation_Status* 	Animation_Status 	= object->Animation_Status;

	if(Animation == NULL || Animation_Status == NULL) {
		return COMPLETE;
	}
	
	switch (Animation_Status->State)
	{
	case NO_STARTED:
		Animation_Status->Target.X = object->Coordinates.X;
		Animation_Status->Target.Y = object->Coordinates.Y;

//		break;	// Skipped on purpose
	
	case DELAYING:
		if(Animation_Status->State == DELAYING) {
			Animation_Status->Tick_Counter--;
		} else {
			Animation_Status->Tick_Counter 	= Animation->Tick_Delay;
			Animation_Status->State 		= DELAYING;
		}

		if(Animation_Status->Tick_Counter > 0) {
			break;
		}

//		break;	// Skipped on purpose

	case MOVING:
		if(Animation_Status->State == MOVING) {
			Animation_Status->Tick_Counter--;
		} else {
			Animation_Status->Tick_Counter 	= Animation->Tick_Duration;
			Animation_Status->State 		= MOVING;
		}

		float Progress = (float)Animation_Status->Tick_Counter / (float)Animation->Tick_Duration;

		object->Coordinates.X = Animation_Status->Target.X + (int16_t)((float)Animation->Offset.X * Progress);
		object->Coordinates.Y = Animation_Status->Target.Y + (int16_t)((float)Animation->Offset.Y * Progress);

		if(Animation_Status->Tick_Counter == 0) {
			Animation_Status->State = COMPLETE;
		}
		break;

	case COMPLETE:
	default:
		Animation_Status->State = COMPLETE;
		break;
	}

	return Animation_Status->State;
}

void Display_Draw_Touch_Refernce_Points(Display_Color color)
{
	for(int i=0;i<Display_Touch_Get_Reference_Point_Count();i++)
	{
		Coordinates* Point = Display_Touch_Get_Reference_Point(i);
		if(Point != NULL) {
			Display_Shapes_Draw_Circle_Filled(Point->X-5, Point->Y-5, 5, color);
		}
	}
}

void Display_Draw_Touch_Marker(Display_Color color)
{
	int Radius = 5;
	uint Marker_Count = Display_Touch_Get_Marker_Count();

	for(uint i=0;i<Marker_Count;i++) {
		Coordinates* Coordinates = Display_Touch_Get_Marker_Coordinates(i);
		if(Coordinates == NULL) {
			continue;
		}

		Display_Shapes_Draw_Circle_Filled(Coordinates->X-Radius, Coordinates->Y-Radius, Radius, color);
	}
}

void Display_Draw_Center_Lines(Display_Color color)
{
	Display_Shapes_Draw_HLine(0, DISPLAY_HEIGHT/2 - 1, DISPLAY_WIDTH, 2, color);
	Display_Shapes_Draw_VLine(DISPLAY_WIDTH/2 - 1, 0, DISPLAY_HEIGHT, 2, color);
}

void Display_Check_Button_Touch(int16_t x, int16_t y)
{
	for(uint i=0;i<Display_Objects_Count();i++)
	{
		Display_Object* 		Object 		= Display_Objects_Get_By_ID(i);

		if(Object->Type != BUTTON) { continue; }

		Coordinates		Coordinates = Object->Coordinates;
		Style*			Style		= Object->Style;
		Object_Button*	N			= (Object_Button*)Object->Data;

		if(Style != NULL) {
			Display_Draw_Style(&Coordinates, Style, N->Dimension.Width, N->Dimension.Height, false);
		}

		if(x >= Coordinates.X && x <= Coordinates.X + N->Dimension.Width && y >= Coordinates.Y && y <= Coordinates.Y + N->Dimension.Height)
		{
			_Touched_Button_Return_Value = N->Return_Value;
		}
	}
}

void Display_Object_Select_Next(void)
{
	Display_Object* Current_Object = Display_Get_Selected_Object();
	int Current_Object_Index = Display_Get_Index_Of_Object(Current_Object);

	if(Current_Object_Index == -1)
	{
		Current_Object_Index = 0;
		Current_Object = Display_Objects_Get_By_ID(Current_Object_Index);
		if(Current_Object->Selectable == true)
		{
			Current_Object->Focused = true;
			return;
		}
	}

	for(uint i=0;i<Display_Objects_Count();i++)
	{
		uint Index = i + Current_Object_Index + 1;
		if(Index >= Display_Objects_Count())
		{
			Index -= Display_Objects_Count();
		}

		Display_Object* Next_Possible_Object = Display_Objects_Get_By_ID(Index);

		if(Next_Possible_Object->Selectable == true)
		{
			Current_Object->Focused = false;
			Next_Possible_Object->Focused = true;
			return;
		}
	}
}

void Display_Object_Select_Previous(void)
{
	Display_Object* Current_Object = Display_Get_Selected_Object();
	int Current_Object_Index = Display_Get_Index_Of_Object(Current_Object);

	if(Current_Object_Index == -1)
	{
		Current_Object_Index = 0;
	}

	for(uint i=0;i<Display_Objects_Count();i++)
	{
		uint Index = (Display_Objects_Count() - i - 1) + Current_Object_Index;
		if(Index >= Display_Objects_Count())
		{
			Index -= Display_Objects_Count();
		}

		Display_Object* Next_Possible_Object = Display_Objects_Get_By_ID(Index);

		if(Next_Possible_Object->Selectable == true)
		{
			Current_Object->Focused = false;
			Next_Possible_Object->Focused = true;
			return;
		}
	}

	if(Current_Object->Selectable == true)
	{
		Current_Object->Focused = true;
	}
}

Display_Object* Display_Get_Selected_Object(void)
{
	for(uint i=0;i<Display_Objects_Count();i++)
	{
		Display_Object* Object = Display_Objects_Get_By_ID(i);
		if(Object->Selectable == true && Object->Focused == true)
		{
			return Object;
		}
	}

	return NULL;
}

int	Display_Get_Index_Of_Object(Display_Object* object)
{
	if(object == NULL) { return -1; }

	for(uint i=0;i<Display_Objects_Count();i++)
	{
		if(Display_Objects_Get_By_ID(i) == object)
		{
			 return i;
		}
	}
}

void Display_Screen_Transition_Tick()
{
	if(_Screen_Transition_Direction == TRANSITION_NONE) {
		return;
	}

	switch (_Screen_Transition_Direction)
	{
	case TRANSITION_LEFT:
		if(_Screen_Transition_Step < SCREEN_TRANSITION_STEP_COUNT_HORIZONTAL) {
			Display_Buffer_Shift_Left(_Screen_Transition_Steps_Horizontal[_Screen_Transition_Step]);
			_Screen_Transition_Offset.X -= _Screen_Transition_Steps_Horizontal[_Screen_Transition_Step];
			_Screen_Transition_Step++;
		}
		else if(_Screen_Transition_Offset.X > 0) {
			Display_Buffer_Shift_Left(1);
			_Screen_Transition_Offset.X--;
		}
		else {
			_Screen_Transition_Offset.X = 0;
			_Screen_Transition_Direction = TRANSITION_NONE;
		}
		break;

	case TRANSITION_RIGHT:
		if(_Screen_Transition_Step < SCREEN_TRANSITION_STEP_COUNT_HORIZONTAL) {
			Display_Buffer_Shift_Right(_Screen_Transition_Steps_Horizontal[_Screen_Transition_Step]);
			_Screen_Transition_Offset.X += _Screen_Transition_Steps_Horizontal[_Screen_Transition_Step];
			_Screen_Transition_Step++;
		}
		else if(_Screen_Transition_Offset.X < 0) {
			Display_Buffer_Shift_Right(1);
			_Screen_Transition_Offset.X++;
		}
		else {
			_Screen_Transition_Offset.X = 0;
			_Screen_Transition_Direction = TRANSITION_NONE;
		}
		break;

	case TRANSITION_UP:
		if(_Screen_Transition_Step < SCREEN_TRANSITION_STEP_COUNT_VERTICAL) {
			Display_Buffer_Shift_Up(_Screen_Transition_Steps_Vertical[_Screen_Transition_Step]);
			_Screen_Transition_Offset.Y -= _Screen_Transition_Steps_Vertical[_Screen_Transition_Step];
			_Screen_Transition_Step++;
		}
		else if(_Screen_Transition_Offset.Y > 0) {
			Display_Buffer_Shift_Up(1);
			_Screen_Transition_Offset.Y--;
		}
		else {
			_Screen_Transition_Offset.Y = 0;
			_Screen_Transition_Direction = TRANSITION_NONE;
		}
		break;
	
	case TRANSITION_DOWN:
		if(_Screen_Transition_Step < SCREEN_TRANSITION_STEP_COUNT_VERTICAL) {
			Display_Buffer_Shift_Down(_Screen_Transition_Steps_Vertical[_Screen_Transition_Step]);
			_Screen_Transition_Offset.Y += _Screen_Transition_Steps_Vertical[_Screen_Transition_Step];
			_Screen_Transition_Step++;
		}
		else if(_Screen_Transition_Offset.Y < 0) {
			Display_Buffer_Shift_Down(1);
			_Screen_Transition_Offset.Y++;
		}
		else {
			_Screen_Transition_Offset.Y = 0;
			_Screen_Transition_Direction = TRANSITION_NONE;
		}
		break;

	default:
		break;
	}
}

void Display_Buffer_Shift_Left(uint steps)
{
	for(int16_t Row=0;Row<DISPLAY_HEIGHT;Row++)
	{
		int16_t Col = 0;
		for(;Col<DISPLAY_WIDTH-steps;Col++)
		{
			_Image_Buffer.Dim_2[Col][Row] = _Image_Buffer.Dim_2[Col+steps][Row];
		}

		for(;Col<DISPLAY_WIDTH;Col++)
		{
			_Image_Buffer.Dim_2[Col][Row] = Display_Objects_Background_Color_Get();
		}
	}
}

void Display_Buffer_Shift_Right(uint steps)
{
	for(int16_t Row=0;Row<DISPLAY_HEIGHT;Row++)
	{
		int16_t Col = DISPLAY_WIDTH-1;
		for(;Col>steps;Col--)
		{
			_Image_Buffer.Dim_2[Col][Row] = _Image_Buffer.Dim_2[Col-steps][Row];
		}

		for(;Col>=0 && Col<DISPLAY_WIDTH;Col--)
		{
			_Image_Buffer.Dim_2[Col][Row] = Display_Objects_Background_Color_Get();
		}
	}
}

void Display_Buffer_Shift_Up(uint steps)
{
	for(int16_t Col=0;Col<DISPLAY_WIDTH;Col++)
	{
		int16_t Row = 0;
		for(;Row<DISPLAY_HEIGHT-steps;Row++)
		{
			_Image_Buffer.Dim_2[Col][Row] = _Image_Buffer.Dim_2[Col][Row+steps];
		}

		for(;Row<DISPLAY_HEIGHT;Row++)
		{
			_Image_Buffer.Dim_2[Col][Row] = Display_Objects_Background_Color_Get();
		}
	}
}

void Display_Buffer_Shift_Down(uint steps)
{
	for(int16_t Col=0;Col<DISPLAY_WIDTH;Col++)
	{
		int16_t Row = DISPLAY_HEIGHT-1;
		for(;Row>steps;Row--)
		{
			_Image_Buffer.Dim_2[Col][Row] = _Image_Buffer.Dim_2[Col][Row-steps];
		}

		for(;Row>=0 && Row<DISPLAY_HEIGHT;Row--)
		{
			_Image_Buffer.Dim_2[Col][Row] = Display_Objects_Background_Color_Get();
		}
	}
}