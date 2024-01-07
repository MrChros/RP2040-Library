# RP2040-Library

These is my self created libraries for the RP2040. I use the libraries for my personal projects, so do not expect professional code.
Please consider that this repository has been set up short term, so there is not a lot of explanation. In case of questions, don't hestitate
to contact me or to raise a ticket here in GitHUB.
The following libraries are included:

## I2C Master
A basic collection I2C Master functions, sending and receive. Sending can be done via DMA as well.
When using the libary, you need to configure the correct GPIO pins and the correct I2C hardware unit of the PR2040.
See the defines section in I2C_Master.c file

### Other Library Dependencies
*None*

### RP2040 SDK Requirements
* pico_stdlib
* hardware_i2c
* hardware_irq
* hardware_dma


## OLED SSD1306
This is a basic library for the OLED display SSD1306. A hight of 64 and 32 pixels is supported. This need to be defined in the defines section
of the OLED_SSD1306.h file. You can define the display data from the local buffer to the display yo be transfered blocking or by DMA in the
function 
```
void SSD1306_Transmit_Buffer();
```
within the OLED_SSD1306.c file. 

Fonts are also supported, but only fonts with fixed width. See the Fonts subfolder for some examples.
In order to use a font, set the pointer to font using the function 
```
void SSD1306_Set_Font(const uint8_t* font);
```

When the font point is not NULL, text can be written using the fuctions
```
void SSD1306_Write_Text(int16_t x, int16_t y, char* text, uint8_t length);
void SSD1306_Write_Text_Centered(int16_t y, char* text, uint8_t length);
void SSD1306_Write_Int_Dec(int16_t x, int16_t y, int32_t val, uint8_t numbers, bool display_signed, bool skip_leading_zero);
void SSD1306_Write_Int_Hex(int16_t x, int16_t y, uint val, uint8_t numbers, uint8_t write_0x);
void SSD1306_Write_Unit(int16_t x, int16_t y, uint8_t numbers, uint8_t extra_space, char* text, uint8_t length);
```


Icons or images can been shown as well. I also attached some example images. The bitmaps of the images are located in the subfolder "Images".
The binary data for the images is available in the files "Bitmaps.h" and "Bitmaps.c". In order to display an image, simply pass the pointer
to the image data to one of the functions 
```
void SSD1306_Draw_Bitmap(int16_t x, int16_t y, const uint8_t* bitmap);
void SSD1306_Draw_Bitmap_Centered(int16_t y, const uint8_t* bitmap);
```

To add new bitmaps, perform the following steps:
1. Draw your on black in white image. Black is color, that will be displayed later on. The dimensions do not matter.
2. Save the images as .bmp-file
3. Go to https://javl.github.io/image2cpp/
4. Upload your bitmap and check 'Invert Image Colors'
5. Select Code output format: plain bytes
6. Use Draw Mode: Vertical - 1 bit per pixel
7. Click "Generate Code"
8. Copy the generated output to the Bitmap.c file - use the already present bitmaps as example
9. Add the same bitmap variable declared as "extern" to the Bitmap.h file
10. You are ready to use your own bitmap

### Other Library Dependencies
* I2C Master 

### RP2040 SDK Requirements
* pico_stdlib
* hardware_divider

## OLED SSD11306 Screen Concept

The idea of the Screen Concept is to have an abstraction of the different views you can display on the OLED11306. Meaning, you want have one .c-file per view or screen. Each .c-file is structured to have a setup-function with a unique name and several static functions to be called, when events occur. With events, I refer to the rotary encoder functions for rotation CW/CCW or click. Additionally, there is a tick-function to be called periodically, to update the contents (e.g. a number or text) of the current view.

But let's go step by step:
1. Checked in are two screens (Screen_0.c and Screen_1.c). Both screens have a unique setup function (void Screen_Setup_0(...)/ void Screen_Setup_1(...)). When a setup-function is called, a number of function pointers are being updated:
   - void Screen_Action_CW(void),
   - void Screen_Action_CCW(void), 
   - void Screen_Action_Click(void) and the tick-function
   - void Screen_Tick(void).

2. All of these function pointers now point to the repective function of the current view. This allows to generally call screen function, without checking what screen is currently being displayed.

3. The function pointers are called from also checked in main.c-file. In the beginning of the main-functions, the different peripherals are being initalized. In this case, starting with I2C as master followed by the SSD1306 library. Then direclty, the setup function of the initial screen (is this case Screen_0) is called. This is mandatory in order to initialze the screen related functions pointers. The rotary encoder is initialized next.

    The last step of the initialization should be the repeating timer, to trigger screen updates. In this example, the timer is set to a period of 40 ms. This results in 25 ticks per second, or in other words to 25 FPS.

4. Every time this repeating timer has fired (see while-loop in main.c), the OLED display buffer is transferred via I2C to the display. This transfer is done using DMA, meaning barely any CPU time is required. 

5. When the DMA transfer via I2C has been completed, the tick-function of the current screen is called (remember that the call is done via a function pointer). In the tick-function, the local screen buffer are mean to be updated and be written. In this example, nothing is written on the screen. It is recommended, before wirting new content to the screen buffer to clear the screen buffer avoiding any artifacts.

   > [!IMPORTANT]
   > DMA Transfer and tick-function both need to be completed with the period time of the repeating time. 40ms is usually more than sufficient. Reducing the period time for the sake of more FPS needs to carefully handled and verified.

6. While the repeating timer has not fired and the DMA Transfer is not just completed, any other tasks can be performed. In this example only the rotary encoder events are being polled and the according screen event-function are being called in case an event happened.

   Feel free to add your own function here. If there is CPU intensive stuff to be handled, I recommend to move that to the second core of the RP2040.

7. The Screens-h-file contains the list of all existing screen setup-functions. So, if you add another screen, you need to add the unqiue name of the new screen to this file, in order to make it available to other screens.

8. The file Screen_Variables.c only contains the actual function-pointers definitions and should not be changed.

9. When calling another screen, you can define an transition effect. With the current version of the OLED library only linear motions to left/right/up/down are possible. Of, course the animation can also be skipped.

10. To my experience, you may end up with a bunch of screen files, when implementing a menu structure or whatever. You for info here.

### Other Library Dependencies
* OLED SSD1306
* Rotary Encoder

### RP2040 SDK Requirements
* pico_stdlib

## Rotary Encoder
The Rotary Encoder library consists of a small function to read in and handle rotary encoder switches like a [KY-040](https://www.google.com/search?q=KY-040).

In order to use this function, the GPIOs of the encoder pins need to defined in the Rotary_Encoder.c file. See defines for
```
#define ENCODER_PIN_A_GPIO			0
#define ENCODER_PIN_B_GPIO			1
#define ENCODER_SWITCH_GPIO			2
```

It is important to know, that the rotary encoder library uses the Interrupt Service Routine (ISR) for GPIO pins of the RP2040. This is important, as you can have only a single interrupt routine for GPIOs. Within this ISR you can determine, which GPIO caused the interrupt. Additional GPIOs can be added to the already existing ISR. Moving this ISR to another file, requires more effort and is not covered here.

Rotations or clicks of the rotary encoder can be polled by calling the functions
```
bool Rotary_Encoder_Rotation_CW_Occurred(void);
bool Rotary_Encoder_Rotation_CCW_Occurred(void)
bool Rotary_Encoder_Switch_Press_Occurred(void)
```

If an event has occurred, 'true' will be returned by these functions, 'false' otherwise.

Do not forget to call 
```
void Rotary_Encoder_Init(void);
```
before using polling for events.

### Other Library Dependencies
_None_

### RP2040 SDK Requirements
* pico_stdlib
* hardware_irq

## USB Serial
Basic Library to exchange data with a serial terminal on a computer. The library includes functions to send text as well as numbers in different formats (Ints and Floats).

In order for this library to work, you need to update you CMakeLists.txt with the following statement before the last line with:
```
# enable usb output, disable uart output
pico_enable_stdio_usb(${projname} 1)
pico_enable_stdio_uart(${projname} 0)
```

Within your main, you need to call the function 
```
void USB_Serial_Init(void);
```
Afterwards you can poll received data using the function
```
bool USB_Serial_Available(void);
```
Use the send-function to send data to the computer. See the comments in the header-file for further information.


### Other Library Dependencies
_None_

### RP2040 SDK Requirements
* pico_stdlib

