# RP2040-Library

These is my self created libraries for the RP2040. I use the libraries for my personal projects, so do not expect professional code.
Please consider that this repository has been set up short term, so there is not a lot of explanation. In case of questions, don't hestitate
to contact me or to raise a ticket here in GitHUB.
The following libraries are included:

## I2C Master
A basic collection I2C Master functions, sending and receive. Sending can be done via DMA as well.
When using the libary, you need to configure the correct GPIO pins and the correct I2C hardware unit of the PR2040.
See the defines section in I2C_Master.c file


## OLED SSD1306
This is a basic library for the OLED display SSD1306. A hight of 64 and 32 pixels is supported. This need to be defined in the defines section
of the OLED_SSD1306.h file. You can define the display data from the local buffer to the display yo be transfered blocking or by DMA in the
function 
```
void SSD1306_Transmit_Buffer()
```
within the OLED_SSD1306.c file. 

Fonts are also supported, but only fonts with fixed width. See the Fonts subfolder for some examples.
Icons or images can been shown as well. I also attached some example images.

### Dependencies
* I2C Master 