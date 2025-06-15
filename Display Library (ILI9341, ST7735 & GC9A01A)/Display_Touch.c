/*
 * Display_Touch.c
 *
 * Created: Mon Jun 05 2023 2:39:04 PM
 *  Author: MrChros
 */
// ============================================================================================
// Includes
#include "Display_Touch.h"

#include "hardware/divider.h"

#include "Display.h"
#include "Display_SPI.h"
#include "Display_Config.h"


// ============================================================================================
// Defines
/////////////////////////////////
// Touch Communication Defines //
/////////////////////////////////
#define DISPLAY_TOUCH_SPI_BAUDRATE		2000000

#define DISPLAY_TOUCH_MUX_X				1
#define DISPLAY_TOUCH_MUX_Y				5
#define DISPLAY_TOUCH_MUX_Z1			3
#define DISPLAY_TOUCH_MUX_Z2			4

#define DISPLAY_TOUCH_RES_12BIT			1
#define DISPLAY_TOUCH_RES_8BIT			0

#define ACQ_MODE_DIFF					0
#define ACQ_MODE_SINGLE					1

#define SHIFT_START_BIT					7
#define SHIFT_MULTIPLEXER				4
#define SHIFT_RESOLUTION				3
#define SHIFT_ACQ_MODE					2
#define SHIFT_PD_REFERENCE				1	// PD = Power-Down
#define SHIFT_PD_ADC					0	// PD = Power-Down

#define OFF								0
#define ON								1

///////////////////////////////
// Touch Calculation Defines //
///////////////////////////////
#define POINT_TOUCH_REFERENCE_OFFSET 	30
#define POINT_TOUCH_REFERECE_COUNT		2


///////////////////////////
// Touch Marking Defines //
///////////////////////////
#define TOUCH_MARKER_COUNT				100
#define TOUCH_MARKER_DEFAULT_TICKS		50


// ============================================================================================
// Variables
static uint _X;
static uint _Y;
static uint _Z1;
static uint _Z2;
static float _Pressure;

static uint _Baudrate_Backup;
static bool _Display_Currently_Touched;

typedef struct {
	Coordinates Coordinates_Screen;
	Coordinates Coordinates_Touch;
} Touch_Reference_Point_s;
static Touch_Reference_Point_s _Touch_Reference_Points[POINT_TOUCH_REFERECE_COUNT];

typedef struct {
	float Slope;
	float Intersect;
} Touch_Conversion_Factor_s;
static Touch_Conversion_Factor_s _Touch_Conversion_Factors[POINT_TOUCH_REFERECE_COUNT];

typedef struct {
	Coordinates Coordinates;
	uint Tick_Duration;
} Touch_Marker_s;

typedef struct {
	Touch_Marker_s Marker[TOUCH_MARKER_COUNT];
	uint Fill_Count;
	uint Read_Start;
	uint Write_Next;
} Touch_Marker_Buffer_s;
static Touch_Marker_Buffer_s _Touch_Marker_Buffer;

// ============================================================================================
// Function Declarations
void 	Display_Touch_Marker_Add(int16_t x, int16_t y);
void	Display_Touch_Marker_Tick(void);

int16_t Display_Touch_Get_Screen_X_From_Touch_X(uint x_touch);
int16_t Display_Touch_Get_Screen_Y_From_Touch_Y(uint y_touch);
void 	Display_Touch_Update_Conversion_Factors(void);
bool 	Display_Touch_Is_Touched();

bool 	Display_Touch_Read_Sequential(uint times_read_each_value);
bool 	Display_Touch_Read_Overlapping(uint average_of_measurements);	
bool 	Display_Touch_Read_X();
bool 	Display_Touch_Read_Y();
bool 	Display_Touch_Read_Z1();
bool 	Display_Touch_Read_Z2();
bool 	Display_Touch_Read_Single_Parameter(uint mux, uint resolution, uint8_t *data);

uint8_t Display_Touch_Get_Control_Byte(uint mux, uint resolution);
uint	Display_Touch_Assemble_Value(uint8_t byte_low, uint8_t byte_high);

static inline void Display_Touch_SPI_CS_Select();
static inline void Display_Touch_SPI_CS_Deselect();


/*******************************************************************
	Interrupt Service Routines
*******************************************************************/


/*******************************************************************
	Functions
*******************************************************************/
void Display_Touch_Init()
{
	_X  = 0;
	_Y  = 0;
	_Z1 = 0;
	_Z2 = 0;
	_Pressure = 0;

	_Display_Currently_Touched = false;


	// Set Reference Point to be shown on Screen and used for later calculations
	_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.X 		= POINT_TOUCH_REFERENCE_OFFSET;
	_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.Y 		= POINT_TOUCH_REFERENCE_OFFSET;

	_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Screen.X 	= DISPLAY_WIDTH  - POINT_TOUCH_REFERENCE_OFFSET;
	_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Screen.Y 	= DISPLAY_HEIGHT - POINT_TOUCH_REFERENCE_OFFSET;

	Display_Touch_Set_Reference_Point(DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT	, 0, 0);
	Display_Touch_Set_Reference_Point(DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT, 1, 1);

	// Prefill corresponding Touch coordinates
	// Later to be read from a non-volatile memory 
	Display_Touch_Set_Reference_Point(DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT	,  720, 3376);
	Display_Touch_Set_Reference_Point(DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT, 3616,  656);

	Display_Touch_Update_Conversion_Factors();
	

	_Touch_Marker_Buffer.Fill_Count = 0;
	_Touch_Marker_Buffer.Read_Start	= 0;
	_Touch_Marker_Buffer.Write_Next	= 0;
	for(int i=0;i<TOUCH_MARKER_COUNT;i++) {
		_Touch_Marker_Buffer.Marker[i].Tick_Duration = 0;
	}


	// Initialize CS-Pin as regular GPIO
	gpio_init	(DISPLAY_SPI_PIN_CS_TOUCH);
    gpio_set_dir(DISPLAY_SPI_PIN_CS_TOUCH, GPIO_OUT);
    Display_Touch_SPI_CS_Deselect();
}

void Display_Touch_Set_Reference_Point(uint point, uint x_touch, uint y_touch)
{
	if(point >= POINT_TOUCH_REFERECE_COUNT) {
		return;
	}

	_Touch_Reference_Points[point].Coordinates_Touch.X = x_touch;
	_Touch_Reference_Points[point].Coordinates_Touch.Y = y_touch;
}

void Display_Touch_Detect(uint average_of_measurements)
{
	bool Success;

	Display_Touch_Marker_Tick();

	Success = Display_Touch_Read_Overlapping(average_of_measurements);
	// Success = Display_Touch_Read_Sequential(average_of_measurements);

	if(Success == false) {
		return;
	}

	if(Display_Touch_Is_Touched() == true) {
		int16_t X_Screen = Display_Touch_Get_Screen_X_From_Touch_X(_X);
		int16_t Y_Screen = Display_Touch_Get_Screen_Y_From_Touch_Y(_Y);
		
		if(X_Screen>=0 && X_Screen<DISPLAY_WIDTH && Y_Screen>=0 && Y_Screen<DISPLAY_HEIGHT) {
			Display_Touch_Marker_Add(X_Screen, Y_Screen);
			Display_Issue_Touch_Event(X_Screen, Y_Screen);
		}
	}

	if(_Display_Currently_Touched == false && Display_Touch_Is_Touched() == true) {
		_Display_Currently_Touched = true;
	} else if(_Display_Currently_Touched == true && Display_Touch_Is_Touched() == false) {
		_Display_Currently_Touched = false;
	}
}

int Display_Touch_Get_Reference_Point_Count()
{
	return sizeof(_Touch_Reference_Points)/sizeof(Touch_Reference_Point_s);	
}

Coordinates* Display_Touch_Get_Reference_Point(int point_number)
{
	if(point_number < 0  || point_number >= Display_Touch_Get_Reference_Point_Count()) {
		return NULL;
	}

	return &(_Touch_Reference_Points[point_number].Coordinates_Screen);
}

uint Display_Touch_Get_Marker_Count()
{
	return _Touch_Marker_Buffer.Fill_Count;
}

Coordinates* Display_Touch_Get_Marker_Coordinates(uint index)
{
	if(index >= TOUCH_MARKER_COUNT) {
		return NULL;
	}
	
	index += _Touch_Marker_Buffer.Read_Start;

	while(index >= TOUCH_MARKER_COUNT) {
		index -= TOUCH_MARKER_COUNT;
	}

	return &(_Touch_Marker_Buffer.Marker[index].Coordinates);
}

uint Display_Touch_Get_Last_X()
{
	return _X;
}

uint Display_Touch_Get_Last_Y()
{
	return _Y;
}

uint Display_Touch_Get_Last_Z1()
{
	return _Z1;
}

uint Display_Touch_Get_Last_Z2()
{
	return _Z2;
}

float Display_Touch_Get_Last_Pressure()
{
	return _Pressure;
}


/*******************************************************************
	Internal Functions
*******************************************************************/
void Display_Touch_Marker_Add(int16_t x, int16_t y)
{
	_Touch_Marker_Buffer.Marker[_Touch_Marker_Buffer.Write_Next].Coordinates.X = x;
	_Touch_Marker_Buffer.Marker[_Touch_Marker_Buffer.Write_Next].Coordinates.Y = y;
	_Touch_Marker_Buffer.Marker[_Touch_Marker_Buffer.Write_Next].Tick_Duration = TOUCH_MARKER_DEFAULT_TICKS;

	_Touch_Marker_Buffer.Write_Next++;
	if(_Touch_Marker_Buffer.Write_Next >= TOUCH_MARKER_COUNT) {
		_Touch_Marker_Buffer.Write_Next = 0;
	}

	if(_Touch_Marker_Buffer.Fill_Count < TOUCH_MARKER_COUNT) {
		_Touch_Marker_Buffer.Fill_Count++;
	}
}

void Display_Touch_Marker_Tick(void)
{
	uint Fill_Count = _Touch_Marker_Buffer.Fill_Count;
	uint Read_Start = _Touch_Marker_Buffer.Read_Start;

	for(uint i=0;i<Fill_Count;i++)
	{
		uint Index = Read_Start + i;
		while(Index >= TOUCH_MARKER_COUNT) {
			Index -= TOUCH_MARKER_COUNT;
		}
		
		if(_Touch_Marker_Buffer.Marker[Index].Tick_Duration > 0) {
			_Touch_Marker_Buffer.Marker[Index].Tick_Duration--;

			if(_Touch_Marker_Buffer.Marker[Index].Tick_Duration==0 && i==0) {
				_Touch_Marker_Buffer.Read_Start++;
				_Touch_Marker_Buffer.Fill_Count--;
			}

			if(_Touch_Marker_Buffer.Read_Start >= TOUCH_MARKER_COUNT) {
				_Touch_Marker_Buffer.Read_Start = 0;
			}
		}
	}
}

int16_t Display_Touch_Get_Screen_X_From_Touch_X(uint x_touch)
{
	return (int16_t)(_Touch_Conversion_Factors[0].Slope * x_touch + _Touch_Conversion_Factors[0].Intersect);
}

int16_t Display_Touch_Get_Screen_Y_From_Touch_Y(uint y_touch)
{
	return (int16_t)(_Touch_Conversion_Factors[1].Slope * y_touch + _Touch_Conversion_Factors[1].Intersect);
}

void Display_Touch_Update_Conversion_Factors(void)
{
	const uint X = 0;
	const uint Y = 1;
	
	float Y_Diff 	= (float)(_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Screen.X - _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.X);
	float X_Diff 	= (float)(_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Touch.X - _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Touch.X);
		
	float Slope 	= Y_Diff / X_Diff;
	float Intersect = _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.X - (Slope * _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Touch.X);
	
	_Touch_Conversion_Factors[X].Slope 		= Slope;
	_Touch_Conversion_Factors[X].Intersect 	= Intersect;


	Y_Diff 		= (float)(_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Screen.Y - _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.Y);
	X_Diff 		= (float)(_Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_BOTTOM_RIGHT].Coordinates_Touch.Y - _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Touch.Y);
		
	Slope 		= Y_Diff / X_Diff;
	Intersect 	= _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Screen.Y - (Slope * _Touch_Reference_Points[DISPLAY_TOUCH_REFERENCE_POINT_TOP_LEFT].Coordinates_Touch.Y);

	_Touch_Conversion_Factors[Y].Slope 		= Slope;
	_Touch_Conversion_Factors[Y].Intersect 	= Intersect;
}

bool Display_Touch_Is_Touched()
{
	return (_Y > 0) && (_Z1 > 0);
}

bool Display_Touch_Read_Sequential(uint times_read_each_value)
{
	if(times_read_each_value < 1) {
		return false;
	}
	
	_Baudrate_Backup = Display_SPI_Get_Baudrate();
	Display_SPI_Set_Baudrate(DISPLAY_TOUCH_SPI_BAUDRATE);

	for(int i=0;i<times_read_each_value;i++)
	{
		bool Success = Display_Touch_Read_X();
		if(!Success) { Display_SPI_Set_Baudrate(_Baudrate_Backup); return false; }
	}

	for(int i=0;i<times_read_each_value;i++)
	{
		bool Success = Display_Touch_Read_Y();
		if(!Success) { Display_SPI_Set_Baudrate(_Baudrate_Backup); return false; }
	}

	for(int i=0;i<times_read_each_value;i++)
	{
		bool Success = Display_Touch_Read_Z1();
		if(!Success) { Display_SPI_Set_Baudrate(_Baudrate_Backup); return false; }
	}

	for(int i=0;i<times_read_each_value;i++)
	{
		bool Success = Display_Touch_Read_Z2();
		if(!Success) { Display_SPI_Set_Baudrate(_Baudrate_Backup); return false; }
	}

	Display_SPI_Set_Baudrate(_Baudrate_Backup);

	_Pressure = (float)(((288 * _X) >> 12)) * (float)((((float)_Z2) / ((float)_Z1)) - 1);

	return true;
}

bool Display_Touch_Read_Overlapping(uint average_of_measurements)
{
	if(average_of_measurements < 1) {
		return false;
	}
	
	uint8_t Write_Data[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t Read_Data[9];

	Write_Data[0] = Display_Touch_Get_Control_Byte(DISPLAY_TOUCH_MUX_X, DISPLAY_TOUCH_RES_12BIT);
	Write_Data[2] = Display_Touch_Get_Control_Byte(DISPLAY_TOUCH_MUX_Y, DISPLAY_TOUCH_RES_12BIT);
	Write_Data[4] = Display_Touch_Get_Control_Byte(DISPLAY_TOUCH_MUX_Z1, DISPLAY_TOUCH_RES_12BIT);
	Write_Data[6] = Display_Touch_Get_Control_Byte(DISPLAY_TOUCH_MUX_Z2, DISPLAY_TOUCH_RES_12BIT);


	_Baudrate_Backup = Display_SPI_Get_Baudrate();
	Display_SPI_Set_Baudrate(DISPLAY_TOUCH_SPI_BAUDRATE);

	uint X = 0;
	uint Y = 0;
	uint Z1 = 0;
	uint Z2 = 0;
	bool Success;

	for(int i=0;i<average_of_measurements;i++)
	{
		Display_Touch_SPI_CS_Select();
		Success = Display_SPI_Read_Write(Write_Data, Read_Data, sizeof(Write_Data));
		Display_Touch_SPI_CS_Deselect();

		if(Success) {
			X += Display_Touch_Assemble_Value(Read_Data[2], Read_Data[1]);
			Y += Display_Touch_Assemble_Value(Read_Data[4], Read_Data[3]);
			Z1 += Display_Touch_Assemble_Value(Read_Data[6], Read_Data[5]);
			Z2 += Display_Touch_Assemble_Value(Read_Data[8], Read_Data[7]);
		} else {
			break;
		}
	}

	Display_SPI_Set_Baudrate(_Baudrate_Backup);

	if(Success) {
		divmod_result_t Result;
		Result = hw_divider_divmod_u32(X, average_of_measurements);		_X = to_quotient_u32(Result);
		Result = hw_divider_divmod_u32(Y, average_of_measurements);		_Y = to_quotient_u32(Result);
		Result = hw_divider_divmod_u32(Z1, average_of_measurements);	_Z1 = to_quotient_u32(Result);
		Result = hw_divider_divmod_u32(Z2, average_of_measurements);	_Z2 = to_quotient_u32(Result);

		_Pressure = (float)(((288 * _X) >> 12)) * (float)((((float)_Z2) / ((float)_Z1)) - 1);
	}

	return Success;
}

bool Display_Touch_Read_X()
{
	uint8_t Read_Data[3];
	bool Success = Display_Touch_Read_Single_Parameter(DISPLAY_TOUCH_MUX_X, DISPLAY_TOUCH_RES_12BIT, Read_Data);

	_X = Display_Touch_Assemble_Value(Read_Data[2], Read_Data[1]);

	return Success;
}

bool Display_Touch_Read_Y()
{
	uint8_t Read_Data[3];
	bool Success = Display_Touch_Read_Single_Parameter(DISPLAY_TOUCH_MUX_Y, DISPLAY_TOUCH_RES_12BIT, Read_Data);

	_Y = Display_Touch_Assemble_Value(Read_Data[2], Read_Data[1]);

	return Success;
}

bool Display_Touch_Read_Z1()
{
	uint8_t Read_Data[3];
	bool Success = Display_Touch_Read_Single_Parameter(DISPLAY_TOUCH_MUX_Z1, DISPLAY_TOUCH_RES_12BIT, Read_Data);

	_Z1 = Display_Touch_Assemble_Value(Read_Data[2], Read_Data[1]);

	return Success;
}

bool Display_Touch_Read_Z2()
{
	uint8_t Read_Data[3];
	bool Success = Display_Touch_Read_Single_Parameter(DISPLAY_TOUCH_MUX_Z2, DISPLAY_TOUCH_RES_12BIT, Read_Data);

	_Z2 = Display_Touch_Assemble_Value(Read_Data[2], Read_Data[1]);

	return Success;
}

bool Display_Touch_Read_Single_Parameter(uint mux, uint resolution, uint8_t *data)
{
	uint8_t Write_Data[3] = { 0, 0, 0 };
	Write_Data[0] = Display_Touch_Get_Control_Byte(mux, resolution);
	
	Display_Touch_SPI_CS_Select();
	bool Success = Display_SPI_Read_Write(Write_Data, data, sizeof(Write_Data));
	Display_Touch_SPI_CS_Deselect();

	return Success;
}

uint8_t Display_Touch_Get_Control_Byte(uint mux, uint resolution)
{
	return (uint8_t)(	(ON)				<< SHIFT_START_BIT		|	// Set Start Bit to '1' 
						(mux & 0x07) 		<< SHIFT_MULTIPLEXER 	|	// Set A2 - A0
						(resolution & 0x01) << SHIFT_RESOLUTION		|	// Set Resolution (0 == 12 Bit / 1 == 8 Bit)
						(ACQ_MODE_DIFF)		<< SHIFT_ACQ_MODE		|	// Differential Mode | .... Obsolete: Single Ended only
						(OFF)				<< SHIFT_PD_REFERENCE	|	// Internal Reference Voltage OFF
						(ON)				<< SHIFT_PD_ADC);			// ADC ON
}

uint Display_Touch_Assemble_Value(uint8_t byte_low, uint8_t byte_high)
{
	return (((uint)byte_high) << 5) | (byte_low >> 3);
}

static inline void Display_Touch_SPI_CS_Select() 
{
    asm volatile("nop \n nop \n nop");
    gpio_put(DISPLAY_SPI_PIN_CS_TOUCH, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void Display_Touch_SPI_CS_Deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(DISPLAY_SPI_PIN_CS_TOUCH, 1);
    asm volatile("nop \n nop \n nop");
}
