#include <stdint.h>

const uint8_t _Font_OLED_6x14_Bold[] = 
{
	0x0, 0x0, 	// size of zero indicates fixed width font,
	6,    		// width
	14,   		// height
	0x20, 		// first char
	0x60, 		// char count 
	 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //   0
	0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, // ! 1
	0x00, 0x1E, 0x1E, 0x00, 0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // " 2
	0x20, 0xFC, 0xFC, 0xFC, 0xFC, 0x20, 0x01, 0x0F, 0x0F, 0x0F, 0x0F, 0x01, // # 3
	0x30, 0x48, 0xFC, 0xFC, 0x88, 0x30, 0x06, 0x08, 0x1F, 0x1F, 0x08, 0x07, // $ 4
	0x18, 0x3C, 0xA4, 0x78, 0x1C, 0x0C, 0x0C, 0x0E, 0x07, 0x09, 0x0F, 0x06, // % 5
	0x00, 0xB8, 0xFC, 0x44, 0xFC, 0xB8, 0x07, 0x0F, 0x08, 0x07, 0x0F, 0x09, // & 6
	0x00, 0x00, 0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ' 7
	0x00, 0xE0, 0xF8, 0x1C, 0x06, 0x02, 0x00, 0x03, 0x0F, 0x1C, 0x30, 0x20, // ( 8
	0x00, 0x02, 0x06, 0x1C, 0xF8, 0xE0, 0x00, 0x20, 0x30, 0x1C, 0x0F, 0x03, // ) 9
	0x8C, 0x50, 0xFC, 0xFC, 0x50, 0x8C, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, // * 10
	0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, // + 11
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x3C, 0x1C, 0x00, // , 12
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // - 13
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, // . 14
	0x00, 0x00, 0xC0, 0xF0, 0x3E, 0x0E, 0x38, 0x3E, 0x07, 0x01, 0x00, 0x00, // / 15
	0xF8, 0xFC, 0x04, 0x04, 0xFC, 0xF8, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // 0 16
	0x10, 0x18, 0xFC, 0xFC, 0x00, 0x00, 0x08, 0x08, 0x0F, 0x0F, 0x08, 0x08, // 1 17
	0x18, 0x1C, 0x04, 0xC4, 0xFC, 0x38, 0x0C, 0x0E, 0x0B, 0x09, 0x08, 0x08, // 2 18
	0x18, 0x1C, 0x44, 0x44, 0xFC, 0xB8, 0x06, 0x0E, 0x08, 0x08, 0x0F, 0x07, // 3 19
	0x80, 0xC0, 0x60, 0xF8, 0xFC, 0x00, 0x03, 0x03, 0x02, 0x0F, 0x0F, 0x02, // 4 20
	0x7C, 0x7C, 0x24, 0x24, 0xE4, 0xC4, 0x06, 0x0E, 0x08, 0x08, 0x0F, 0x07, // 5 21
	0xF0, 0xF8, 0x4C, 0x44, 0xDC, 0x98, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // 6 22
	0x1C, 0x1C, 0xC4, 0xF4, 0x3C, 0x0C, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // 7 23
	0x38, 0xFC, 0xC4, 0xC4, 0xFC, 0x38, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // 8 24
	0x78, 0xFC, 0x84, 0x84, 0xFC, 0xF8, 0x06, 0x0E, 0x08, 0x0C, 0x07, 0x03, // 9 25
	0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, // : 26
	0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x12, 0x1E, 0x0E, 0x00, // ; 27
	0x80, 0xC0, 0x60, 0x30, 0x18, 0x08, 0x00, 0x01, 0x03, 0x06, 0x0C, 0x08, // < 28
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // = 29
	0x08, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x08, 0x0C, 0x06, 0x03, 0x01, 0x00, // > 30
	0x18, 0x1C, 0xC4, 0xE4, 0x3C, 0x18, 0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, // ? 31
	0xF0, 0xF8, 0xEC, 0xF4, 0x1C, 0xF8, 0x03, 0x07, 0x0D, 0x0B, 0x0A, 0x0B, // @ 32
	0xF8, 0xFC, 0x0C, 0x0C, 0xFC, 0xF8, 0x0F, 0x0F, 0x01, 0x01, 0x0F, 0x0F, // A 33
	0xFC, 0xFC, 0x44, 0x44, 0xFC, 0x98, 0x0F, 0x0F, 0x08, 0x08, 0x0F, 0x07, // B 34
	0xF8, 0xFC, 0x04, 0x04, 0x1C, 0x18, 0x07, 0x0F, 0x08, 0x08, 0x0E, 0x06, // C 35
	0xFC, 0xFC, 0x04, 0x0C, 0xF8, 0xF0, 0x0F, 0x0F, 0x08, 0x0C, 0x07, 0x03, // D 36
	0xFC, 0xFC, 0x44, 0x44, 0x44, 0x04, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, // E 37
	0xFC, 0xFC, 0x44, 0x44, 0x44, 0x04, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, // F 38
	0xF8, 0xFC, 0x04, 0x84, 0x9C, 0x98, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // G 39
	0xFC, 0xFC, 0x40, 0x40, 0xFC, 0xFC, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, // H 40
	0x04, 0x04, 0xFC, 0xFC, 0x04, 0x04, 0x08, 0x08, 0x0F, 0x0F, 0x08, 0x08, // I 41
	0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x06, 0x0E, 0x08, 0x0C, 0x07, 0x03, // J 42
	0xFC, 0xFC, 0xF0, 0x98, 0x0C, 0x04, 0x0F, 0x0F, 0x01, 0x03, 0x0E, 0x0C, // K 43
	0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, // L 44
	0xFC, 0xF8, 0x60, 0x60, 0xF8, 0xFC, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, // M 45
	0xFC, 0xFC, 0x70, 0x80, 0xFC, 0xFC, 0x0F, 0x0F, 0x00, 0x03, 0x0F, 0x0F, // N 46
	0xF8, 0xFC, 0x04, 0x04, 0xFC, 0xF8, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // O 47
	0xFC, 0xFC, 0x84, 0x84, 0xFC, 0x78, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, // P 48
	0xF8, 0xFC, 0x04, 0x04, 0xFC, 0xF8, 0x07, 0x0F, 0x09, 0x0A, 0x1F, 0x37, // Q 49
	0xFC, 0xFC, 0x44, 0xC4, 0xFC, 0x38, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, // R 50
	0x18, 0x3C, 0xE4, 0xC4, 0x1C, 0x18, 0x06, 0x0E, 0x08, 0x09, 0x0F, 0x06, // S 51
	0x04, 0x04, 0xFC, 0xFC, 0x04, 0x04, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // T 52
	0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // U 53
	0xFC, 0xFC, 0x00, 0x00, 0xFC, 0xFC, 0x01, 0x07, 0x0E, 0x0E, 0x07, 0x01, // V 54
	0xFC, 0xFC, 0x80, 0x80, 0xFC, 0xFC, 0x07, 0x0F, 0x07, 0x07, 0x0F, 0x07, // W 55
	0x0C, 0x3C, 0xF0, 0xF0, 0x3C, 0x0C, 0x0C, 0x0F, 0x03, 0x03, 0x0F, 0x0C, // x 56
	0x1C, 0x7C, 0xE0, 0xE0, 0x7C, 0x1C, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // Y 57
	0x04, 0x04, 0xC4, 0xF4, 0x3C, 0x0C, 0x0C, 0x0F, 0x0B, 0x08, 0x08, 0x08, // Z 58
	0x00, 0xFE, 0xFE, 0x02, 0x02, 0x02, 0x00, 0x3F, 0x3F, 0x20, 0x20, 0x20, // [ 59
	0x0E, 0x3E, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x3E, 0x38, // \ 60
	0x00, 0x02, 0x02, 0x02, 0xFE, 0xFE, 0x00, 0x20, 0x20, 0x20, 0x3F, 0x3F, // ] 61
	0x04, 0x06, 0x03, 0x03, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ^ 62
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // _ 63
	0x00, 0x02, 0x06, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ` 64
	0x40, 0x60, 0x20, 0xA0, 0xE0, 0xC0, 0x06, 0x0F, 0x09, 0x08, 0x0F, 0x0F, // a 65
	0xFC, 0xFC, 0x20, 0x20, 0xE0, 0xC0, 0x0F, 0x0F, 0x08, 0x08, 0x0F, 0x07, // b 66
	0xC0, 0xE0, 0x20, 0x20, 0x60, 0x40, 0x07, 0x0F, 0x08, 0x08, 0x0C, 0x04, // c 67
	0xC0, 0xE0, 0x20, 0x20, 0xFC, 0xFC, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x0F, // d 68
	0xC0, 0xE0, 0x20, 0x20, 0xE0, 0xC0, 0x07, 0x0F, 0x09, 0x09, 0x0D, 0x05, // e 69
	0x40, 0x40, 0xF8, 0xFC, 0x4C, 0x48, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // f 70
	0xC0, 0xE0, 0x20, 0xE0, 0xC0, 0x60, 0x19, 0x3F, 0x2A, 0x2B, 0x39, 0x10, // g 71
	0xFC, 0xFC, 0x20, 0x20, 0xE0, 0xC0, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, // h 72
	0x00, 0x00, 0xEC, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // i 73
	0x00, 0x00, 0x00, 0x00, 0xEC, 0xEC, 0x00, 0x10, 0x30, 0x20, 0x3F, 0x1F, // j 74
	0xFC, 0xFC, 0x80, 0xC0, 0x60, 0x00, 0x0F, 0x0F, 0x01, 0x03, 0x06, 0x0C, // k 75
	0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, // l 76
	0xE0, 0xE0, 0xC0, 0xE0, 0xE0, 0xC0, 0x0F, 0x0F, 0x07, 0x07, 0x0F, 0x0F, // m 77
	0xE0, 0xE0, 0x20, 0x20, 0xE0, 0xC0, 0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, // n 78
	0xC0, 0xE0, 0x20, 0x20, 0xE0, 0xC0, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x07, // o 79
	0xE0, 0xE0, 0x20, 0x20, 0xE0, 0xC0, 0x3F, 0x3F, 0x08, 0x08, 0x0F, 0x07, // p 80
	0xC0, 0xE0, 0x20, 0x20, 0xE0, 0xE0, 0x07, 0x0F, 0x08, 0x08, 0x3F, 0x3F, // q 81
	0xE0, 0xE0, 0x20, 0x20, 0xE0, 0xC0, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, // r 82
	0x40, 0xE0, 0xA0, 0x20, 0x60, 0x40, 0x04, 0x0C, 0x09, 0x0B, 0x0E, 0x04, // s 83
	0x20, 0x20, 0xFC, 0xFC, 0x20, 0x20, 0x00, 0x00, 0x07, 0x0F, 0x08, 0x08, // t 84
	0xE0, 0xE0, 0x00, 0x00, 0xE0, 0xE0, 0x07, 0x0F, 0x08, 0x08, 0x0F, 0x0F, // u 85
	0xE0, 0xE0, 0x00, 0x00, 0xE0, 0xE0, 0x00, 0x03, 0x0F, 0x0F, 0x03, 0x00, // v 86
	0xE0, 0xE0, 0x80, 0x80, 0xE0, 0xE0, 0x07, 0x0F, 0x07, 0x07, 0x0F, 0x07, // w 87
	0x60, 0xE0, 0x80, 0x80, 0xE0, 0x60, 0x0C, 0x0E, 0x03, 0x03, 0x0E, 0x0C, // x 88
	0x60, 0xE0, 0x80, 0x00, 0xE0, 0xE0, 0x10, 0x31, 0x27, 0x3E, 0x1F, 0x01, // y 89
	0x20, 0x20, 0x20, 0xA0, 0xE0, 0x60, 0x0C, 0x0E, 0x0B, 0x09, 0x08, 0x08, // z 90
	0x00, 0x80, 0xFC, 0x7E, 0x02, 0x02, 0x00, 0x00, 0x1F, 0x3F, 0x20, 0x20, // { 91
	0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x00, 0x00, // | 92
	0x00, 0x02, 0x02, 0x7E, 0xFC, 0x80, 0x00, 0x20, 0x20, 0x3F, 0x1F, 0x00, // } 93
	0x1C, 0x06, 0x0C, 0x0C, 0x18, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ~ 94
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F  
};