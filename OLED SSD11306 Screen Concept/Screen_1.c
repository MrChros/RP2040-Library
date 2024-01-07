/*
 * Screen_Menu_Main.c
 *
 * Created: Fri Apr 02 2021 14:34:01
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "Screens.h"

#include "OLED_SSD1306.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables



// ============================================================================================
// Function Declarations
void Screen_Setup_1(uint transition_animation);

static void Screen_Tick			(void);
static void Screen_Action_CW	(void);
static void Screen_Action_CCW	(void);
static void Screen_Action_Click	(void);


/*******************************************************************
	Functions
*******************************************************************/
void Screen_Setup_1(uint transition_animation)
{
	_Screen_Tick			= Screen_Tick;
	_Screen_Action_CW		= Screen_Action_CW;
	_Screen_Action_CCW		= Screen_Action_CCW;
	_Screen_Action_Click	= Screen_Action_Click;

	SSD1306_Animation_Start(transition_animation);

	///////////////////////////////
	// Initialize Variables	here //
	///////////////////////////////
}

void Screen_Tick(void)
{
	SSD1306_Clear(true);

	///////////////////////////////
	// Draw Screem Elements	here //
	///////////////////////////////
}

void Screen_Action_CW(void)
{
	
}

void Screen_Action_CCW(void)
{
	Screen_Setup_0(ANIMATION_DIRECTION_RIGHT);
}

void Screen_Action_Click(void)
{
	
}

/*******************************************************************
	Internal Functions
*******************************************************************/
