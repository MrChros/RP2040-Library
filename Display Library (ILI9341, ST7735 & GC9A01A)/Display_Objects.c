/*
 * Display_Objects.c
 *
 * Created: Fri Apr 02 2021 13:58:23
 *  Author Chris
 */
#include "Display.h"
#include "Display_Font.h"
#include "Display_Image.h"
#include "Display_Objects.h"
#include "Display_Message_Box_Icons.h"

#include <string.h>


// ============================================================================================
// Defines


// ============================================================================================
// Variables
volatile Display_Color _Background_Color;

struct
{
	Display_Object *Array;
	size_t Used;
	size_t Size;
} _Objects;

struct
{
	Style *Array;
	size_t Used;
	size_t Size;
} _Styles;

struct
{
	Animation *Array;
	size_t Used;
	size_t Size;
} _Animations;

struct
{
	Font *Array;
	size_t Used;
	size_t Size;
} _Fonts;


// ============================================================================================
// Function Declarations
Object_ID	Display_Objects_Add(Object_Type type, Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, void *data, Style_ID style_id, Animation_ID animation_id);
Style*		Display_Objects_Style_From_ID(Style_ID style_id);
Animation*	Display_Objects_Animation_From_ID(Animation_ID animation_id);
Font*		Display_Objects_Font_From_ID(Font_ID font_id);

void 		Display_Objects_Update_Coordinates_Offset_And_Dimension(Display_Object *object, Anchor_Point anchor_point, Coordinates_Type coordinates_type);
Dimension 	Display_Objects_Get_Content_Size(Display_Object *object);


/*******************************************************************
	Functions
*******************************************************************/
void Display_Objects_Init(Display_Color background_color)
{
	_Objects.Array = NULL;
	_Objects.Used = _Objects.Size = 0;

	_Styles.Array = NULL;
	_Styles.Used = _Styles.Size = 0;

	Display_Objects_Background_Color_Set(background_color);
}

void Display_Objects_Background_Color_Set(Display_Color color)
{
	_Background_Color = color;
}

Display_Color Display_Objects_Background_Color_Get(void)
{
	return _Background_Color;
}

void Display_Objects_Clear(void)
{
	Object_Canvas* C;
	
	for (uint i = 0; i < _Objects.Size; i++)
	{
		Display_Object* O = Display_Objects_Get_By_ID(i);
		switch (O->Type)
		{
		case FLOAT:
			free((Object_Float*)(O->Data));
			break;
		case INTEGER:
			free((Object_Integer*)(O->Data));
			break;
		case TEXT:
			free((Object_Text*)(O->Data));
			break;
		case VALUE_BAR:
			free((Object_Value_Bar*)(O->Data));
			break;
		case GRAPH:
			free((Object_Graph*)(O->Data));
			break;
		case BUTTON:
			free((Object_Button*)(O->Data));
			break;
		case IMAGE:
			// Nothing to free here...
			break;
		case BOOLEAN:
			free((Object_Bool*)(O->Data));
			break;
		case SHAPE:
			free((Object_Shape*)(O->Data));
			break;
		case CANVAS:
			C = (Object_Canvas*)(O->Data);
			free(C->Data);
			free(C);
		case MESSAGE_BOX:
			free((Object_Message_Box*)(O->Data));
			break;
		
		default:
			break;
		}
		O->Data = NULL;
		
		if (O->Animation_Status != NULL)
		{
			free(O->Animation_Status);
			O->Animation_Status = NULL;
		}
	}

	free(_Objects.Array);
	_Objects.Array = NULL;
	_Objects.Used = _Objects.Size = 0;

	free(_Styles.Array);
	_Styles.Array = NULL;
	_Styles.Used = _Styles.Size = 0;

	free(_Animations.Array);
	_Animations.Array = NULL;
	_Animations.Used = _Animations.Size = 0;

	free(_Fonts.Array);
	_Fonts.Array = NULL;
	_Fonts.Used = _Fonts.Size = 0;

	Display_Unselect_Object();
}

Style_ID Display_Objects_Add_Style(Display_Color background_color, Display_Color border_color, uint border_thickness, uint border_radius, uint padding_top, uint padding_right, uint padding_bottom, uint padding_left, float width_height_ratio)
{
	Style Style;
	Style.Background_Color 			= background_color;
	Style.Border_Color 				= border_color;
	Style.Border_Thickness 			= border_thickness;
	Style.Border_Radius 			= border_radius;
	Style.Padding[PADDING_TOP] 		= padding_top;
	Style.Padding[PADDING_LEFT] 	= padding_left;
	Style.Padding[PADDING_BOTTOM] 	= padding_bottom;
	Style.Padding[PADDING_RIGHT] 	= padding_right;
	Style.Width_Height_Ratio		= width_height_ratio;

	if (_Styles.Used == _Styles.Size)
	{
		_Styles.Size++;
		_Styles.Array = realloc(_Styles.Array, _Styles.Size * sizeof(Style));
	}

	_Styles.Array[_Styles.Used++] = Style;

	return (Style_ID)(_Styles.Used - 1);
}

Animation_ID Display_Objects_Add_Animation(int offset_x, int offset_y, uint tick_delay, uint tick_duration, Animation_Order start_condition)
{
	Animation Animation;
	Animation.Offset.X 			= offset_x;
	Animation.Offset.Y 			= offset_y;
	Animation.Tick_Delay 		= tick_delay;
	Animation.Tick_Duration 	= tick_duration;
	Animation.Animation_Start 	= start_condition;

	if (_Animations.Used == _Animations.Size)
	{
		_Animations.Size++;
		_Animations.Array = realloc(_Animations.Array, _Animations.Size * sizeof(Animation));
	}

	_Animations.Array[_Animations.Used++] = Animation;

	return (Animation_ID)(_Animations.Used - 1);
}

Font_ID	Display_Objects_Add_Font(const unsigned char *font, uint8_t character_spacing)
{
	Font Font;
	Font.Font = font;
	Font.Character_Spacing = character_spacing;

	if (_Fonts.Used == _Fonts.Size)
	{
		_Fonts.Size++;
		_Fonts.Array = realloc(_Fonts.Array, _Fonts.Size * sizeof(Font));
	}

	_Fonts.Array[_Fonts.Used++] = Font;

	return (Font_ID)(_Fonts.Used - 1);
}

size_t Display_Objects_Count(void)
{
	return _Objects.Size;
}

Display_Object *Display_Objects_Get_By_ID(Object_ID id)
{
	if (id < _Objects.Size)
	{
		return &(_Objects.Array[id]);
	}

	return NULL;
}

size_t Display_Objects_Get_Count_Canvas(void)
{
	size_t Canvas_Count = 0;

	for(uint i=0;i<Display_Objects_Count();i++)
	{
		if(Display_Objects_Get_By_ID(i)->Type == CANVAS)
		{
			Canvas_Count++;
		}
	}

	return Canvas_Count;
}

Object_ID Display_Objects_Get_ID_From_Canvas_Number(uint canvas_number)
{
	for(uint i=0;i<Display_Objects_Count();i++)
	{
		if(Display_Objects_Get_By_ID(i)->Type == CANVAS)
		{
			if(canvas_number == 0)
			{
				return i;
			}
			else
			{
				canvas_number--;
			}
		}
	}

	return -1;
}

Object_ID Display_Objects_Add_Float(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, float *value, char *format, Font_ID font, Display_Color color, Style_ID style_id, Animation_ID animation_id)
{
	Object_Float *Number = malloc(sizeof(Object_Float));
	Number->Font = Display_Objects_Font_From_ID(font);
	Number->Color = color;
	Number->Value = value;
	sprintf(Number->Format, "%s", format);

	return Display_Objects_Add(FLOAT, anchor_point, coordinates_type, x, y, selectable, (void *)Number, style_id, animation_id);
}

Object_ID Display_Objects_Add_Integer(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, int *value, char *format, Font_ID font, Display_Color color, Style_ID style_id, Animation_ID animation_id)
{
	Object_Integer *Number = malloc(sizeof(Object_Integer));
	Number->Font = Display_Objects_Font_From_ID(font);
	Number->Color = color;
	Number->Value = value;
	sprintf(Number->Format, "%s", format);

	return Display_Objects_Add(INTEGER, anchor_point, coordinates_type, x, y, selectable, (void *)Number, style_id, animation_id);
}

Object_ID Display_Objects_Add_Text(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, char *text, Font_ID font, Display_Color color, Style_ID style_id, Animation_ID animation_id)
{
	Object_Text *Text = malloc(sizeof(Object_Text));
	Text->Font 			= Display_Objects_Font_From_ID(font);
	Text->Color 		= color;
	Text->Length = sprintf(Text->Text, "%s", text);

	return Display_Objects_Add(TEXT, anchor_point, coordinates_type, x, y, selectable, (void *)Text, style_id, animation_id);
}

Object_ID Display_Objects_Add_Value_Bar(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, uint *value, uint max, uint min, Orientation orientation, Display_Color color, uint width, uint height, Style_ID style_id, Animation_ID animation_id)
{
	Object_Value_Bar *Bar = malloc(sizeof(Object_Value_Bar));
	Bar->Value = value;
	Bar->Max = max;
	Bar->Min = min;
	Bar->Orientation = orientation;
	Bar->Color = color;
	Bar->Dimension.Width = width;
	Bar->Dimension.Height = height;

	return Display_Objects_Add(VALUE_BAR, anchor_point, coordinates_type, x, y, selectable, (void *)Bar, style_id, animation_id);
}

Object_ID Display_Objects_Add_Graph(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, uint16_t *data, uint16_t data_length, uint max, uint min, Display_Color color, uint height, Style_ID style_id, Animation_ID animation_id)
{
	Object_Graph *Graph = malloc(sizeof(Object_Graph));
	Graph->Data = data;
	Graph->Data_Length = data_length;
	Graph->Max = max;
	Graph->Min = min;
	Graph->Color = color;
	Graph->Dimension.Width = data_length;
	Graph->Dimension.Height = height;

	return Display_Objects_Add(GRAPH, anchor_point, coordinates_type, x, y, selectable, (void *)Graph, style_id, animation_id);
}

Object_ID Display_Objects_Add_Button(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, char *text, Font_ID font, uint return_value, Display_Color color, uint width, uint height, Style_ID style_id, Animation_ID animation_id)
{
	Object_Button *Button = malloc(sizeof(Object_Button));
	Button->Font = Display_Objects_Font_From_ID(font);
	Button->Return_Value = return_value;
	Button->Color = color;
	Button->Length = sprintf(Button->Text, "%s", text);
	Button->Dimension.Height = height;
	Button->Dimension.Width = width;

	return Display_Objects_Add(BUTTON, anchor_point, coordinates_type, x, y, selectable, (void *)Button, style_id, animation_id);
}

Object_ID Display_Objects_Add_Image(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Image_Color* image, Style_ID style_id, Animation_ID animation_id)
{
	return Display_Objects_Add(IMAGE, anchor_point, coordinates_type, x, y, selectable, (void *)image, style_id, animation_id);
}

Object_ID Display_Objects_Add_Bool(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, bool* value, char* text_true, Display_Color color_true, char* text_false, Display_Color color_false, Font_ID font	, Style_ID style_id, Animation_ID animation_id)
{
	Object_Bool *Bool = malloc(sizeof(Object_Bool));
	Bool->Value			= value;
	Bool->Length_True	= sprintf(Bool->Text_True	, "%s", text_true);
	Bool->Length_False 	= sprintf(Bool->Text_False	, "%s", text_false);
	Bool->Color_True	= color_true;
	Bool->Color_False	= color_false;
	Bool->Font 			= Display_Objects_Font_From_ID(font);

	return Display_Objects_Add(BOOLEAN, anchor_point, coordinates_type, x, y, selectable, (void *)Bool, style_id, animation_id);
}

Object_ID Display_Objects_Add_Rectangle(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Display_Color color, uint width, uint height, Style_ID style_id, Animation_ID animation_id)
{
	Object_Shape *Shape = malloc(sizeof(Object_Shape));
	Shape->Type 			= RECTANGLE;
	Shape->Color 			= color;
	Shape->Dimension.Width 	= width;
	Shape->Dimension.Height = height;

	return Display_Objects_Add(SHAPE, anchor_point, coordinates_type, x, y, selectable, (void *)Shape, style_id, animation_id);
}

Object_ID Display_Objects_Add_Canvas(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, Display_Color color, uint width, uint height, Style_ID style_id, Animation_ID animation_id)
{
	Object_Canvas *Canvas = malloc(sizeof(Object_Canvas));
	Canvas->Dimension.Width = width;
	Canvas->Dimension.Height = height;
	Canvas->Data = (uint16_t*) malloc(width * height * sizeof(uint16_t));

	for(uint y=0;y<height;y++)
	{
		for(uint x=0;x<width;x++)
		{
			Canvas->Data[y * width + x] = color;
		}	
	}

	return Display_Objects_Add(CANVAS, anchor_point, coordinates_type, x, y, selectable, (void *)Canvas, style_id, animation_id);
}

Object_ID Display_Objects_Add_Message_Box(Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, char* text, Font_ID font, Object_Message_Box_Icon icon, Display_Color color, Style_ID style_id)
{
	Object_Message_Box *Message_Box = malloc(sizeof(Object_Message_Box));
	Message_Box->Length 			= sprintf(Message_Box->Text, "%s", text);
	Message_Box->Font 				= Display_Objects_Font_From_ID(font);
	Message_Box->Color 				= color;
	Message_Box->Icon				= icon;
	Message_Box->Show_Ticks_Left 	= 0;
	
	return Display_Objects_Add(MESSAGE_BOX, anchor_point, coordinates_type, x, y, NOT_SELECTABLE, (void *)Message_Box, style_id, NO_ANIMATION);
}

void Display_Objects_Update_Coordinates(Object_ID id, Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);
	
	Object->Coordinates.X = x;
	Object->Coordinates.Y = y;
	Display_Objects_Update_Coordinates_Offset_And_Dimension(Object, anchor_point, coordinates_type);
}

void Display_Objects_Update_Color(Object_ID id, Display_Color color)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);
	
	switch(Object->Type)
	{
		case FLOAT:
			((Object_Float*)(Object->Data))->Color = color;
			break;

		case INTEGER:
			((Object_Integer*)(Object->Data))->Color = color;
			break;

		case TEXT:
			((Object_Text*)(Object->Data))->Color = color;
			break;

		case VALUE_BAR:
			((Object_Value_Bar*)(Object->Data))->Color = color;
			break;

		case BUTTON:
			((Object_Button*)(Object->Data))->Color = color;
			break;

		case SHAPE:
			((Object_Shape*)(Object->Data))->Color = color;
			break;

		default:
			break;
	}
}

void Display_Objects_Update_Text(Object_ID id, char* text)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);
	
	Object_Text* Text;
	Object_Button* Button;
	
	switch(Object->Type)
	{
		case TEXT:
			Text = (Object_Text*)(Object->Data);
			Text->Length = sprintf(Text->Text, "%s", text);
			break;

		case BUTTON:
			Button = (Object_Button*)(Object->Data);
			Button->Length = sprintf(Button->Text, "%s", text);
			break;
		
		default:
			break;
	}
}

void Display_Objects_Update_Image(Object_ID id, Image_Color* new_image)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);

	if(Object == NULL || new_image == NULL)
	{
		return;
	}

	switch(Object->Type)
	{
		case IMAGE:
			Object->Data = (void *)new_image;
			break;
		default:
			break;
	}
}

void Display_Objects_Update_Max_Min(Object_ID id, uint max, uint min)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);

	if(Object == NULL)
	{
		return;
	}

	switch(Object->Type)
	{
		case VALUE_BAR:
			((Object_Value_Bar*)Object->Data)->Max = max;
			((Object_Value_Bar*)Object->Data)->Min = min;
			break;

		case GRAPH:
			((Object_Graph*)Object->Data)->Max = max;
			((Object_Graph*)Object->Data)->Min = min;
			break;
			
		default:
			break;
	}
}

int	Display_Objects_Update_Pixel(Object_ID id, int16_t x, int16_t y, Display_Color color)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);

	if(Object == NULL)
	{
		return OBJECT_NOT_FOUND;
	}

	Object_Canvas* C;

	switch(Object->Type)
	{
		case CANVAS:
			C = (Object_Canvas*)(Object->Data);

			if(x<0 || x>=C->Dimension.Width) { return X_OUT_OF_RANGE; }
			if(y<0 || y>=C->Dimension.Height) { return Y_OUT_OF_RANGE; }
			
			C->Data[y * C->Dimension.Width + x] = color;
			
			return CANVAS_UPDATE_OK;

		default:
			break;
	}

	return OBJECT_WRONG_TYPE;
}

void Display_Objects_Show_Message_Box(Object_ID id, uint ticks)
{
	Display_Object* Object = Display_Objects_Get_By_ID(id);

	if(Object == NULL) {
		return;
	}

	if (Object->Type != MESSAGE_BOX) {
		return;
	}

	Object_Message_Box* M = (Object_Message_Box*)(Object->Data);
	M->Show_Ticks_Max = ticks;
	M->Show_Ticks_Left = ticks;
}

/*******************************************************************
	Internal Functions
*******************************************************************/
Object_ID Display_Objects_Add(Object_Type type, Anchor_Point anchor_point, Coordinates_Type coordinates_type, int16_t x, int16_t y, bool selectable, void *data, Style_ID style_id, Animation_ID animation_id)
{
	Display_Object Object;
	Object.Type 			= type;
	Object.Coordinates.X 	= x;
	Object.Coordinates.Y 	= y;
	Object.Content_Offset.X = 0;
	Object.Content_Offset.Y = 0;
	Object.Dimension.Height	= 0;
	Object.Dimension.Width	= 0;
	Object.Selectable		= selectable;
	Object.Focused			= false;
	Object.Data 			= data;
	Object.Style 			= Display_Objects_Style_From_ID(style_id);
	Object.Animation 		= Display_Objects_Animation_From_ID(animation_id);
	Object.Animation_Status = NULL;

	Display_Objects_Update_Coordinates_Offset_And_Dimension(&Object, anchor_point, coordinates_type);

	if (Object.Animation != NULL)
	{
		Object.Animation_Status = malloc(sizeof(Animation_Status));
		Object.Animation_Status->State = NO_STARTED;
	}

	// _Objects->used is the number of used entries, because _Objects->array[_Objects->used++] updates _Objects->used only *after* the array has been accessed.
	// Therefore a->used can go up to a->size
	if (_Objects.Used == _Objects.Size)
	{
		_Objects.Size++;
		_Objects.Array = realloc(_Objects.Array, _Objects.Size * sizeof(Display_Object));
	}

	_Objects.Array[_Objects.Used++] = Object;

	return Display_Objects_Count() - 1;
}

Style *Display_Objects_Style_From_ID(Style_ID style_id)
{
	if (style_id >= 0 && style_id < _Styles.Used)
	{
		return &_Styles.Array[style_id];
	}

	return NULL;
}

Animation *Display_Objects_Animation_From_ID(Animation_ID animation_id)
{
	if (animation_id >= 0 && animation_id < _Animations.Used)
	{
		return &_Animations.Array[animation_id];
	}

	return NULL;
}

Font*	Display_Objects_Font_From_ID(Font_ID font_id)
{
	if (font_id >= 0 && font_id < _Fonts.Used)
	{
		return &_Fonts.Array[font_id];
	}

	return NULL;
}

void Display_Objects_Update_Coordinates_Offset_And_Dimension(Display_Object *object, Anchor_Point anchor_point, Coordinates_Type coordinates_type)
{
	if (object == NULL)
	{
		return;
	}

	///////////////////////////////////////
	// Coordinated from Percent to Pixel //
	///////////////////////////////////////
	Coordinates Coordinates_In_Px = { .X = object->Coordinates.X, .Y = object->Coordinates.Y };

	switch (coordinates_type)
	{
	case BOTH_IN_PIXEL:
		break;

	case X_IN_PIXEL_Y_IN_PERCENT:
		Coordinates_In_Px.Y = (DISPLAY_HEIGHT * Coordinates_In_Px.Y) / 100;
		break;

	case X_IN_PERCENT_Y_IN_PIXEL:
		Coordinates_In_Px.X = (DISPLAY_WIDTH * Coordinates_In_Px.X) / 100;
		break;

	case BOTH_IN_PERCENT:
		Coordinates_In_Px.X = (DISPLAY_WIDTH * Coordinates_In_Px.X) / 100;
		Coordinates_In_Px.Y = (DISPLAY_HEIGHT * Coordinates_In_Px.Y) / 100;
		break;

	default:
		break;
	}


	////////////////////////////////////////
	// Get Coordinated of Top-Left corner //
	////////////////////////////////////////
	Dimension Content_Size = Display_Objects_Get_Content_Size(object);

	Coordinates Coordinates_Content_Top_Left = Coordinates_In_Px;

	// Horizontal
	switch (anchor_point & 0x0F)
	{
	case CENTER:
		Coordinates_Content_Top_Left.X = Coordinates_In_Px.X - (Content_Size.Width >> 1);
		break;

	case LEFT:
		// Already assigned via default value
		// Coordinates_Content_Top_Left.X = Coordinates_In_Px.X;
		break;

	case RIGHT:
		Coordinates_Content_Top_Left.X = Coordinates_In_Px.X - Content_Size.Width;
		break;
	}

	// Vertical
	switch (anchor_point & 0xF0)
	{
	case MIDDLE:
		Coordinates_Content_Top_Left.Y = Coordinates_In_Px.Y - (Content_Size.Height >> 1);
		break;

	case TOP:
		// Already assigned via default value
		// Coordinates_Content_Top_Left.Y = Coordinates_In_Px.Y;
		break;

	case BOTTOM:
		Coordinates_Content_Top_Left.Y = Coordinates_In_Px.Y - Content_Size.Height;
		break;
	}

	Coordinates 	Offset_Content_Style 	= { .X = 0, .Y = 0 };
	Dimension		Object_Dimension 		= Content_Size;

	if(object->Style != NULL)
	{
		Style* Style = object->Style;
		
		//////////////////////////////////////////////////////////
		// !!! HEIGHT and WIDTH of Style not yet considered !!! //
		//////////////////////////////////////////////////////////
		
		Offset_Content_Style.X = Style->Border_Thickness + Style->Padding[PADDING_LEFT];
		Offset_Content_Style.Y = Style->Border_Thickness + Style->Padding[PADDING_TOP];

		Object_Dimension.Height += (Style->Border_Thickness << 1) + Style->Padding[PADDING_TOP] 	+ Style->Padding[PADDING_BOTTOM];
		Object_Dimension.Width 	+= (Style->Border_Thickness << 1) + Style->Padding[PADDING_LEFT] 	+ Style->Padding[PADDING_RIGHT];

		if(Style->Width_Height_Ratio != STYLE_WIDTH_HEIGHT_RATIO_AUTO)
		{
			uint16_t Ratio_Height = Object_Dimension.Width * Style->Width_Height_Ratio;
			int16_t Height_Difference = Ratio_Height - Object_Dimension.Height;

			Object_Dimension.Height = Ratio_Height;
			Offset_Content_Style.Y += (Height_Difference >> 1);
		}
	}

	object->Content_Offset 	= Offset_Content_Style;
	object->Coordinates.X 	= Coordinates_Content_Top_Left.X - Offset_Content_Style.X;
	object->Coordinates.Y 	= Coordinates_Content_Top_Left.Y - Offset_Content_Style.Y;
	object->Dimension		= Object_Dimension;
}

Dimension Display_Objects_Get_Content_Size(Display_Object *object)
{
	Dimension Dimension = { .Width = 0, .Height = 0 };

	if (object == NULL)
	{
		return Dimension;
	}

	void *Data = object->Data;
	char String[64];
	uint String_Char_Count;

	switch (object->Type)
	{
	case FLOAT:
		Display_Font_Set_Font(((Object_Float *)Data)->Font->Font);
		String_Char_Count = sprintf(String, ((Object_Float *)Data)->Format, *(((Object_Float *)Data)->Value));
		Dimension.Height = Display_Font_Get_Font_Height();
		Dimension.Width = Display_Font_Width_String(String, String_Char_Count, ((Object_Float *)Data)->Font->Character_Spacing);
		break;

	case INTEGER:
		Display_Font_Set_Font(((Object_Integer *)Data)->Font->Font);
		String_Char_Count = sprintf(String, ((Object_Integer *)Data)->Format, *(((Object_Integer *)Data)->Value));
		Dimension.Height = Display_Font_Get_Font_Height();
		Dimension.Width = Display_Font_Width_String(String, String_Char_Count, ((Object_Integer *)Data)->Font->Character_Spacing);
		break;

	case TEXT:
		Display_Font_Set_Font(((Object_Text *)Data)->Font->Font);
		Dimension.Height 	= Display_Font_Get_Font_Height();
		Dimension.Width 	= Display_Font_Width_String((((Object_Text *)Data)->Text), ((Object_Text *)Data)->Length, ((Object_Text *)Data)->Font->Character_Spacing);
		break;

	case VALUE_BAR:
		Dimension.Height = ((Object_Value_Bar *)Data)->Dimension.Height;
		Dimension.Width = ((Object_Value_Bar *)Data)->Dimension.Width;
		break;

	case GRAPH:
		Dimension.Height = ((Object_Graph *)Data)->Dimension.Height;
		Dimension.Width = ((Object_Graph *)Data)->Dimension.Width;
		break;

	case BUTTON:
		Dimension.Height = ((Object_Button *)Data)->Dimension.Height;
		Dimension.Width = ((Object_Button *)Data)->Dimension.Width;
		break;

	case IMAGE:
		Dimension.Height = Display_Image_Get_Height((Image_Color*)Data);
		Dimension.Width = Display_Image_Get_Width((Image_Color*)Data);
		break;

	case BOOLEAN:
		Display_Font_Set_Font(((Object_Bool *)Data)->Font->Font);
		Dimension.Height 	= Display_Font_Get_Font_Height();
		if(((Object_Bool *)Data)->Length_True >= ((Object_Bool *)Data)->Length_False)
		{
			Dimension.Width = Display_Font_Width_String((((Object_Bool *)Data)->Text_True), ((Object_Bool *)Data)->Length_True, ((Object_Bool *)Data)->Font->Character_Spacing);
		}
		else
		{
			Dimension.Width = Display_Font_Width_String((((Object_Bool *)Data)->Text_False), ((Object_Bool *)Data)->Length_False, ((Object_Bool *)Data)->Font->Character_Spacing);
		}
		break;

	case SHAPE:
		Dimension.Height = ((Object_Shape *)Data)->Dimension.Height;
		Dimension.Width = ((Object_Shape *)Data)->Dimension.Width;
		break;

	case CANVAS:
		Dimension.Height = ((Object_Canvas *)Data)->Dimension.Height;
		Dimension.Width = ((Object_Canvas *)Data)->Dimension.Width;
		break;

	case MESSAGE_BOX:
		Display_Font_Set_Font(((Object_Message_Box *)Data)->Font->Font);
		Dimension.Height 	= Display_Font_Get_Font_Height() + MESSAGE_BOX_TEXT_BAR_DISTANCE + MESSAGE_BOX_BAR_HEIGHT;
		Dimension.Width 	= Display_Font_Width_String((((Object_Message_Box *)Data)->Text), ((Object_Message_Box *)Data)->Length, ((Object_Message_Box *)Data)->Font->Character_Spacing);
		
		if((((Object_Message_Box *)Data)->Icon) != MESSAGE_BOX_ICON_NONE) {
			Dimension.Width += MESSAGE_BOX_TEXT_ICON_DISTANCE + Display_Message_Box_Icons_Get_Icon_Width(((Object_Message_Box *)Data)->Icon);

			if(Display_Message_Box_Icons_Get_Icon_Height(((Object_Message_Box *)Data)->Icon) > Display_Font_Get_Font_Height()) {
				Dimension.Height = Display_Message_Box_Icons_Get_Icon_Height(((Object_Message_Box *)Data)->Icon) + MESSAGE_BOX_TEXT_BAR_DISTANCE + MESSAGE_BOX_BAR_HEIGHT;
			}
		}
		
		break;

	default:
		break;
	}

	return Dimension;
}