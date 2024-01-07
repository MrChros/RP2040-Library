/*
 * main.c
 *
 * Created: Mon Nov 08 2021 14:06:53
 *  Author Chris
 */

// ============================================================================================
// Includes
#include <stdio.h>

#include "pico/stdlib.h"

#include "I2C_Master.h"
#include "OLED_SSD1306.h"

#include "Screens.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables


// ============================================================================================
// Function Declarations


/*******************************************************************
	Interrupt Service Routines
*******************************************************************/
bool ISR_Repeating_Timer(struct repeating_timer *t)
{
	_Timer_Fired = true;
    return true;
}


/*******************************************************************
	Main Routine
*******************************************************************/
int main(void)
{
	// I2C Master	=================================================
	I2CM_Init(I2C_NO_INTERNAL_PULLUP);
	
	// OLED SSD1306 =================================================
	SSD1306_Init();
	Screen_Setup_0_Start(ANIMATION_DIRECTION_UP);

	// Rotary Encoder	=============================================
	Rotary_Encoder_Init();

	// Repeating Timer	=============================================
	struct repeating_timer Timer;
	add_repeating_timer_ms(40, ISR_Repeating_Timer, NULL, &Timer);


    while (1)
	{
        if (_Timer_Fired == true)
		{
			_Timer_Fired = false;

			// Transmit Screen Buffer to OLED SSD1306 via I2C using
			// DMA Transfer to minimize CPU load
			SSD1306_Transmit_Buffer();
		}

		if(SSD1306_Transmit_Completed())
		{
			// If the Buffer has been fully transmitted, we can edit the buffer
			
			// First, perfom an animation step in case there is a screen transition ongoing
			SSD1306_Animation_Tick();
			
			// Call the tick function of the current display to update the content of the current screen
			_Screen_Tick();
		}
		else
		{
			// Check any of the rotary encoder events occurred and if so, call the according function of the current screen
			if(Rotary_Encoder_Rotation_CW_Occurred()) { 
				_Screen_Action_CW();
			}

			if(Rotary_Encoder_Rotation_CCW_Occurred()) {
				_Screen_Action_CCW();
			}

			if(Rotary_Encoder_Switch_Press_Occurred()) {
				_Screen_Action_Click();
			}
			
			// Call any of your own functions here...
		}
    }
}
