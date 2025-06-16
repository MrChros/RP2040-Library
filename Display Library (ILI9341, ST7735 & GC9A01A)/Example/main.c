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

#include "Display.h"
#include "Display_SPI.h"
#include "Screens_Display.h"


// ============================================================================================
// Variables
volatile bool _Timer_Fired = false;


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
	// Display 		=================================================
	// Initialize the Display library
	Display_Init(DISPLAY_COLOR_BLACK, true, false);
	// Call Setup-function of initisl screen
	// Very important to initialize all screen function pointerss
	Screen_Display_Setup_Menu_Main();	

	// Repeating Timer	=============================================
	// Initialize a repeating timer to refresh the display and do other tasks if wanted
	struct repeating_timer Timer;
	add_repeating_timer_ms(40, ISR_Repeating_Timer, NULL, &Timer);	// 40ms -> 25 FPS

    while (1) {
		if (_Timer_Fired == true)
		{
			_Timer_Fired = false;

			Display_Send_Buffer();
			Display_Inc_Frame_Counter();

			_Screen_Display_Tick();
		}

		// Add functions here to execute during the DMA Transfer
		if(Display_SPI_DMA_Transfer_Ongoing())
		{
					
		}

		if(Display_Send_Buffer_Completed())
		{	
			Display_Render_Objects();
		}
		else
		{
        	// Do something else...
		}	
    }
}