/*
 * Display_Init.c
 *
 * Created: Sun Mar 21 2021 12:52:01
 *  Author Chris
 */
#include "Display_SPI.h"
#include "Display_Init.h"
#include "Display_Config.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables


// ============================================================================================
// Function Declarations


/*******************************************************************
	Functions
*******************************************************************/
void Display_Init_GPIOs(void)
{
	// Initialize Reset- and DC-Pins as regular GPIOs
	gpio_init			(DISPLAY_PIN_RESET);
    gpio_set_dir		(DISPLAY_PIN_RESET, GPIO_OUT);
	gpio_set_slew_rate	(DISPLAY_PIN_RESET, GPIO_SLEW_RATE_FAST);
	DISPLAY_RESET_OFF();

	gpio_init			(DISPLAY_PIN_COMMAND);
    gpio_set_dir		(DISPLAY_PIN_COMMAND, GPIO_OUT);
	gpio_set_slew_rate	(DISPLAY_PIN_COMMAND, GPIO_SLEW_RATE_FAST);
	DISPLAY_MODE_SET_DATA();
}

void Display_Init_Reset(void)
{
	sleep_ms(10);
	DISPLAY_RESET_ON();
	sleep_ms(10);
	DISPLAY_RESET_OFF();
	sleep_ms(10);

	Display_SPI_Start_Command(0x01);	// Soft Reset (Valid for all Display types)
	Display_SPI_Finish_Command();
	sleep_ms(150);
}

void Display_Init_WakeUp(void)
{
	#ifdef DISPLAY_ILI9341
		Display_SPI_Start_Command(ILI9341_CMD_SLEEP_OUT);
		Display_SPI_Finish_Command();

		sleep_ms(150);

		uint8_t Data[2] = { 0, 0 };
		
		Display_SPI_Start_Command(ILI9341_CMD_MEMORY_WRITE);
		for(int16_t r=0 ; r<DISPLAY_HEIGHT ; r++)
		{
			for(int16_t c=0 ; c<DISPLAY_WIDTH ; c++)
			{
				spi_write_blocking(DISPLAY_SPI_PORT, Data, 2);
			}
		}	
		Display_SPI_Finish_Command();
		
		
		Display_SPI_Start_Command(ILI9341_CMD_DISPLAY_ON);
		Display_SPI_Finish_Command();
	#elif defined(DISPLAY_ST7735)
		Display_SPI_Start_Command(ST77XX_NORON);
		Display_SPI_Finish_Command();
		sleep_ms(10);

		Display_SPI_Start_Command(ST77XX_DISPON);
		Display_SPI_Finish_Command();
		sleep_ms(500);
	#elif defined(DISPLAY_GC9A01A)
		Display_SPI_Start_Command(GC9A01A_NORON);
		Display_SPI_Finish_Command();
		sleep_ms(10);

		Display_SPI_Start_Command(GC9A01A_DISPON);
		Display_SPI_Finish_Command();
		sleep_ms(500);
	#endif
}

void Display_Init_Registers(void)
{
	#ifdef DISPLAY_ILI9341
		Display_SPI_Start_Command(ILI9341_CMD_GAMMA_SET);
		Display_SPI_Start_Parameter(0x01);
		Display_SPI_Finish_Command();

		// Positive Gamma Correction
		Display_SPI_Start_Command(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION);
		Display_SPI_Start_Parameter_Array((uint8_t[15]){ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 }, 15);
		Display_SPI_Finish_Command();

		// Negative Gamma Correction
		Display_SPI_Start_Command(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION);
		Display_SPI_Start_Parameter_Array((uint8_t[15]){ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f }, 15);
		Display_SPI_Finish_Command();

		// Memory Access Control
		Display_SPI_Start_Command(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
		Display_SPI_Start_Parameter(0x00 | (DISPLAY_FLIP_X << 6) | (DISPLAY_FLIP_Y << 7));	// RGB
		Display_SPI_Finish_Command();

		// Pixel Format
		Display_SPI_Start_Command(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
		Display_SPI_Start_Parameter(0x55);  // 16-bit
		Display_SPI_Finish_Command();

		// Display Inversion OFF
		Display_SPI_Start_Command(ILI9341_CMD_DISP_INVERSION_OFF | DISPLAY_INVERT_COLORS);	
		Display_SPI_Finish_Command();

		// Frame Rate; Default, 70 Hz
		Display_SPI_Start_Command(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL);
		Display_SPI_Start_Parameter(0x00);
		Display_SPI_Start_Parameter(0x1B);
		Display_SPI_Finish_Command();
	#elif defined(DISPLAY_ST7735)
		Display_SPI_Start_Command(ST77XX_SLPOUT);
		Display_SPI_Finish_Command();
		sleep_ms(500);

		Display_SPI_Start_Command(ST77XX_COLMOD);
		Display_SPI_Start_Parameter(0x05);  // 16-bit color RGB 565
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(ST77XX_MADCTL);
		Display_SPI_Start_Parameter(0x80);  // Row/col addr, top-bttom refresh
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(ST77XX_CASET);
		Display_SPI_Start_Parameter_Array((uint8_t[4]){ 0x00, 0x00, 0x00, DISPLAY_HEIGHT - 1 }, 4);	
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(ST77XX_RASET);
		Display_SPI_Start_Parameter_Array((uint8_t[4]){ 0x00, 0x00, 0x00, DISPLAY_WIDTH - 1 }, 4);	
		Display_SPI_Finish_Command();
	#elif defined(DISPLAY_GC9A01A)
		Display_SPI_Start_Command(GC9A01A_INREGEN2);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0xEB);	Display_SPI_Start_Parameter(0x14);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_INREGEN1);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A_INREGEN2);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0xEB);	Display_SPI_Start_Parameter(0x14);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x84);	Display_SPI_Start_Parameter(0x40);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x85);	Display_SPI_Start_Parameter(0xFF);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x86);	Display_SPI_Start_Parameter(0xFF);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x87);	Display_SPI_Start_Parameter(0xFF);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x88);	Display_SPI_Start_Parameter(0x0A);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x89);	Display_SPI_Start_Parameter(0x21);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8A);	Display_SPI_Start_Parameter(0x00);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8B);	Display_SPI_Start_Parameter(0x80);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8C);	Display_SPI_Start_Parameter(0x01);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8D);	Display_SPI_Start_Parameter(0x01);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8E);	Display_SPI_Start_Parameter(0xFF);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x8F);	Display_SPI_Start_Parameter(0xFF);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x86);	Display_SPI_Start_Parameter_Array((uint8_t[2]){ 0x00, 0x00 }, 2);		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_MADCTL);
		Display_SPI_Start_Parameter(0x00);  // Row/col addr, top-bttom refresh
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_COLMOD);
		Display_SPI_Start_Parameter(0x05);  // 16-bit color RGB 565
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0x90);	Display_SPI_Start_Parameter_Array((uint8_t[4]){ 0x08, 0x08, 0x08, 0x08 }, 4);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xBD);	Display_SPI_Start_Parameter(0x86);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xBC);	Display_SPI_Start_Parameter(0x00);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x90);	Display_SPI_Start_Parameter_Array((uint8_t[3]){ 0x60, 0x01, 0x04 }, 3);		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A1_POWER2);	Display_SPI_Start_Parameter(0x13);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A1_POWER3);	Display_SPI_Start_Parameter(0x13);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A1_POWER4);	Display_SPI_Start_Parameter(0x22);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0xBE);	Display_SPI_Start_Parameter(0x11);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xE1);	Display_SPI_Start_Parameter_Array((uint8_t[2]){ 0x10, 0x0E }, 2);			Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xDF);	Display_SPI_Start_Parameter_Array((uint8_t[3]){ 0x21, 0x0C, 0x02 }, 3);		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_GAMMA1);	Display_SPI_Start_Parameter_Array((uint8_t[6]){ 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A }, 6);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A_GAMMA2);	Display_SPI_Start_Parameter_Array((uint8_t[6]){ 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F }, 6);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A_GAMMA3);	Display_SPI_Start_Parameter_Array((uint8_t[6]){ 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A }, 6);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A_GAMMA4);	Display_SPI_Start_Parameter_Array((uint8_t[6]){ 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F }, 6);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0xED);	Display_SPI_Start_Parameter_Array((uint8_t[2]){ 0x18, 0x08 }, 2);			Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xAE);	Display_SPI_Start_Parameter(0x77);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0xCD);	Display_SPI_Start_Parameter(0x63);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_CASET);
		Display_SPI_Start_Parameter_Array((uint8_t[4]){ 0x00, 0x00, 0x00, DISPLAY_HEIGHT - 1 }, 4);	
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_RASET);
		Display_SPI_Start_Parameter_Array((uint8_t[4]){ 0x00, 0x00, 0x00, DISPLAY_WIDTH - 1 }, 4);	
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_FRAMERATE);
		Display_SPI_Start_Parameter(0x34);
		Display_SPI_Finish_Command();

		Display_SPI_Start_Command(0x62);	Display_SPI_Start_Parameter_Array((uint8_t[12]){ 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70 }, 12);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x63);	Display_SPI_Start_Parameter_Array((uint8_t[12]){ 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70 }, 12);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x64);	Display_SPI_Start_Parameter_Array((uint8_t[7]){ 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07 }, 7);									Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x66);	Display_SPI_Start_Parameter_Array((uint8_t[10]){ 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00 }, 10);				Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x67);	Display_SPI_Start_Parameter_Array((uint8_t[10]){ 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98 }, 10);				Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x74);	Display_SPI_Start_Parameter_Array((uint8_t[7]){ 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00 }, 7);									Display_SPI_Finish_Command();
		Display_SPI_Start_Command(0x98);	Display_SPI_Start_Parameter_Array((uint8_t[2]){ 0x3e, 0x07 }, 2);																Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_TEON);	Display_SPI_Finish_Command();
		Display_SPI_Start_Command(GC9A01A_INVON);	Display_SPI_Finish_Command();

		Display_SPI_Start_Command(GC9A01A_SLPOUT);
		Display_SPI_Finish_Command();
		sleep_ms(500);


	#endif
}


/*******************************************************************
	Internal Functions
*******************************************************************/
