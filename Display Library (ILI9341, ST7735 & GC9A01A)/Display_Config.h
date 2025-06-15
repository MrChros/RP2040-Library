/*
 * Display_Config.h
 *
 * Created: Sun Mar 21 2021 16:11:51
 *  Author Chris
 */
#ifndef DISPLAY_CONFIG_H_
#define DISPLAY_CONFIG_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/types.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/structs/spi.h"


// ============================================================================================
// Defines
#define YES							1
#define NO							0


/*******************************************************************
	Pin and Interface Definitions
*******************************************************************/
#define DISPLAY_SPI_PORT			spi0
#define DISPLAY_SPI_PIN_CS			22
#define DISPLAY_SPI_PIN_SCK			18
#define DISPLAY_SPI_PIN_MOSI		19
#define DISPLAY_PIN_RESET			20
#define DISPLAY_PIN_COMMAND			21

#define DISPLAY_SPI_PIN_MISO		4			// Only required if Touch Screen is used
#define DISPLAY_SPI_PIN_CS_TOUCH	6			// Only required if Touch Screen is used

#define DISPLAY_SPI_BAUDRATE		70000000	// 66.5 MHz

#define DISPLAY_MODE_SET_CMD()      gpio_put(DISPLAY_PIN_COMMAND, 0)	// Macro for setting TFT mode signal low (command)
#define DISPLAY_MODE_SET_DATA()     gpio_put(DISPLAY_PIN_COMMAND, 1)	// Macro for setting TFT mode signal (data)
#define DISPLAY_RESET_ON()			gpio_put(DISPLAY_PIN_RESET, 0)
#define DISPLAY_RESET_OFF()			gpio_put(DISPLAY_PIN_RESET, 1)


/*******************************************************************
	Display Definitions
*******************************************************************/
// #define DISPLAY_ILI9341
// #define DISPLAY_ST7735
#define DISPLAY_GC9A01A

#ifdef DISPLAY_ILI9341
	#define DISPLAY_WIDTH			320		// Width of display using default orientation
	#define DISPLAY_HEIGHT			240		// Height of display using default orientation
#elif defined(DISPLAY_ST7735)
	#define DISPLAY_WIDTH			160		// Width of display using default orientation
	#define DISPLAY_HEIGHT			128		// Height of display using default orientation
#elif defined(DISPLAY_GC9A01A)
	#define DISPLAY_WIDTH			240		// Width of display using default orientation
	#define DISPLAY_HEIGHT			240		// Height of display using default orientation
#endif


////////////////////////////
// DISPLAY MAIN DATATYPES //
////////////////////////////
typedef uint16_t					Display_Color;

#define DISPLAY_IMAGE_BUFFER_PIXEL_SIZE		(DISPLAY_WIDTH*DISPLAY_HEIGHT)
#define DISPLAY_IMAGE_BUFFER_BYTE_SIZE		(DISPLAY_IMAGE_BUFFER_PIXEL_SIZE*sizeof(Display_Color))

typedef union
{
	volatile Display_Color Dim_2[DISPLAY_WIDTH][DISPLAY_HEIGHT];
	volatile Display_Color Dim_1[DISPLAY_IMAGE_BUFFER_PIXEL_SIZE];
} Display_Image_Buffer;


///////////////////////////
// DISPLAY CONFIGURATION //
///////////////////////////
#define DISPLAY_FLIP_X					NO
#define DISPLAY_FLIP_Y					NO
#define DISPLAY_INVERT_COLORS			NO
#define DISPLAY_DEFAULT_CHAR_SPACING 	2


///////////////////////////////
// DISPLAY COLOR DEFINITIONS //
///////////////////////////////
#if DISPLAY_INVERT_COLORS==NO
	#define DISPLAY_COLOR(r, g, b)			((Display_Color)b & 0x00F8) | (((Display_Color)r << 5) & 0x1F00) | ((Display_Color)g >> 5) | (((Display_Color)g << 11) & 0xE000)
#else
	#define DISPLAY_COLOR(r, g, b)			((Display_Color)r & 0x00F8) | (((Display_Color)b << 5) & 0x1F00) | ((Display_Color)g >> 5) | (((Display_Color)g << 11) & 0xE000)
#endif

#define DISPLAY_COLOR_BLACK				DISPLAY_COLOR(  0,   0,   0)
#define DISPLAY_COLOR_NAVY				DISPLAY_COLOR(  0,   0, 128)
#define DISPLAY_COLOR_DARKGREEN			DISPLAY_COLOR(  0, 128,   0)
#define DISPLAY_COLOR_DARKCYAN			DISPLAY_COLOR(  0, 128, 128)
#define DISPLAY_COLOR_MAROON			DISPLAY_COLOR(128,   0,   0)
#define DISPLAY_COLOR_PURPLE			DISPLAY_COLOR(128,   0, 128)
#define DISPLAY_COLOR_OLIVE				DISPLAY_COLOR(128, 128,   0)
#define DISPLAY_COLOR_LIGHTGREY			DISPLAY_COLOR(192, 192, 192)
#define DISPLAY_COLOR_DARKGREY			DISPLAY_COLOR(128, 128, 128)
#define DISPLAY_COLOR_BLUE				DISPLAY_COLOR(  0,   0, 255)
#define DISPLAY_COLOR_GREEN				DISPLAY_COLOR(  0, 255,   0)
#define DISPLAY_COLOR_CYAN				DISPLAY_COLOR(  0, 255, 255)
#define DISPLAY_COLOR_RED				DISPLAY_COLOR(255,   0,   0)
#define DISPLAY_COLOR_MAGENTA			DISPLAY_COLOR(255,   0, 255)
#define DISPLAY_COLOR_YELLOW			DISPLAY_COLOR(255, 255,   0)
#define DISPLAY_COLOR_WHITE				DISPLAY_COLOR(255, 255, 255)
#define DISPLAY_COLOR_ORANGE			DISPLAY_COLOR(255, 165,   0)
#define DISPLAY_COLOR_GREENYELLOW		DISPLAY_COLOR(173, 255,  47)
#define DISPLAY_COLOR_PINK				DISPLAY_COLOR(255,   0, 255)



#ifdef DISPLAY_ILI9341
	// Level 1 Commands (from the display Datasheet)
	#define ILI9341_CMD_NOP                             0x00
	#define ILI9341_CMD_SOFTWARE_RESET                  0x01
	#define ILI9341_CMD_READ_DISP_ID                    0x04
	#define ILI9341_CMD_READ_DISP_STATUS                0x09
	#define ILI9341_CMD_READ_DISP_MADCTRL               0x0B
	#define ILI9341_CMD_READ_DISP_PIXEL_FORMAT          0x0C
	#define ILI9341_CMD_READ_DISP_IMAGE_FORMAT          0x0D
	#define ILI9341_CMD_READ_DISP_SIGNAL_MODE           0x0E
	#define ILI9341_CMD_READ_DISP_SELF_DIAGNOSTIC       0x0F
	#define ILI9341_CMD_ENTER_SLEEP_MODE                0x10
	#define ILI9341_CMD_SLEEP_OUT                       0x11
	#define ILI9341_CMD_PARTIAL_MODE_ON                 0x12
	#define ILI9341_CMD_NORMAL_DISP_MODE_ON             0x13
	#define ILI9341_CMD_DISP_INVERSION_OFF              0x20
	#define ILI9341_CMD_DISP_INVERSION_ON               0x21
	#define ILI9341_CMD_GAMMA_SET                       0x26
	#define ILI9341_CMD_DISPLAY_OFF                     0x28
	#define ILI9341_CMD_DISPLAY_ON                      0x29
	#define ILI9341_CMD_COLUMN_ADDRESS_SET              0x2A
	#define ILI9341_CMD_PAGE_ADDRESS_SET                0x2B
	#define ILI9341_CMD_MEMORY_WRITE                    0x2C
	#define ILI9341_CMD_COLOR_SET                       0x2D
	#define ILI9341_CMD_MEMORY_READ                     0x2E
	#define ILI9341_CMD_PARTIAL_AREA                    0x30
	#define ILI9341_CMD_VERT_SCROLL_DEFINITION          0x33
	#define ILI9341_CMD_TEARING_EFFECT_LINE_OFF         0x34
	#define ILI9341_CMD_TEARING_EFFECT_LINE_ON          0x35
	#define ILI9341_CMD_MEMORY_ACCESS_CONTROL           0x36
	#define ILI9341_CMD_VERT_SCROLL_START_ADDRESS       0x37
	#define ILI9341_CMD_IDLE_MODE_OFF                   0x38
	#define ILI9341_CMD_IDLE_MODE_ON                    0x39
	#define ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET         0x3A
	#define ILI9341_CMD_WRITE_MEMORY_CONTINUE           0x3C
	#define ILI9341_CMD_READ_MEMORY_CONTINUE            0x3E
	#define ILI9341_CMD_SET_TEAR_SCANLINE               0x44
	#define ILI9341_CMD_GET_SCANLINE                    0x45
	#define ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS        0x51
	#define ILI9341_CMD_READ_DISPLAY_BRIGHTNESS         0x52
	#define ILI9341_CMD_WRITE_CTRL_DISPLAY              0x53
	#define ILI9341_CMD_READ_CTRL_DISPLAY               0x54
	#define ILI9341_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS  0x55
	#define ILI9341_CMD_READ_CONTENT_ADAPT_BRIGHTNESS   0x56
	#define ILI9341_CMD_WRITE_MIN_CAB_LEVEL             0x5E
	#define ILI9341_CMD_READ_MIN_CAB_LEVEL              0x5F
	#define ILI9341_CMD_READ_ID1                        0xDA
	#define ILI9341_CMD_READ_ID2                        0xDB
	#define ILI9341_CMD_READ_ID3                        0xDC

	// Level 2 Commands (from the display Datasheet)
	#define ILI9341_CMD_RGB_SIGNAL_CONTROL              0xB0
	#define ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL       0xB1
	#define ILI9341_CMD_FRAME_RATE_CONTROL_IDLE_8COLOR  0xB2
	#define ILI9341_CMD_FRAME_RATE_CONTROL_PARTIAL      0xB3
	#define ILI9341_CMD_DISPLAY_INVERSION_CONTROL       0xB4
	#define ILI9341_CMD_BLANKING_PORCH_CONTROL          0xB5
	#define ILI9341_CMD_DISPLAY_FUNCTION_CONTROL        0xB6
	#define ILI9341_CMD_ENTRY_MODE_SET                  0xB7
	#define ILI9341_CMD_BACKLIGHT_CONTROL_1             0xB8
	#define ILI9341_CMD_BACKLIGHT_CONTROL_2             0xB9
	#define ILI9341_CMD_BACKLIGHT_CONTROL_3             0xBA
	#define ILI9341_CMD_BACKLIGHT_CONTROL_4             0xBB
	#define ILI9341_CMD_BACKLIGHT_CONTROL_5             0xBC
	#define ILI9341_CMD_BACKLIGHT_CONTROL_6             0xBD
	#define ILI9341_CMD_BACKLIGHT_CONTROL_7             0xBE
	#define ILI9341_CMD_BACKLIGHT_CONTROL_8             0xBF
	#define ILI9341_CMD_POWER_CONTROL_1                 0xC0
	#define ILI9341_CMD_POWER_CONTROL_2                 0xC1
	#define ILI9341_CMD_VCOM_CONTROL_1                  0xC5
	#define ILI9341_CMD_VCOM_CONTROL_2                  0xC7
	#define ILI9341_CMD_POWER_CONTROL_A                 0xCB
	#define ILI9341_CMD_POWER_CONTROL_B                 0xCF
	#define ILI9341_CMD_NVMEM_WRITE                     0xD0
	#define ILI9341_CMD_NVMEM_PROTECTION_KEY            0xD1
	#define ILI9341_CMD_NVMEM_STATUS_READ               0xD2
	#define ILI9341_CMD_READ_ID4                        0xD3
	#define ILI9341_CMD_POSITIVE_GAMMA_CORRECTION       0xE0
	#define ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION       0xE1
	#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_1         0xE2
	#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_2         0xE3
	#define ILI9341_CMD_DRIVER_TIMING_CONTROL_A         0xE8
	#define ILI9341_CMD_DRIVER_TIMING_CONTROL_B         0xEA
	#define ILI9341_CMD_ENABLE_3_GAMMA_CONTROL          0xF2
	#define ILI9341_CMD_INTERFACE_CONTROL               0xF6
	#define ILI9341_CMD_PUMP_RATIO_CONTROL              0xF7
#elif defined(DISPLAY_ST7735)
	#define ST77XX_SLPOUT 								0x11
	#define ST77XX_NORON 								0x13
	#define ST77XX_INVOFF 								0x20
	#define ST77XX_DISPON 								0x29
	#define ST77XX_CASET 								0x2A
	#define ST77XX_RASET 								0x2B
	#define ST77XX_MEM_WR								0x2C
	#define ST77XX_MADCTL 								0x36
	#define ST77XX_COLMOD 								0x3A

	#define ST7735_MADCTL_BGR 							0x08
	#define ST7735_MADCTL_MH 							0x04

	#define ST7735_FRMCTR1 								0xB1
	#define ST7735_FRMCTR2 								0xB2
	#define ST7735_FRMCTR3 								0xB3
	#define ST7735_INVCTR 								0xB4
	#define ST7735_DISSET5 								0xB6

	#define ST7735_PWCTR1 								0xC0
	#define ST7735_PWCTR2 								0xC1
	#define ST7735_PWCTR3 								0xC2
	#define ST7735_PWCTR4 								0xC3
	#define ST7735_PWCTR5 								0xC4
	#define ST7735_VMCTR1 								0xC5

	#define ST7735_PWCTR6 								0xFC

	#define ST7735_GMCTRP1 								0xE0
	#define ST7735_GMCTRN1 								0xE1
#elif defined(DISPLAY_GC9A01A)
	#define GC9A01A_SWRESET 							0x01	///< Software Reset (maybe, not documented)
	#define GC9A01A_RDDID 								0x04	///< Read display identification information
	#define GC9A01A_RDDST 								0x09	///< Read Display Status
	#define GC9A01A_SLPIN 								0x10	///< Enter Sleep Mode
	#define GC9A01A_SLPOUT 								0x11	///< Sleep Out
	#define GC9A01A_PTLON 								0x12	///< Partial Mode ON
	#define GC9A01A_NORON 								0x13	///< Normal Display Mode ON
	#define GC9A01A_INVOFF 								0x20	///< Display Inversion OFF
	#define GC9A01A_INVON 								0x21	///< Display Inversion ON
	#define GC9A01A_DISPOFF 							0x28	///< Display OFF
	#define GC9A01A_DISPON 								0x29	///< Display ON
	#define GC9A01A_CASET 								0x2A	///< Column Address Set
	#define GC9A01A_RASET 								0x2B	///< Row Address Set
	#define GC9A01A_RAMWR 								0x2C	///< Memory Write
	#define GC9A01A_PTLAR 								0x30	///< Partial Area
	#define GC9A01A_VSCRDEF 							0x33	///< Vertical Scrolling Definition
	#define GC9A01A_TEOFF								0x34	///< Tearing Effect Line OFF
	#define GC9A01A_TEON 								0x35	///< Tearing Effect Line ON
	#define GC9A01A_MADCTL 								0x36	///< Memory Access Control
	#define GC9A01A_VSCRSADD 							0x37	///< Vertical Scrolling Start Address
	#define GC9A01A_IDLEOFF 							0x38	///< Idle mode OFF
	#define GC9A01A_IDLEON 								0x39    ///< Idle mode ON
	#define GC9A01A_COLMOD 								0x3A	///< Pixel Format Set
	#define GC9A01A_CONTINUE 							0x3C	///< Write Memory Continue
	#define GC9A01A_TEARSET 							0x44	///< Set Tear Scanline
	#define GC9A01A_GETLINE 							0x45	///< Get Scanline
	#define GC9A01A_SETBRIGHT 							0x51	///< Write Display Brightness
	#define GC9A01A_SETCTRL 							0x53	///< Write CTRL Display
	#define GC9A01A1_POWER7 							0xA7	///< Power Control 7
	#define GC9A01A_TEWC 								0xBA	///< Tearing effect width control
	#define GC9A01A1_POWER1 							0xC1	///< Power Control 1
	#define GC9A01A1_POWER2 							0xC3	///< Power Control 2
	#define GC9A01A1_POWER3 							0xC4	///< Power Control 3
	#define GC9A01A1_POWER4 							0xC9	///< Power Control 4
	#define GC9A01A_RDID1 								0xDA	///< Read ID 1
	#define GC9A01A_RDID2 								0xDB	///< Read ID 2
	#define GC9A01A_RDID3 								0xDC	///< Read ID 3
	#define GC9A01A_FRAMERATE 							0xE8	///< Frame rate control
	#define GC9A01A_SPI2DATA 							0xE9	///< SPI 2DATA control
	#define GC9A01A_INREGEN2 							0xEF	///< Inter register enable 2
	#define GC9A01A_GAMMA1 								0xF0    ///< Set gamma 1
	#define GC9A01A_GAMMA2 								0xF1    ///< Set gamma 2
	#define GC9A01A_GAMMA3 								0xF2    ///< Set gamma 3
	#define GC9A01A_GAMMA4 								0xF3    ///< Set gamma 4
	#define GC9A01A_IFACE 								0xF6	///< Interface control
	#define GC9A01A_INREGEN1 							0xFE	///< Inter register enable 1
#endif

#ifdef DISPLAY_ILI9341
	#define DISPLAY_MEMORY_WRITE				ILI9341_CMD_MEMORY_WRITE		
#elif defined(DISPLAY_ST7735)
	#define DISPLAY_MEMORY_WRITE				ST77XX_MEM_WR
#elif defined(DISPLAY_GC9A01A)
	#define DISPLAY_MEMORY_WRITE				GC9A01A_RAMWR
#endif

#endif /* DISPLAY_CONFIG_H_ */