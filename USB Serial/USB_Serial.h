/*
 * USB_Serial.h
 *
 * Created: Sat Mar 20 2021 09:23:49
 *  Author Chris
 */
#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// ============================================================================================
// Defines
#define USB_SERIAL_TERMINATOR				0x0D

#define USB_SERIAL_SEND_STRING(_TEXT_)		USB_Serial_Write((uint8_t*)_TEXT_, sizeof(_TEXT_)-1)
#define USB_SERIAL_SEND_TERMINATOR()		USB_Serial_Put_Char(USB_SERIAL_TERMINATOR);


// ============================================================================================
// Function Declarations
void	USB_Serial_Init(void);


// Receiving Data
int16_t	USB_Serial_Get_Byte(void);		// Receive a Byte (-1 if timeout/error)
bool	USB_Serial_Available(void);		// Number of bytes in receive buffer
void	USB_Serial_Flush_Input(void);	// Discard any buffered input

// Transmitting Data
bool	USB_Serial_Put_Char(uint8_t c);							// Transmit a character
bool	USB_Serial_Write(const uint8_t *buffer, uint16_t size); // Transmit a buffer
bool	USB_Serial_Send_Int_Dec(int value, uint numbers);
bool	USB_Serial_Send_Int_Hex(uint value, uint numbers, bool Send_0x);
bool 	USB_Serial_Send_Float(float value, uint numbers, uint decimals, bool force_sign);

#endif /* USB_SERIAL_H_ */