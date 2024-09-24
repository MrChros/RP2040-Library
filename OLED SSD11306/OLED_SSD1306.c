/*
 * OLED_SSD1306.c
 *
 * Created: 22/01/2020 18:36:34
 *  Author: Chris
 */ 

// ============================================================================================
// Includes
#include "OLED_SSD1306.h"

#include <math.h>
#include <string.h>
#include "hardware/divider.h"

#include "I2C_Master.h"


// ============================================================================================
// Defines
#define SSD1306_I2C_ADDRESS								0x3C	//0x3C or 0x3D
#define SSD1306_DATA									0xC0
#define SSD1306_DATA_CONTINUE							0x40

#define SSD1306_COMMAND									0x00
#define SSD1306_SET_CONTRAST_CONTROL					0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME					0xA4
#define SSD1306_DISPLAY_ALL_ON							0xA5
#define SSD1306_NORMAL_DISPLAY							0xA6
#define SSD1306_INVERT_DISPLAY							0xA7
#define SSD1306_DISPLAY_OFF								0xAE
#define SSD1306_DISPLAY_ON								0xAF
#define SSD1306_NOP										0xE3

#define SSD1306_HORIZONTAL_SCROLL_RIGHT					0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT					0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT	0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT		0x2A
#define SSD1306_DEACTIVATE_SCROLL						0x2E
#define SSD1306_ACTIVATE_SCROLL							0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA3

#define SSD1306_SET_LOWER_COLUMN						0x00
#define SSD1306_SET_HIGHER_COLUMN						0x10
#define SSD1306_MEMORY_ADDR_MODE						0x20
#define SSD1306_SET_COLUMN_ADDR							0x21
#define SSD1306_SET_PAGE_ADDR							0x22

#define SSD1306_SET_START_LINE							0x40
#define SSD1306_SET_SEGMENT_REMAP						0xA0
#define SSD1306_SET_MULTIPLEX_RATIO						0xA8
#define SSD1306_COM_SCAN_DIR_INC						0xC0
#define SSD1306_COM_SCAN_DIR_DEC						0xC8
#define SSD1306_SET_DISPLAY_OFFSET						0xD3
#define SSD1306_SET_COM_PINS							0xDA
#define SSD1306_CHARGE_PUMP								0x8D

#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO				0xD5
#define SSD1306_SET_PRECHARGE_PERIOD					0xD9
#define SSD1306_SET_VCOM_DESELECT						0xDB

#define SSD1306_PIXEL_WIDTH								128

#if SSD1306_HEIGHT == SSD1306_HEIGHT_32PX
	#define SSD1306_PIXEL_HEIGHT							32
#elif SSD1306_HEIGHT == SSD1306_HEIGHT_64PX
	#define SSD1306_PIXEL_HEIGHT							64
#else
	#error SSD1306: Illegal display height definition
#endif

#define SSD1306_PAGE_HEIGHT								8
#define SSD1306_PAGE_COUNT								SSD1306_PIXEL_HEIGHT / SSD1306_PAGE_HEIGHT
#define SSD1306_ANIMATION_STEP_COUNT_HORIZONTAL			sizeof(_Animation_Horizontal_Steps)
#define SSD1306_ANIMATION_STEP_COUNT_VERTICAL			sizeof(_Animation_Vertical_Steps)

#define SWAP_INT16_T(_A_, _B_) 							{ int16_t t = _A_; _A_ = _B_; _B_ = t; }


// ============================================================================================
// Typedef
typedef uint16_t Display_Data;


// ============================================================================================
// Variables
volatile static bool		_Invert_Enabled;
volatile static bool	 	_Update_Pending;

struct OLED_SSD1306_BUFFER
{
	Display_Data Command;
	union Display_Data
	{
		Display_Data Dim_2[SSD1306_PAGE_COUNT][SSD1306_PIXEL_WIDTH];
		Display_Data Dim_1[SSD1306_PAGE_COUNT * SSD1306_PIXEL_WIDTH];
	} Data;
};

static struct OLED_SSD1306_BUFFER 	_Display_Buffer;
static const unsigned char*			_Font;
static uint							_Screen_Transition_Direction;
static int16_t						_Screen_Transition_Offset_X;
static int16_t						_Screen_Transition_Offset_Y;
static Screen_Transition_Type 		_Screen_Transition_Type;
static uint 						_Screen_Transition_Duration;
static uint							_Screen_Transition_Step;
static uint							_Screen_Transition_Position;

static const uint8_t FONT_DEF_LENGTH	   	= 0;	
static const uint8_t FONT_DEF_FIXED_WIDTH 	= 2;
static const uint8_t FONT_DEF_HEIGHT      	= 3;
static const uint8_t FONT_DEF_FIRST_CHAR  	= 4;
static const uint8_t FONT_DEF_CHAR_COUNT  	= 5;
static const uint8_t FONT_DEF_WIDTH_TABLE 	= 6;


// ============================================================================================
// Function Declarations
void SSD1306_Set_Pixel_Column(int16_t x, int16_t y, uint8_t pixel_column);
void SSD1306_Set_Pixel(int16_t x, int16_t y, bool value);
bool SSD1306_Get_Pixel(int16_t x, int16_t y);
void SSD1306_Send_Command(uint8_t command);
void SSD1306_Set_Cursor(uint8_t column, uint8_t page);

void SSD1306_Buffer_Shift_Left(uint steps);
void SSD1306_Buffer_Shift_Right(uint steps);
void SSD1306_Buffer_Shift_Up(uint steps);
void SSD1306_Buffer_Shift_Down(uint steps);

uint64_t SSD1306_Buffer_Column2UINT64(uint column_number);
void SSD1306_Buffer_UINT642Column(uint column_number, uint64_t buffer_column);

void SSD1306_Draw_Line_Thin(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

float SSD1306_Transition_Linear(float x);

float SSD1306_Transition_Ease_In_Sine(float x);
float SSD1306_Transition_Ease_Out_Sine(float x);
float SSD1306_Transition_Ease_InOut_Sine(float x);

float SSD1306_Transition_Ease_In_Quad(float x);
float SSD1306_Transition_Ease_Out_Quad(float x);
float SSD1306_Transition_Ease_InOut_Quad(float x);

float SSD1306_Transition_Ease_In_Cubic(float x);
float SSD1306_Transition_Ease_Out_Cubic(float x);
float SSD1306_Transition_Ease_InOut_Cubic(float x);

float SSD1306_Transition_Ease_In_Quart(float x);
float SSD1306_Transition_Ease_Out_Quart(float x);
float SSD1306_Transition_Ease_InOut_Quart(float x);

float SSD1306_Transition_Ease_In_Quint(float x);
float SSD1306_Transition_Ease_Out_Quint(float x);
float SSD1306_Transition_Ease_InOut_Quint(float x);

float SSD1306_Transition_Ease_In_Expo(float x);
float SSD1306_Transition_Ease_Out_Expo(float x);
float SSD1306_Transition_Ease_InOut_Expo(float x);

float SSD1306_Transition_Ease_In_Circ(float x);
float SSD1306_Transition_Ease_Out_Circ(float x);
float SSD1306_Transition_Ease_InOut_Circ(float x);


/*******************************************************************
	Public Functions
*******************************************************************/
void SSD1306_Init(bool do_rotate_180)
{
	_Font							= NULL;
	_Invert_Enabled 				= false;
	_Update_Pending					= false;
	_Screen_Transition_Direction 	= TRANSITION_NONE;
	_Screen_Transition_Offset_X		= 0;
	_Screen_Transition_Offset_Y		= 0;
	_Screen_Transition_Type			= TRANSITION_LINEAR;
	_Screen_Transition_Duration		= 0;
	_Screen_Transition_Step			= 0;
	_Screen_Transition_Position		= 0;

	sleep_ms(20);
	SSD1306_Send_Command(SSD1306_DISPLAY_OFF);
	SSD1306_Send_Command(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
	SSD1306_Send_Command(0x80);
	SSD1306_Send_Command(SSD1306_SET_MULTIPLEX_RATIO);
	SSD1306_Send_Command(0x1F | SSD1306_HEIGHT);			// 0x3F for 64px; 0x1F for 32px
	SSD1306_Send_Command(SSD1306_SET_DISPLAY_OFFSET);
	SSD1306_Send_Command(0x00);
	SSD1306_Send_Command(SSD1306_SET_START_LINE | 0x0);
//	SSD1306_Send_Command(SSD1306_CHARGE_PUMP);
//	SSD1306_Send_Command(0x14);
	SSD1306_Send_Command(SSD1306_MEMORY_ADDR_MODE);
	SSD1306_Send_Command(0x00);
	SSD1306_Send_Command(SSD1306_SET_SEGMENT_REMAP | (uint8_t)(!do_rotate_180));

	if(do_rotate_180 == true) {
		SSD1306_Send_Command(SSD1306_COM_SCAN_DIR_INC);
	} else {
		SSD1306_Send_Command(SSD1306_COM_SCAN_DIR_DEC);
	}

	SSD1306_Send_Command(SSD1306_SET_COM_PINS);
	SSD1306_Send_Command(0x02 | (SSD1306_HEIGHT >> 1));	// 0x12 for 64px; 0x02 for 32px
	SSD1306_Send_Command(SSD1306_SET_CONTRAST_CONTROL);
	SSD1306_Send_Command(0xCF);
	SSD1306_Send_Command(SSD1306_SET_PRECHARGE_PERIOD);
	SSD1306_Send_Command(0xF1);
	SSD1306_Send_Command(SSD1306_SET_VCOM_DESELECT);
	SSD1306_Send_Command(0x40);
	SSD1306_Send_Command(SSD1306_DISPLAY_ALL_ON_RESUME);
	SSD1306_Send_Command(SSD1306_NORMAL_DISPLAY);


	SSD1306_Send_Command(SSD1306_CHARGE_PUMP);
	SSD1306_Send_Command(0x14);
	sleep_ms(20);
	
	SSD1306_Send_Command(SSD1306_DISPLAY_ON);
	sleep_ms(20);
}

bool SSD1306_Get_Update_Pending()
{
	return _Update_Pending;
}

void SSD1306_Transmit_Buffer()
{
	SSD1306_Set_Cursor(0, 0);

	_Display_Buffer.Command = SSD1306_DATA_CONTINUE;

	I2CM_Transmit(SSD1306_I2C_ADDRESS, (Display_Data*)&_Display_Buffer, sizeof(_Display_Buffer)/sizeof(Display_Data), true, false);

	_Update_Pending = false;
}

void SSD1306_Transmit_Page(uint8_t start_column, uint8_t length, uint8_t page)
{
	if(page			>= SSD1306_PAGE_COUNT ) { return; }
	if(start_column >= SSD1306_PIXEL_WIDTH) { return; }
	
	if(start_column + length > SSD1306_PIXEL_WIDTH)
	{
		length = SSD1306_PIXEL_WIDTH - start_column;
	}

	SSD1306_Set_Cursor(start_column, page);

	if(page == 0 && start_column == 0)
	{
		_Display_Buffer.Command = SSD1306_DATA_CONTINUE;
		I2CM_Transmit(SSD1306_I2C_ADDRESS, (Display_Data*)&_Display_Buffer, length+1, true, false);
	}
	else
	{
		uint	Index_Before_Data	= page * SSD1306_PIXEL_WIDTH + start_column - 1;
		uint8_t Temp_Column			= _Display_Buffer.Data.Dim_1[Index_Before_Data];
		_Display_Buffer.Data.Dim_1[Index_Before_Data] = SSD1306_DATA_CONTINUE;

		I2CM_Transmit(SSD1306_I2C_ADDRESS, (Display_Data*)&_Display_Buffer.Data.Dim_1[Index_Before_Data], length+1, true, false);
		
		_Display_Buffer.Data.Dim_1[Index_Before_Data] = Temp_Column;
	}
}

bool SSD1306_Transmit_Completed()
{
	return I2CM_DMA_Transmit_Complete();
}

void SSD1306_Transition_Start(uint direction, Screen_Transition_Type type, uint frame_duration)
{
	_Screen_Transition_Offset_X 	= 0;
	_Screen_Transition_Offset_Y 	= 0;
	_Screen_Transition_Direction 	= TRANSITION_NONE;
	_Screen_Transition_Type 		= type;
	_Screen_Transition_Duration 	= frame_duration;
	_Screen_Transition_Step 		= 0;
	_Screen_Transition_Position 	= 0;
	
	if(_Screen_Transition_Duration == 0) {
		return;
	}
	
	switch (direction)
	{
	case TRANSITION_DIRECTION_LEFT:
		_Screen_Transition_Offset_X		= SSD1306_PIXEL_WIDTH;
		_Screen_Transition_Direction 	= TRANSITION_DIRECTION_LEFT;
		break;
	
	case TRANSITION_DIRECTION_RIGHT:
		_Screen_Transition_Offset_X 	= -SSD1306_PIXEL_WIDTH;
		_Screen_Transition_Direction 	= TRANSITION_DIRECTION_RIGHT;
		break;

	case TRANSITION_DIRECTION_UP:
		_Screen_Transition_Offset_Y		= SSD1306_PIXEL_HEIGHT;
		_Screen_Transition_Direction 	= TRANSITION_DIRECTION_UP;
		break;
	
	case TRANSITION_DIRECTION_DOWN:
		_Screen_Transition_Offset_Y		= -SSD1306_PIXEL_HEIGHT;
		_Screen_Transition_Direction 	= TRANSITION_DIRECTION_DOWN;
		break;

	default:
		break;
	}
}

void SSD1306_Transition_Tick()
{
	if(_Screen_Transition_Direction == TRANSITION_NONE) {
		return;
	}
	float Transition_X = ((float)_Screen_Transition_Step) / ((float)_Screen_Transition_Duration);
	float New_Percent = 0.0f;
	
	switch (_Screen_Transition_Type)
	{
	case TRANSITION_LINEAR: 			New_Percent = SSD1306_Transition_Linear(Transition_X); 				break;

	case TRANSITION_EASE_IN_SINE: 		New_Percent = SSD1306_Transition_Ease_In_Sine(Transition_X); 		break;
	case TRANSITION_EASE_OUT_SINE: 		New_Percent = SSD1306_Transition_Ease_Out_Sine(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_SINE: 	New_Percent = SSD1306_Transition_Ease_InOut_Sine(Transition_X); 	break;

	case TRANSITION_EASE_IN_QUAD: 		New_Percent = SSD1306_Transition_Ease_In_Quad(Transition_X); 		break;
	case TRANSITION_EASE_OUT_QUAD: 		New_Percent = SSD1306_Transition_Ease_Out_Quad(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_QUAD: 	New_Percent = SSD1306_Transition_Ease_InOut_Quad(Transition_X); 	break;

	case TRANSITION_EASE_IN_CUBIC: 		New_Percent = SSD1306_Transition_Ease_In_Cubic(Transition_X); 		break;
	case TRANSITION_EASE_OUT_CUBIC: 	New_Percent = SSD1306_Transition_Ease_Out_Cubic(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_CUBIC: 	New_Percent = SSD1306_Transition_Ease_InOut_Cubic(Transition_X); 	break;

	case TRANSITION_EASE_IN_QUART: 		New_Percent = SSD1306_Transition_Ease_In_Quart(Transition_X); 		break;
	case TRANSITION_EASE_OUT_QUART: 	New_Percent = SSD1306_Transition_Ease_Out_Quart(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_QUART: 	New_Percent = SSD1306_Transition_Ease_InOut_Quart(Transition_X); 	break;

	case TRANSITION_EASE_IN_QUINT: 		New_Percent = SSD1306_Transition_Ease_In_Quint(Transition_X); 		break;
	case TRANSITION_EASE_OUT_QUINT: 	New_Percent = SSD1306_Transition_Ease_Out_Quint(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_QUINT: 	New_Percent = SSD1306_Transition_Ease_InOut_Quint(Transition_X); 	break;

	case TRANSITION_EASE_IN_EXPO: 		New_Percent = SSD1306_Transition_Ease_In_Expo(Transition_X); 		break;
	case TRANSITION_EASE_OUT_EXPO: 		New_Percent = SSD1306_Transition_Ease_Out_Expo(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_EXPO: 	New_Percent = SSD1306_Transition_Ease_InOut_Expo(Transition_X); 	break;

	case TRANSITION_EASE_IN_CIRC: 		New_Percent = SSD1306_Transition_Ease_In_Circ(Transition_X); 		break;
	case TRANSITION_EASE_OUT_CIRC: 		New_Percent = SSD1306_Transition_Ease_Out_Circ(Transition_X); 		break;
	case TRANSITION_EASE_INOUT_CIRC: 	New_Percent = SSD1306_Transition_Ease_InOut_Circ(Transition_X); 	break;
	
	default:
		_Screen_Transition_Offset_X = 0;
		_Screen_Transition_Offset_Y = 0;
		_Screen_Transition_Direction = TRANSITION_NONE;
		return;
	}

	int16_t Position_Target = 0;

	switch (_Screen_Transition_Direction)
	{
		case TRANSITION_DIRECTION_LEFT:
		case TRANSITION_DIRECTION_RIGHT:
			Position_Target = SSD1306_PIXEL_WIDTH;
			break;

		case TRANSITION_DIRECTION_UP:
		case TRANSITION_DIRECTION_DOWN:
			Position_Target = SSD1306_HEIGHT;
			break;

		default:
			_Screen_Transition_Offset_X = 0;
			_Screen_Transition_Offset_Y = 0;
			_Screen_Transition_Direction = TRANSITION_NONE;
			return;
	}

	int16_t New_Position = (uint)(New_Percent * Position_Target);
	int16_t Shift_Step = New_Position - _Screen_Transition_Position;

	switch (_Screen_Transition_Direction)
	{
		case TRANSITION_DIRECTION_LEFT:		SSD1306_Buffer_Shift_Left(Shift_Step);	_Screen_Transition_Offset_X -= Shift_Step;	break;
		case TRANSITION_DIRECTION_RIGHT:	SSD1306_Buffer_Shift_Right(Shift_Step);	_Screen_Transition_Offset_X += Shift_Step;	break;
		case TRANSITION_DIRECTION_UP:		SSD1306_Buffer_Shift_Up(Shift_Step);	_Screen_Transition_Offset_Y -= Shift_Step;	break;
		case TRANSITION_DIRECTION_DOWN:		SSD1306_Buffer_Shift_Down(Shift_Step);	_Screen_Transition_Offset_Y += Shift_Step;	break;
	}

	if(New_Position >= Position_Target) {
		_Screen_Transition_Offset_X = 0;
		_Screen_Transition_Offset_Y = 0;
		_Screen_Transition_Direction = TRANSITION_NONE;
	}

	_Screen_Transition_Step++;
	_Screen_Transition_Position = New_Position;




// 	switch (_Animation_Direction)
// 	{
// 	case ANIMATION_DIRECTION_LEFT:
// 		if(_Animation_Step < SSD1306_ANIMATION_STEP_COUNT_HORIZONTAL) {
// 			SSD1306_Buffer_Shift_Left(_Animation_Horizontal_Steps[_Animation_Step]);
// 			_Animation_Offset_X -= _Animation_Horizontal_Steps[_Animation_Step];
// 			_Animation_Step++;
// 		} else if(_Animation_Offset_X > 0) {
// 			SSD1306_Buffer_Shift_Left(1);
// 			_Animation_Offset_X -= 1;
// 		} else {
// 			_Animation_Direction 	= ANIMATION_NONE;
// 			_Animation_Offset_X 	= 0;
// 		}
// 		break;

// 	case ANIMATION_DIRECTION_RIGHT:
// 		if(_Animation_Step < SSD1306_ANIMATION_STEP_COUNT_HORIZONTAL) {
// 			SSD1306_Buffer_Shift_Right(_Animation_Horizontal_Steps[_Animation_Step]);
// 			_Animation_Offset_X += _Animation_Horizontal_Steps[_Animation_Step];
// 			_Animation_Step++;
// 		} else if(_Animation_Offset_X < 0) {
// 			SSD1306_Buffer_Shift_Right(1);
// 			_Animation_Offset_X += 1;
// 		} else {
// 			_Animation_Direction 	= ANIMATION_NONE;
// 			_Animation_Offset_X 	= 0;
// 		}
// 		break;

// 	case ANIMATION_DIRECTION_UP:
// 		if(_Animation_Step < SSD1306_ANIMATION_STEP_COUNT_VERTICAL) {
// 			SSD1306_Buffer_Shift_Up(_Animation_Vertical_Steps[_Animation_Step]);
// 			_Animation_Offset_Y -= _Animation_Vertical_Steps[_Animation_Step];
// 			_Animation_Step++;
// 		} else if(_Animation_Offset_Y > 0) {
// 			SSD1306_Buffer_Shift_Up(1);
// 			_Animation_Offset_Y -= 1;
// 		} else {
// 			_Animation_Direction = ANIMATION_NONE;
// 			_Animation_Offset_Y = 0;
// 		}
// 		break;

// 	case ANIMATION_DIRECTION_DOWN:
// 		if(_Animation_Step < SSD1306_ANIMATION_STEP_COUNT_VERTICAL) {
// 			SSD1306_Buffer_Shift_Down(_Animation_Vertical_Steps[_Animation_Step]);
// 			_Animation_Offset_Y += _Animation_Vertical_Steps[_Animation_Step];
// 			_Animation_Step++;
// 		} else if(_Animation_Offset_Y < 0) {
// 			SSD1306_Buffer_Shift_Down(1);
// 			_Animation_Offset_Y += 1;
// 		} else {
// 			_Animation_Direction = ANIMATION_NONE;
// 			_Animation_Offset_Y = 0;
// 		}
// 		break;

// 	default:
// 		break;
// 	}
}

bool SSD1306_Transition_Ongoing()
{
	return _Screen_Transition_Direction != TRANSITION_NONE;
}

void SSD1306_Set_Font(const uint8_t* font)
{
	_Font = (uint8_t*)font;
}

uint SSD1306_Get_Font_Width(void)
{
	if(_Font == NULL) {
		return 0;
	}

	return _Font[FONT_DEF_FIXED_WIDTH];
}

uint SSD1306_Get_Font_Height(void)
{
	if(_Font == NULL) {
		return 0;
	}

	return _Font[FONT_DEF_HEIGHT];
}

void SSD1306_Set_Invert_Enabled(bool invert_enabled)
{
	_Invert_Enabled = invert_enabled;
}

uint SSD1306_Get_Width(void)
{
	return SSD1306_PIXEL_WIDTH;
}

uint SSD1306_Get_Height(void)
{
	return SSD1306_PIXEL_HEIGHT;
}

uint16_t SSD1306_Get_Display_Data(int16_t page, int16_t x)
{
	if(page < 0 || page >= SSD1306_PAGE_COUNT ) { return 0; }
	if(x 	< 0 || page >= SSD1306_PIXEL_WIDTH) { return 0; }

	return _Display_Buffer.Data.Dim_2[page][x];
}

void SSD1306_Clear(bool skip_while_animation)
{
	if(skip_while_animation == true && _Screen_Transition_Direction != TRANSITION_NONE) {
		return;
	}
	
	memset((void*)_Display_Buffer.Data.Dim_1, 0x00, SSD1306_PAGE_COUNT * SSD1306_PIXEL_WIDTH * sizeof(Display_Data));

	_Update_Pending = true;
}

void SSD1306_Fill_Pattern(int16_t x, int16_t y, uint length, uint8_t pattern)
{
	for(int16_t i=0;i<length;i++)
	{
		SSD1306_Set_Pixel_Column(x + i, y, pattern);
	}

	_Update_Pending = true;
}

void SSD1306_Fill_Page_Array(int16_t x, int16_t y, uint length, uint8_t* array)
{
	for(int16_t i=0;i<length;i++)
	{
		SSD1306_Set_Pixel_Column(x + i, y, array[i]);
	}

	_Update_Pending = true;
}

void SSD1306_Write_Text(int16_t x, int16_t y, char* text, uint8_t length)
{
	if(_Font == NULL) { return; }

	uint8_t Font_Width	= _Font[FONT_DEF_FIXED_WIDTH];
	uint8_t Font_Height	= _Font[FONT_DEF_HEIGHT];
	uint8_t Font_Offset = _Font[FONT_DEF_FIRST_CHAR];

	uint8_t Font_Page_Height = 1;
	while((Font_Page_Height * 8) < Font_Height) { Font_Page_Height++; }

	int16_t Write_X;

	for(uint8_t p=0;p<Font_Page_Height;p++)
	{
		Write_X = x;

		for(uint8_t c=0;c<length;c++)
		{
			uint16_t Font_Index = FONT_DEF_WIDTH_TABLE + (text[c] - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
			
			for(uint8_t j=0;j<Font_Width;j++)
			{
				SSD1306_Set_Pixel_Column(Write_X++, y + p * SSD1306_PAGE_HEIGHT, _Font[Font_Index + j]);
			}
		}
	}

	_Update_Pending = true;
}

void SSD1306_Write_Text_Centered(int16_t y, char* text, uint8_t length)
{
	if(_Font == NULL) { return; }

	int16_t Font_Width	= _Font[FONT_DEF_FIXED_WIDTH];
	int16_t Total_Width = Font_Width * length;

	int16_t Column = (SSD1306_PIXEL_WIDTH - Total_Width) >> 1;

	SSD1306_Write_Text(Column, y, text, length);
}

void SSD1306_Write_Int_Dec(int16_t x, int16_t y, int32_t val, uint8_t numbers, bool display_signed, bool skip_leading_zero)
{
	if(_Font == NULL) { return; }

	uint8_t Font_Width	= _Font[FONT_DEF_FIXED_WIDTH];
	uint8_t Font_Height	= _Font[FONT_DEF_HEIGHT];
	uint8_t Font_Offset = _Font[FONT_DEF_FIRST_CHAR];

	uint8_t Leading_Zero = skip_leading_zero;

	uint8_t Font_Page_Height = 1;
	while((Font_Page_Height * 8) < Font_Height) { Font_Page_Height++; }

	int16_t Write_X;

	for(uint8_t p=0;p<Font_Page_Height;p++)
	{
		int32_t val_internal = val;
		uint8_t i, num;
		uint factor = 1;

		Write_X = x;

		if(display_signed == true)
		{
			uint16_t Font_Index;
			if(val_internal >= 0)
			{
				Font_Index = FONT_DEF_WIDTH_TABLE + ('+' - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
			}
			else
			{
				Font_Index = FONT_DEF_WIDTH_TABLE + ('-' - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
				val_internal = val_internal * -1;
			}

			for(uint8_t j=0;j<Font_Width;j++)
			{
				SSD1306_Set_Pixel_Column(Write_X++, y + p * SSD1306_PAGE_HEIGHT, _Font[Font_Index + j]);
			}
		}
	
		for(i=0;i<numbers-1;i++)
		{
			factor *= 10;
		}
	
		for(i=0;i<numbers;i++)
		{
			num = val_internal / factor;

			uint16_t Font_Index;

			if(num == 0 && Leading_Zero == true && i < (numbers - 1))
			{
				Font_Index = FONT_DEF_WIDTH_TABLE + (' ' - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
			}
			else
			{
				Leading_Zero = false;
				Font_Index = FONT_DEF_WIDTH_TABLE + ('0' + num - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
			}

			for(uint8_t j=0;j<Font_Width;j++)
			{
				SSD1306_Set_Pixel_Column(Write_X++, y + p * SSD1306_PAGE_HEIGHT, _Font[Font_Index + j]);
			}

			val_internal %= factor;
			factor /= 10;
		}
	}

	_Update_Pending = true;
}

void SSD1306_Write_Int_Hex(int16_t x, int16_t y, uint val, uint8_t numbers, uint8_t write_0x)
{
	if(_Font == NULL) { return; }
	
	uint8_t Font_Width	= _Font[FONT_DEF_FIXED_WIDTH];
	uint8_t Font_Height	= _Font[FONT_DEF_HEIGHT];
	uint8_t Font_Offset = _Font[FONT_DEF_FIRST_CHAR];

	uint8_t Font_Page_Height = 1;
	while((Font_Page_Height * 8) < Font_Height) { Font_Page_Height++; }

	int16_t Write_X;

	for(uint8_t p=0;p<Font_Page_Height;p++)
	{
		uint8_t Hex_Number, i;

		Write_X = x;

		if(write_0x>0)
		{
			uint8_t Text_0x[2] = "0x";
			for(uint8_t c=0;c<2;c++)
			{
				uint16_t Font_Index = FONT_DEF_WIDTH_TABLE + (Text_0x[c] - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;
				
				for(uint8_t j=0;j<Font_Width;j++)
				{
					SSD1306_Set_Pixel_Column(Write_X++, y + p * SSD1306_PAGE_HEIGHT, _Font[Font_Index + j]);
				}
			}
		}

		for(i=0;i<numbers;i++)
		{
			Hex_Number = (uint8_t)(val >> ((numbers-i-1) * 4)) & 0x0F;
			
			if(Hex_Number<10)
				Hex_Number = '0' + Hex_Number;
			else
				Hex_Number = 'A' + (Hex_Number - 10);
			
			uint16_t Font_Index = FONT_DEF_WIDTH_TABLE + (Hex_Number - Font_Offset) * (Font_Width * Font_Page_Height) + p * Font_Width;

			for(uint8_t j=0;j<Font_Width;j++)
			{
				SSD1306_Set_Pixel_Column(Write_X++, y + p * SSD1306_PAGE_HEIGHT, _Font[Font_Index + j]);
			}
		}
	}

	_Update_Pending = true;
}

void SSD1306_Write_Unit(int16_t x, int16_t y, uint8_t numbers, uint8_t extra_space, char* text, uint8_t length)
{
	if(_Font == NULL) { return; }

	uint8_t Font_Width = _Font[FONT_DEF_FIXED_WIDTH];

	x += ((numbers * Font_Width) + extra_space);

	SSD1306_Write_Text(x, y, text, length);
}

void SSD1306_Draw_Bitmap(int16_t x, int16_t y, const uint8_t* bitmap)
{
	if(bitmap == NULL) { return; }

	int16_t Bitmap_Width  = bitmap[1];
	int16_t Bitmap_Height = bitmap[2];

	int16_t Bitmap_Page_Height = 1;
	while((Bitmap_Page_Height * 8) < Bitmap_Height) {
		Bitmap_Page_Height++;
	}

	for(int16_t page=0;page<Bitmap_Page_Height;page++)
	{
		for(int16_t col=0;col<Bitmap_Width;col++)
		{
			uint Bitmap_Byte_Index = 4 + page * Bitmap_Width + col;
			SSD1306_Set_Pixel_Column(x + col, y + page * 8, bitmap[Bitmap_Byte_Index]);
		}
	}
}

void SSD1306_Draw_Bitmap_Centered(int16_t y, const uint8_t* bitmap)
{
	if(bitmap == NULL) { return; }

	int16_t Bitmap_Width  = bitmap[1];
	int16_t Column = (SSD1306_PIXEL_WIDTH - Bitmap_Width) >> 1;

	SSD1306_Draw_Bitmap(Column, y, bitmap);
}

void SSD1306_Draw_Point(int16_t x, int16_t y)
{
	SSD1306_Set_Pixel(x, y, true);
}

void SSD1306_Draw_HLine(int16_t x, int16_t y, uint16_t width, uint16_t thickness)
{
	SSD1306_Draw_Rect_Filled(x, y, width, thickness);
}

void SSD1306_Draw_VLine(int16_t x, int16_t y, uint16_t height, uint16_t thickness)
{
	SSD1306_Draw_Rect_Filled(x, y, thickness, height);
}

void SSD1306_Draw_Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t thickness)
{
	int16_t Temp;

	if (thickness == 1)
	{
		SSD1306_Draw_Line_Thin(x0, y0, x1, y1);
		return;
	}

	if (y0 == y1)
	{
		if (x1 > x0)
		{
			SSD1306_Draw_HLine(x0, y0, x1 - x0 + 1, thickness);
		}
		else if (x1 < x0)
		{
			SSD1306_Draw_HLine(x1, y0, x0 - x1 + 1, thickness);
		}
		else
		{
			SSD1306_Draw_Circle_Filled(x0, y0, thickness);
		}
		return;
	}
	else if (x0 == x1)
	{
		if (y1 > y0)
		{
			SSD1306_Draw_VLine(x0, y0, y1 - y0 + 1, thickness);
		}
		else
		{
			SSD1306_Draw_VLine(x0, y1, y0 - y1 + 1, thickness);
		}
		return;
	}

	bool Steep = abs(y1 - y0) > abs(x1 - x0);
	if (Steep == true)
	{
		Temp = x0;
		x0 = y0;
		y0 = Temp;
		Temp = x1;
		x1 = y1;
		y1 = Temp;
	}
	if (x0 > x1)
	{
		Temp = x0;
		x0 = x1;
		x1 = Temp;
		Temp = y0;
		y0 = y1;
		y1 = Temp;
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
	if (Steep == true)
	{
		for (; x0 <= x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len > 1)
				{
					SSD1306_Draw_VLine(y0, xbegin, len + 1, thickness);
				}
				else
				{
					if (thickness == 1)
						SSD1306_Set_Pixel(y0, x0, true);
					else
						SSD1306_Draw_Circle_Filled(y0, x0, thickness >> 1);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		{
			SSD1306_Draw_VLine(y0, xbegin, x0 - xbegin, thickness);
		}
	}
	else
	{
		for (; x0 <= x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len > 1)
				{
					SSD1306_Draw_HLine(xbegin, y0, len + 1, thickness);
				}
				else
				{
					if (thickness == 1)
						SSD1306_Set_Pixel(x0, y0, true);
					else
						SSD1306_Draw_Circle_Filled(x0, y0, thickness >> 1);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		{
			SSD1306_Draw_HLine(xbegin, y0, x0 - xbegin, thickness);
		}
	}
}

void SSD1306_Draw_Rect_Frame(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t thickness)
{
	SSD1306_Draw_HLine(x, y, width, thickness);
	SSD1306_Draw_HLine(x, y + height, width + thickness, thickness);

	SSD1306_Draw_VLine(x, y, height, thickness);
	SSD1306_Draw_VLine(x + width, y, height + thickness, thickness);
}

void SSD1306_Draw_Rect_Filled(int16_t x, int16_t y, uint16_t width, uint16_t height)
{
	for (int16_t r = y; r < (y + height); r++)
	{
		for (int16_t c = x; c < (x + width); c++)
		{
			SSD1306_Set_Pixel(c, r, true);
		}
	}
}

void SSD1306_Draw_Circle_Frame(int16_t x, int16_t y, uint16_t radius, uint16_t thickness)
{
	x = x + radius;
	y = y + radius;

	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t X = 0;
	int16_t Y = radius;

	if (thickness == 1)
	{
		SSD1306_Set_Pixel(x, y + radius, true);
		SSD1306_Set_Pixel(x, y - radius, true);
		SSD1306_Set_Pixel(x + radius, y, true);
		SSD1306_Set_Pixel(x - radius, y, true);
	}
	else
	{
		SSD1306_Draw_Circle_Filled(x, y + radius, thickness >> 1);
		SSD1306_Draw_Circle_Filled(x, y - radius, thickness >> 1);
		SSD1306_Draw_Circle_Filled(x + radius, y, thickness >> 1);
		SSD1306_Draw_Circle_Filled(x - radius, y, thickness >> 1);
	}

	while (X < Y)
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

		if (thickness == 1)
		{
			SSD1306_Set_Pixel(x + X, y + Y, true);
			SSD1306_Set_Pixel(x - X, y + Y, true);
			SSD1306_Set_Pixel(x + X, y - Y, true);
			SSD1306_Set_Pixel(x - X, y - Y, true);
			SSD1306_Set_Pixel(x + Y, y + X, true);
			SSD1306_Set_Pixel(x - Y, y + X, true);
			SSD1306_Set_Pixel(x + Y, y - X, true);
			SSD1306_Set_Pixel(x - Y, y - X, true);
		}
		else
		{
			SSD1306_Draw_Circle_Filled(x + X, y + Y, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x - X, y + Y, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x + X, y - Y, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x - X, y - Y, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x + Y, y + X, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x - Y, y + X, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x + Y, y - X, thickness >> 1);
			SSD1306_Draw_Circle_Filled(x - Y, y - X, thickness >> 1);
		}
	}
}

void SSD1306_Draw_Circle_Filled(int16_t x, int16_t y, uint16_t radius)
{
	x = x + radius;
	y = y + radius;

	SSD1306_Draw_VLine(x, y - radius, 2 * radius + 1, 1);

	uint8_t Corner_Name = 3;
	int16_t Delta = 0;

	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t X = 0;
	int16_t Y = radius;

	while (X < Y)
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

		if ((Corner_Name & 0x1) > 0)
		{
			SSD1306_Draw_VLine(x + X, y - Y, 2 * Y + 1 + Delta, 1);
			SSD1306_Draw_VLine(x + Y, y - X, 2 * X + 1 + Delta, 1);
		}
		if ((Corner_Name & 0x2) > 0)
		{
			SSD1306_Draw_VLine(x - X, y - Y, 2 * Y + 1 + Delta, 1);
			SSD1306_Draw_VLine(x - Y, y - X, 2 * X + 1 + Delta, 1);
		}
	}
}

/*******************************************************************
	Internal Functions
*******************************************************************/
void SSD1306_Set_Pixel_Column(int16_t x, int16_t y, uint8_t pixel_column)
{
	x += _Screen_Transition_Offset_X;
	y += _Screen_Transition_Offset_Y;
	
	if(x < 0 						|| x >= SSD1306_PIXEL_WIDTH) 	{ return; }
	if(y < -(SSD1306_PAGE_HEIGHT-1) || y >= SSD1306_PIXEL_HEIGHT) 	{ return; } 

	int16_t Y_Overlap = y % SSD1306_PAGE_HEIGHT;

	if(Y_Overlap == 0)
	{
		if(_Invert_Enabled == true) { pixel_column = ~pixel_column; }
		
		_Display_Buffer.Data.Dim_2[y >> 3][x] = (Display_Data)pixel_column & 0x00FF;
	}
	else
	{
		x -= _Screen_Transition_Offset_X;
		y -= _Screen_Transition_Offset_Y;
		for(int16_t i;i<SSD1306_PAGE_HEIGHT;i++)
		{
			SSD1306_Set_Pixel(x, y+i, (pixel_column & (1 << i)) > 0);
		}
	}
}

void SSD1306_Set_Pixel(int16_t x, int16_t y, bool value)
{
	x += _Screen_Transition_Offset_X;
	y += _Screen_Transition_Offset_Y;

	if(x < 0 || x >= SSD1306_PIXEL_WIDTH) 	{ return; }
	if(y < 0 || y >= SSD1306_PIXEL_HEIGHT) 	{ return; } 

	uint Page 	= y >> 3;
	uint Offest = y % 8;

	if(_Invert_Enabled == true) { value = !value; }

	if(value)
	{
		_Display_Buffer.Data.Dim_2[Page][x] |= (((Display_Data)1 << Offest) & 0x00FF);
	}
	else
	{
		_Display_Buffer.Data.Dim_2[Page][x] &= (~((Display_Data)1 << Offest) & 0x00FF);
	}
}

bool SSD1306_Get_Pixel(int16_t x, int16_t y)
{
	if(x < 0 || x >= SSD1306_PIXEL_WIDTH) 	{ return false; }
	if(y < 0 || y >= SSD1306_PIXEL_HEIGHT) 	{ return false; } 

	divmod_result_t Result = hw_divider_divmod_u32((uint)y, SSD1306_PAGE_HEIGHT);
	uint Page 	= to_quotient_u32(Result);
	uint Offset = to_remainder_u32(Result);

	return ((_Display_Buffer.Data.Dim_2[Page][x] & ((Display_Data)1 << Offset)) > 0);
}

void SSD1306_Send_Command(uint8_t command)
{
	I2CM_Packet_Transmit(SSD1306_I2C_ADDRESS, SSD1306_COMMAND, 1, &command, 1);
}

void SSD1306_Set_Cursor(uint8_t column, uint8_t page)
{
	SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
	SSD1306_Send_Command(column);
	SSD1306_Send_Command(SSD1306_PIXEL_WIDTH-1);

	SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
	SSD1306_Send_Command(page);
	SSD1306_Send_Command(SSD1306_PAGE_COUNT-1);
}

void SSD1306_Buffer_Shift_Left(uint steps)
{
	for(uint page=0;page<SSD1306_PAGE_COUNT;page++)
	{
		uint col = 0;
		for(;col<SSD1306_PIXEL_WIDTH-steps;col++)
		{
			_Display_Buffer.Data.Dim_2[page][col] = _Display_Buffer.Data.Dim_2[page][col + steps] & 0x00FF;
		}
		
		for(;col<SSD1306_PIXEL_WIDTH;col++)
		{
			_Display_Buffer.Data.Dim_2[page][col] = 0;
		}
	}
}

void SSD1306_Buffer_Shift_Right(uint steps)
{
	for(uint page=0;page<SSD1306_PAGE_COUNT;page++)
	{
		uint col = SSD1306_PIXEL_WIDTH-1;
		for(;col>steps;col--)
		{
			_Display_Buffer.Data.Dim_2[page][col] = _Display_Buffer.Data.Dim_2[page][col - steps] & 0x00FF;
		}
		
		for(;col>=0 && col<SSD1306_PIXEL_WIDTH;col--)
		{
			_Display_Buffer.Data.Dim_2[page][col] = 0;
		}
	}
}

void SSD1306_Buffer_Shift_Up(uint steps)
{
	if(steps > SSD1306_PIXEL_HEIGHT) {
		steps = SSD1306_PIXEL_HEIGHT;
	}
	
	for(uint x=0;x<SSD1306_PIXEL_WIDTH;x++)
	{
		uint64_t Buffer_Column = SSD1306_Buffer_Column2UINT64(x);
		Buffer_Column >>= steps;
		SSD1306_Buffer_UINT642Column(x, Buffer_Column);
	}
}

void SSD1306_Buffer_Shift_Down(uint steps)
{
	if(steps > SSD1306_PIXEL_HEIGHT) {
		steps = SSD1306_PIXEL_HEIGHT;
	}
	
	for(uint x=0;x<SSD1306_PIXEL_WIDTH;x++)
	{
		uint64_t Buffer_Column = SSD1306_Buffer_Column2UINT64(x);
		Buffer_Column <<= steps;
		SSD1306_Buffer_UINT642Column(x, Buffer_Column);
	}
}

uint64_t SSD1306_Buffer_Column2UINT64(uint column_number)
{
	uint64_t Buffer_Column = 0;
	for(uint page=0;page<SSD1306_PAGE_COUNT;page++)
	{
		Buffer_Column |= ((uint64_t)(_Display_Buffer.Data.Dim_2[page][column_number] & 0x00FF)) << (8 * page); 
	}

	return Buffer_Column;
}

void SSD1306_Buffer_UINT642Column(uint column_number, uint64_t buffer_column)
{
	for(uint page=0;page<SSD1306_PAGE_COUNT;page++)
	{
		_Display_Buffer.Data.Dim_2[page][column_number] = (buffer_column >> (8 * page)) & 0x00000000000000FF;
	}
}

void SSD1306_Draw_Line_Thin(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		SWAP_INT16_T(x0, y0);
		SWAP_INT16_T(x1, y1);
	}

	if (x0 > x1)
	{
		SWAP_INT16_T(x0, x1);
		SWAP_INT16_T(y0, y1);
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

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			SSD1306_Set_Pixel(y0, x0, true);
		}
		else
		{
			SSD1306_Set_Pixel(x0, y0, true);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

float SSD1306_Transition_Linear(float x)
{
	if(x <= 1.0f) {
		return x;
	}

	return 1.0f;
}

float SSD1306_Transition_Ease_In_Sine(float x)
{
	if(x <= 1.0f) {
		return (float)( 1 - cos((x * M_PI) / 2.0f));
	}

	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Sine(float x)
{
	if(x <= 1.0f) {
		return (float)(sin((x * M_PI) / 2.0f));
	}

	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Sine(float x)
{
	if(x <= 1.0f) {
		return (float)(-(cos(M_PI * x) - 1.0) / 2.0);
	}

	return 1.0f;
}

float SSD1306_Transition_Ease_In_Quad(float x)
{
	if(x <= 1.0f) {
		return x * x;
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Quad(float x)
{
	if(x <= 1.0f) {
		return 1 - (1 - x) * (1 - x);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Quad(float x)
{
	if(x <= 1.0f) {
		return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_In_Cubic(float x)
{
	if(x <= 1.0f) {
		return x * x * x;
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Cubic(float x)
{
	if(x <= 1.0f) {
		return 1 - pow(1 - x, 3);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Cubic(float x)
{
	if(x <= 1.0f) {
		return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
	}
	
	return 1.0f;
}


float SSD1306_Transition_Ease_In_Quart(float x)
{
	if(x <= 1.0f) {
		return x * x * x * x;
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Quart(float x)
{
	if(x <= 1.0f) {
		return 1 - pow(1 - x, 4);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Quart(float x)
{
	if(x <= 1.0f) {
		return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
	}
	
	return 1.0f;
}


float SSD1306_Transition_Ease_In_Quint(float x)
{
	if(x <= 1.0f) {
		return x * x * x * x * x;
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Quint(float x)
{
	if(x <= 1.0f) {
		return 1 - pow(1 - x, 5);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Quint(float x)
{
	if(x <= 1.0f) {
		return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
	}
	
	return 1.0f;
}


float SSD1306_Transition_Ease_In_Expo(float x)
{
	if(x <= 1.0f) {
		return x == 0 ? 0 : pow(2, 10 * x - 10);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Expo(float x)
{
	if(x <= 1.0f) {
		return x == 1 ? 1 : 1 - pow(2, -10 * x);
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Expo(float x)
{
	if(x <= 1.0f) {
		return x == 0
			? 0
			: x == 1
			? 1
			: x < 0.5 ? pow(2, 20 * x - 10) / 2
			: (2 - pow(2, -20 * x + 10)) / 2;
	}
	
	return 1.0f;
}


float SSD1306_Transition_Ease_In_Circ(float x)
{
	if(x <= 1.0f) {
		return 1 - sqrt(1 - pow(x, 2));
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_Out_Circ(float x)
{
	if(x <= 1.0f) {
		return sqrt(1 - pow(x - 1, 2));
	}
	
	return 1.0f;
}

float SSD1306_Transition_Ease_InOut_Circ(float x)
{
	if(x <= 1.0f) {
		return x < 0.5
			? (1 - sqrt(1 - pow(2 * x, 2))) / 2
			: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
	}
	
	return 1.0f;
}