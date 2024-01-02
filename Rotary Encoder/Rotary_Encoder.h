/*
 * Rotary_Encoder.h
 *
 * Created: Mon Nov 29 2021 11:25:57
 *  Author Chris
 */
#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

// ============================================================================================
// Includes
#include <stdbool.h>


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Rotary_Encoder_Init(void);

bool Rotary_Encoder_Rotation_CW_Occurred(void);
bool Rotary_Encoder_Rotation_CCW_Occurred(void);
bool Rotary_Encoder_Switch_Press_Occurred(void);


#endif /* ROTARY_ENCODER_H_ */
