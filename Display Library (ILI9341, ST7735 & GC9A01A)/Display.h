/*
 * Display.h
 *
 * Created: Sun Mar 21 2021 15:51:51
 *  Author Chris
 */
#ifndef DISPLAY_H_
#define DISPLAY_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Display_Config.h"
#include "Display_Objects.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_Init(Display_Color initial_color, bool send_buffer, bool init_touch);

void Display_Issue_Touch_Event(int16_t x_screen, int16_t y_screen);
void Display_Set_Draw_Touch_Reference_Points(bool do_draw);
void Display_Set_Draw_Center_Lines(bool do_draw);

void Display_Screen_Transition_Start(Screen_Transition_Direction direction);

void Display_Render_Objects(void);
void Display_Send_Buffer(void);
bool Display_Send_Buffer_Completed(void);

void Display_Show_Test_Screen(void);

int	 Display_Get_Button_Touch_Return_Value(void);
void Display_Select_First_Object(void);
void Display_Action_CW(void);
void Display_Action_CCW(void);
void Display_Action_SW(void);
void Display_Select_Object(void);
void Display_Unselect_Object(void);

void Display_Inc_Frame_Counter(void);
uint* Display_Get_Frame_Counter_Reference(void);

void Display_Set_Debug_Print(void);


#endif /* DISPLAY_H_ */