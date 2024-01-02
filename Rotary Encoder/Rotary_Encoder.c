/*
 * Rotary_Encoder.c
 *
 * Created: Mon Nov 29 2021 11:27:29
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "Rotary_Encoder.h"

#include "pico/time.h"
#include "pico/types.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"


// ============================================================================================
// Defines
#define ENCODER_PIN_A_GPIO			18
#define ENCODER_PIN_B_GPIO			19
#define ENCODER_SWITCH_GPIO			27

#define GPIO_IRQ_LEVEL_LOW 			1
#define GPIO_IRQ_LEVEL_HIGH			2
#define GPIO_IRQ_EDGE_FALL			4
#define GPIO_IRQ_EDGE_RISE			8

#define SWITCH_IDLE					1
#define SWITCH_PRESSED				0

#define DEBOUNCE_TIME_ENCODER_MS	50	
#define DEBOUNCE_TIME_SWITCH_MS		300	


// ============================================================================================
// Variables
static volatile uint32_t	_Debounce_Time_Encoder_ms 	= 0;
static volatile uint32_t	_Debounce_Time_Switch_ms 	= 0;

static volatile bool		_Rotation_CW_Occurred;
static volatile bool		_Rotation_CCW_Occurred;
static volatile bool		_Switch_Press_Occurred;

static volatile uint8_t 	_Rotary_Encoder_Previous_NextCode = 0;
static volatile uint16_t 	_Rotary_Encoder_Store_State = 0;
static volatile int8_t 		_Rotary_Encoder_Table[] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };


// ============================================================================================
// Function Declarations


/*******************************************************************
	Interrupt Service Routines
*******************************************************************/
void ISR_Rotary_Encoder(uint gpio, uint32_t events)
{
	if(gpio == ENCODER_PIN_A_GPIO || gpio == ENCODER_PIN_B_GPIO)
	{
		_Rotary_Encoder_Previous_NextCode <<= 2;
		if(gpio_get(ENCODER_PIN_A_GPIO)) { _Rotary_Encoder_Previous_NextCode |= 0x02; }
		if(gpio_get(ENCODER_PIN_B_GPIO)) {	_Rotary_Encoder_Previous_NextCode |= 0x01; }
		_Rotary_Encoder_Previous_NextCode &= 0x0f;

		// If valid then _Rotary_Encoder_Store_State as 16 bit data.
		if(_Rotary_Encoder_Table[_Rotary_Encoder_Previous_NextCode])
		{
			_Rotary_Encoder_Store_State <<= 4;
			_Rotary_Encoder_Store_State |= _Rotary_Encoder_Previous_NextCode;

			if((_Rotary_Encoder_Store_State & 0xFF)==0x2B) { _Rotation_CCW_Occurred = true; }
			if((_Rotary_Encoder_Store_State & 0xFF)==0x17) { _Rotation_CW_Occurred  = true; }
		}
	}
	else if(gpio == ENCODER_SWITCH_GPIO)
	{
		uint32_t Current_Time_ms = to_ms_since_boot(get_absolute_time());

		if(Current_Time_ms < _Debounce_Time_Switch_ms)
		{
			return;
		}

		_Switch_Press_Occurred = true;

		_Debounce_Time_Switch_ms = Current_Time_ms + DEBOUNCE_TIME_SWITCH_MS;
	}
}


/*******************************************************************
	Functions
*******************************************************************/
void Rotary_Encoder_Init(void)
{
	_Rotation_CW_Occurred 		= false;
	_Rotation_CCW_Occurred 		= false;
	_Switch_Press_Occurred		= false;

	
	gpio_init(ENCODER_PIN_A_GPIO);	gpio_set_dir(ENCODER_PIN_A_GPIO, false);
	gpio_init(ENCODER_PIN_B_GPIO);	gpio_set_dir(ENCODER_PIN_B_GPIO, false);
	gpio_init(ENCODER_SWITCH_GPIO);	gpio_set_dir(ENCODER_SWITCH_GPIO, false);
	
	gpio_set_irq_enabled_with_callback(ENCODER_PIN_A_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ISR_Rotary_Encoder);
	gpio_set_irq_enabled(ENCODER_PIN_B_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
	gpio_set_irq_enabled(ENCODER_SWITCH_GPIO, GPIO_IRQ_EDGE_FALL , true);
}

bool Rotary_Encoder_Rotation_CW_Occurred(void)
{
	bool Return_Value = _Rotation_CW_Occurred;

	_Rotation_CW_Occurred = false;

	return Return_Value;
}

bool Rotary_Encoder_Rotation_CCW_Occurred(void)
{
	bool Return_Value = _Rotation_CCW_Occurred;

	_Rotation_CCW_Occurred = false;

	return Return_Value;
}

bool Rotary_Encoder_Switch_Press_Occurred(void)
{
	bool Return_Value = _Switch_Press_Occurred;

	_Switch_Press_Occurred = false;

	return Return_Value;
}


/*******************************************************************
	Internal Functions
*******************************************************************/
