/*
 * Display_Message_Box_Icons.h
 *
 * Created: Sat May 20 2023 11:54:10 AM
 *  Author: MrChros
 */
#ifndef DISPLAY_MESSAGE_BOX_ICONS_H_
#define DISPLAY_MESSAGE_BOX_ICONS_H_

// ============================================================================================
// Includes
#include "Display_Objects_Datatypes.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables
extern const unsigned char _Message_Box_Icon_Add_Circle_Outline[];
extern const unsigned char _Message_Box_Icon_Alert_Circle_Outline[];
extern const unsigned char _Message_Box_Icon_Ban_Outline[];
extern const unsigned char _Message_Box_Icon_Chatbubble_Outline[];
extern const unsigned char _Message_Box_Icon_Checkmark_Circle_Outline[];
extern const unsigned char _Message_Box_Icon_Close_Circle_Outline[];
extern const unsigned char _Message_Box_Icon_Happy_Outline[];
extern const unsigned char _Message_Box_Icon_Information_Circle_Outline[];
extern const unsigned char _Message_Box_Icon_Sad_Outline[];
extern const unsigned char _Message_Box_Icon_Skull_Outline[];


// ============================================================================================
// Function Declarations
uint Display_Message_Box_Icons_Get_Icon_Width(Object_Message_Box_Icon icon);
uint Display_Message_Box_Icons_Get_Icon_Height(Object_Message_Box_Icon icon);

const unsigned char* Display_Message_Box_Icons_Get_Icon_Ptr(Object_Message_Box_Icon icon);


#endif /* DISPLAY_MESSAGE_BOX_ICONS_H_ */
