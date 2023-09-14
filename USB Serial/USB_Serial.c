/*
 * USB_Serial.c
 *
 * Created: Sat Mar 20 2021 09:24:20
 *  Author Chris
 */
#include "USB_Serial.h"
#include "pico/error.h"
#include "pico/stdlib.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables
volatile int16_t _Received_Byte;


// ============================================================================================
// Function Declarations


/*******************************************************************
	Functions
*******************************************************************/
void USB_Serial_Init(void)
{
	stdio_usb_init();

	_Received_Byte = PICO_ERROR_TIMEOUT;
}

////////////////////
// Receiving Data //
////////////////////
int16_t	USB_Serial_Get_Byte(void)
{
	return _Received_Byte;
}

bool USB_Serial_Available(void)
{
	_Received_Byte = PICO_ERROR_TIMEOUT;
	_Received_Byte = getchar_timeout_us(0);

	return (_Received_Byte != PICO_ERROR_TIMEOUT);
}

void USB_Serial_Flush_Input(void)
{
	stdio_flush();
}

///////////////////////
// Transmitting Data //
///////////////////////
bool USB_Serial_Put_Char(uint8_t c)
{
	uint8_t Transmitted_Byte = putchar(c);

	return (Transmitted_Byte == c);
}

bool USB_Serial_Write(const uint8_t *buffer, uint16_t size)
{
	bool Transmit_Success;
	
	for(uint16_t i=0;i<size;i++)
	{
		Transmit_Success = USB_Serial_Put_Char(buffer[i]);
		if(!Transmit_Success)
		{
			return false;
		}
	}

	return true;
}

bool USB_Serial_Send_Int_Dec(int value, uint numbers)
{
	if(value < 0) {
		USB_Serial_Put_Char('-');
		value = value * -1;
	}

	uint Factor = 1;
	for(int i=0;i<numbers-1;i++) {
		Factor *= 10;
	}

	for(int i=0;i<numbers;i++) {
		char Num = (char)(value / Factor);
		bool Success = USB_Serial_Put_Char('0' + Num);

		if(!Success) {
			return false;
		}
		
		value %= Factor;
		Factor /= 10;
	}

	return true;
}

bool USB_Serial_Send_Int_Hex(uint value, uint numbers, bool Send_0x)
{
	if(Send_0x == true)
	{
		bool Success = USB_SERIAL_SEND_STRING("0x");
		if(!Success) {
			return false;
		}
	}
	
	for(int i=0;i<numbers;i++)
	{
		char Hex_Number = (char)(value >> ((numbers-i-1) * 4)) & 0x0F;
		
		if(Hex_Number<10)
			Hex_Number = '0' + Hex_Number;
		else
			Hex_Number = 'A' + (Hex_Number - 10);
		
		bool Success = USB_Serial_Put_Char(Hex_Number);
		if(!Success) {
			return false;
		}
	}

	return true;
}

bool USB_Serial_Send_Float(float value, uint numbers, uint decimals, bool force_sign)
{
	if(value >=0 && force_sign == true) {
		USB_Serial_Put_Char('+');
	}
	else if(value < 0) {
		USB_Serial_Put_Char('-');
		value = value * -1;
	}

	int Int_Part = (int)value;
	bool Success = USB_Serial_Send_Int_Dec(Int_Part, numbers);
	if(!Success) {
		return false;
	}

	if(decimals == 0) {
		return true;
	}

	USB_Serial_Put_Char('.');

	uint Factor = 1;
	for(int i=0;i<decimals;i++) {
		Factor *= 10;
	}
	int Dec_Part = (int)((value - Int_Part) * Factor);

	return USB_Serial_Send_Int_Dec(Dec_Part, decimals);
}