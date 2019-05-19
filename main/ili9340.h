#ifndef MAIN_ILI9340_H_
#define MAIN_ILI9340_H_

#include "driver/spi_master.h"
#include "fontx.h"

#define RED				0xf800
#define GREEN			0x07e0
#define BLUE			0x001f
#define BLACK			0x0000
#define WHITE			0xffff
#define GRAY			0x8c51
#define YELLOW			0xFFE0
#define CYAN			0x07FF
#define PURPLE			0xF81F


#define DIRECTION0		0
#define DIRECTION90		1
#define DIRECTION180	2
#define DIRECTION270	3


typedef struct {
	uint16_t _width;
	uint16_t _height;
	uint16_t _offsetx;
	uint16_t _offsety;
	uint16_t _font_direction;
	uint16_t _font_fill;
	uint16_t _font_fill_color;
	uint16_t _font_underline;
	uint16_t _font_underline_color;
	int16_t _dc;
	int16_t _bl;
	spi_device_handle_t _SPIHandle;
} ILI9340_t;

void spi_master_init(ILI9340_t * dev, int16_t GPIO_CS, int16_t GPIO_DC, int16_t GPIO_RESET, int16_t GPIO_BL);
bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength);
bool spi_master_write_command(ILI9340_t * dev, uint8_t cmd);
bool spi_master_write_data_byte(ILI9340_t * dev, uint8_t data);
bool spi_master_write_data_word(ILI9340_t * dev, uint16_t data);

void lcdInit(ILI9340_t * dev, int width, int height, int offsetx, int offsety);
void lcdDrawPixel(ILI9340_t * dev, uint16_t x, uint16_t y, uint16_t color);
void lcdDrawFillRect(ILI9340_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDisplayOff(ILI9340_t * dev);
void lcdDisplayOn(ILI9340_t * dev);
void lcdFillScreen(ILI9340_t * dev, uint16_t color);
void lcdDrawLine(ILI9340_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDrawRect(ILI9340_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDrawCircle(ILI9340_t * dev, uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void lcdDrawFillCircle(ILI9340_t * dev, uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void lcdDrawRoundRect(ILI9340_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color);
void lcdDrawArrow(ILI9340_t * dev, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
void lcdDrawFillArrow(ILI9340_t * dev, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
uint16_t rgb565_conv(uint16_t r, uint16_t g, uint16_t b);
int lcdDrawChar(ILI9340_t * dev, FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color);
int lcdDrawString(ILI9340_t * dev, FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color);
//int lcdDrawSJISChar(ILI9340_t * dev, FontxFile *fx, uint16_t x, uint16_t y, uint16_t sjis, uint16_t color);
//int lcdDrawUTF8Char(ILI9340_t * dev, FontxFile *fx, uint16_t x, uint16_t y, uint8_t *utf8, uint16_t color);
//int lcdDrawUTF8String(ILI9340_t * dev, FontxFile *fx, uint16_t x, uint16_t y, unsigned char *utfs, uint16_t color);
void lcdSetFontDirection(ILI9340_t * dev, uint16_t);
void lcdSetFontFill(ILI9340_t * dev, uint16_t color);
void lcdUnsetFontFill(ILI9340_t * dev);
void lcdSetFontUnderLine(ILI9340_t * dev, uint16_t color);
void lcdUnsetFontUnderLine(ILI9340_t * dev);
void lcdBacklightOff(ILI9340_t * dev);
void lcdBacklightOn(ILI9340_t * dev);
void lcdInversionOn(ILI9340_t * dev);
#endif /* MAIN_ILI9340_H_ */

