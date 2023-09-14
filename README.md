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

