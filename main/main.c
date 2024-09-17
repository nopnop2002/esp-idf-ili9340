#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "ili9340.h"
#include "fontx.h"
#include "bmpfile.h"
#include "decode_jpeg.h"
#include "decode_png.h"
#include "pngle.h"

#include "driver/gpio.h"

#define INTERVAL 400
#define WAIT vTaskDelay(INTERVAL)

static const char *TAG = "MAIN";

// You have to set these CONFIG value using menuconfig.
#if 0
#define CONFIG_WIDTH	240
#define CONFIG_HEIGHT 320
#define CONFIG_TFT_CS_GPIO 5
#define CONFIG_DC_GPIO 26
#define CONFIG_RESET_GPIO 2
#define CONFIG_BL_GPIO 2
#endif

TickType_t FillTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, RED);
	vTaskDelay(50);
	lcdFillScreen(dev, GREEN);
	vTaskDelay(50);
	lcdFillScreen(dev, BLUE);
	vTaskDelay(50);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t ColorBarTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	if (width < height) {
		uint16_t y1,y2;
		y1 = height/3;
		y2 = (height/3)*2;
		lcdDrawFillRect(dev, 0, 0, width-1, y1-1, RED);
		vTaskDelay(1);
		lcdDrawFillRect(dev, 0, y1-1, width-1, y2-1, GREEN);
		vTaskDelay(1);
		lcdDrawFillRect(dev, 0, y2-1, width-1, height-1, BLUE);
	} else {
		uint16_t x1,x2;
		x1 = width/3;
		x2 = (width/3)*2;
		lcdDrawFillRect(dev, 0, 0, x1-1, height-1, RED);
		vTaskDelay(1);
		lcdDrawFillRect(dev, x1-1, 0, x2-1, height-1, GREEN);
		vTaskDelay(1);
		lcdDrawFillRect(dev, x2-1, 0, width-1, height-1, BLUE);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t ArrowTest(TFT_t * dev, FontxFile *fx, uint16_t model, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);
	
	uint16_t xpos;
	uint16_t ypos;
	int	stlen;
	uint8_t ascii[24];
	uint16_t color;

	lcdFillScreen(dev, BLACK);

	if (model == 0x9225) strcpy((char *)ascii, "ILI9225");
	if (model == 0x9226) strcpy((char *)ascii, "ILI9225G");
	if (model == 0x9340) strcpy((char *)ascii, "ILI9340");
	if (model == 0x9341) strcpy((char *)ascii, "ILI9341");
	if (model == 0x7735) strcpy((char *)ascii, "ST7735");
	if (model == 0x7796) strcpy((char *)ascii, "ST7796S");
	if (width < height) {
		xpos = ((width - fontHeight) / 2) - 1;
		ypos = (height - (strlen((char *)ascii) * fontWidth)) / 2;
		lcdSetFontDirection(dev, DIRECTION90);
	} else {
		ypos = ((height - fontHeight) / 2) - 1;
		xpos = (width - (strlen((char *)ascii) * fontWidth)) / 2;
		lcdSetFontDirection(dev, DIRECTION0);
	}
	color = WHITE;
	lcdDrawString(dev, fx, xpos, ypos, ascii, color);

	lcdSetFontDirection(dev, DIRECTION0);
	//lcdFillScreen(dev, WHITE);
	color = RED;
	lcdDrawFillArrow(dev, 10, 10, 0, 0, 5, color);
	strcpy((char *)ascii, "0,0");
	lcdDrawString(dev, fx, 0, 30, ascii, color);

	color = GREEN;
	lcdDrawFillArrow(dev, width-11, 10, width-1, 0, 5, color);
	//strcpy((char *)ascii, "79,0");
	sprintf((char *)ascii, "%d,0",width-1);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
	lcdDrawString(dev, fx, xpos, 30, ascii, color);

	color = GRAY;
	lcdDrawFillArrow(dev, 10, height-11, 0, height-1, 5, color);
	//strcpy((char *)ascii, "0,159");
	sprintf((char *)ascii, "0,%d",height-1);
	ypos = (height-11) - (fontHeight) + 5;
	lcdDrawString(dev, fx, 0, ypos, ascii, color);

	color = CYAN;
	lcdDrawFillArrow(dev, width-11, height-11, width-1, height-1, 5, color);
	//strcpy((char *)ascii, "79,159");
	sprintf((char *)ascii, "%d,%d",width-1, height-1);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
	lcdDrawString(dev, fx, xpos, ypos, ascii, color);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t DirectionTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=0");
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, fontHeight-1, ascii, color);

	color = BLUE;
	strcpy((char *)ascii, "Direction=2");
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, (width-1), (height-1)-(fontHeight*1), ascii, color);

	color = CYAN;
	strcpy((char *)ascii, "Direction=1");
	lcdSetFontDirection(dev, 1);
	lcdDrawString(dev, fx, (width-1)-fontHeight, 0, ascii, color);

	color = GREEN;
	strcpy((char *)ascii, "Direction=3");
	lcdSetFontDirection(dev, 3);
	lcdDrawString(dev, fx, (fontHeight-1), height-1, ascii, color);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t HorizontalTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=0");
	lcdSetFontDirection(dev, 0);
	lcdDrawString(dev, fx, 0, fontHeight*1-1, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, fontHeight*2-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, 0, fontHeight*3-1, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, 0, fontHeight*4-1, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	strcpy((char *)ascii, "Direction=2");
	lcdSetFontDirection(dev, 2);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*1)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*2)-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*3)-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, width-1, height-(fontHeight*4)-1, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t VerticalTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[20];

	color = RED;
	strcpy((char *)ascii, "Direction=1");
	lcdSetFontDirection(dev, 1);
	lcdDrawString(dev, fx, width-(fontHeight*1), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*2), 0, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, GREEN);
	lcdDrawString(dev, fx, width-(fontHeight*3), 0, ascii, color);
	lcdSetFontUnderLine(dev, RED);
	lcdDrawString(dev, fx, width-(fontHeight*4), 0, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	color = BLUE;
	strcpy((char *)ascii, "Direction=3");
	lcdSetFontDirection(dev, 3);
	lcdDrawString(dev, fx, (fontHeight*1)-1, height-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*2)-1, height-1, ascii, color);
	lcdUnsetFontUnderLine(dev);

	lcdSetFontFill(dev, YELLOW);
	lcdDrawString(dev, fx, (fontHeight*3)-1, height-1, ascii, color);
	lcdSetFontUnderLine(dev, BLUE);
	lcdDrawString(dev, fx, (fontHeight*4)-1, height-1, ascii, color);
	lcdUnsetFontFill(dev);
	lcdUnsetFontUnderLine(dev);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}


TickType_t LineTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color=RED;
	for(int ypos=0;ypos<height;ypos=ypos+10) {
		lcdDrawLine(dev, 0, ypos, width, ypos, color);
	}

	for(int xpos=0;xpos<width;xpos=xpos+10) {
		lcdDrawLine(dev, xpos, 0, xpos, height, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t CircleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = GRAY;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;
	for(int i=5;i<height;i=i+5) {
		lcdDrawCircle(dev, xpos, ypos, i, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t RectAngleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = CYAN;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;

	uint16_t w = width * 0.6;
	uint16_t h = w * 0.5;
	int angle;
	for(angle=0;angle<=(360*3);angle=angle+30) {
		lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, color);
		usleep(10000);
		lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, BLACK);
	}

	for(angle=0;angle<=180;angle=angle+30) {
		lcdDrawRectAngle(dev, xpos, ypos, w, h, angle, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t TriangleTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = CYAN;
	uint16_t xpos = width/2;
	uint16_t ypos = height/2;

	uint16_t w;
	uint16_t h;
	if (height > width) {
		w = width * 0.6;
		h = w * 1.0;
	} else {
		w = height * 0.6;
		h = w * 1.0;
	}
	int angle;

	for(angle=0;angle<=(360*3);angle=angle+30) {
		lcdDrawTriangle(dev, xpos, ypos, w, h, angle, color);
		usleep(10000);
		lcdDrawTriangle(dev, xpos, ypos, w, h, angle, BLACK);
	}

	for(angle=0;angle<=360;angle=angle+30) {
		lcdDrawTriangle(dev, xpos, ypos, w, h, angle, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t RoundRectTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();


	uint16_t color;
	uint16_t limit = width;
	if (width > height) limit = height;
	//lcdFillScreen(dev, WHITE);
	lcdFillScreen(dev, BLACK);
	color = BLUE;
	for(int i=5;i<limit;i=i+5) {
		if (i > (limit-i-1) ) break;
		//ESP_LOGI(__FUNCTION__, "i=%d, width-i-1=%d",i, width-i-1);
		lcdDrawRoundRect(dev, i, i, (width-i-1), (height-i-1), 10, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t FillRectTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	lcdFillScreen(dev, CYAN);

	uint16_t red;
	uint16_t green;
	uint16_t blue;
	srand( (unsigned int)time( NULL ) );
	for(int i=1;i<100;i++) {
		red=rand()%255;
		green=rand()%255;
		blue=rand()%255;
		color=rgb565(red, green, blue);
		uint16_t xpos=rand()%width;
		uint16_t ypos=rand()%height;
		uint16_t size=rand()%(width/5);
		lcdDrawFillRect(dev, xpos, ypos, xpos+size, ypos+size, color);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t ColorTest(TFT_t * dev, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	uint16_t color;
	lcdFillScreen(dev, WHITE);
	color = RED;
	uint16_t delta = height/16;
	uint16_t ypos = 0;
	for(int i=0;i<16;i++) {
		//ESP_LOGI(__FUNCTION__, "color=0x%x",color);
		lcdDrawFillRect(dev, 0, ypos, width-1, ypos+delta, color);
		color = color >> 1;
		ypos = ypos + delta;
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

#if CONFIG_ILI9340 || CONFIG_ILI9341 || CONFIG_ST7796
TickType_t ScrollTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	uint8_t ascii[30];

	int lines = (height - fontHeight) / fontHeight;
	ESP_LOGD(__FUNCTION__, "height=%d fontHeight=%d lines=%d", height, fontHeight, lines);
	int ymax = (lines+1) * fontHeight;
	ESP_LOGD(__FUNCTION__, "ymax=%d",ymax);

	lcdSetFontDirection(dev, DIRECTION0);
	lcdFillScreen(dev, BLACK);

	strcpy((char *)ascii, "Vertical Smooth Scroll");
	lcdDrawString(dev, fx, 0, fontHeight-1, ascii, RED);

	color = CYAN;
	uint16_t vsp = fontHeight*2;
	uint16_t ypos = fontHeight*2-1;
	//for(int i=0;i<30;i++) {
	for(int i=0;i<lines+10;i++) {
		ESP_LOGD(__FUNCTION__, "i=%d ypos=%d", i, ypos);
		sprintf((char *)ascii, "This is text line %d", i);
		if (i < lines) {
			lcdDrawString(dev, fx, 0, ypos, ascii, color);
		} else {
			lcdDrawFillRect(dev, 0, ypos-fontHeight, width-1, ypos, BLACK);
			lcdSetScrollArea(dev, fontHeight, (height-fontHeight), 0);
			lcdScroll(dev, vsp);
			vsp = vsp + fontHeight;
			if (vsp > ymax) vsp = fontHeight*2;
			lcdDrawString(dev, fx, 0, ypos, ascii, color);
		}
		ypos = ypos + fontHeight;
		if (ypos > ymax) ypos = fontHeight*2-1;
		vTaskDelay(25);
	}

	// Initialize scroll area
	//lcdSetScrollArea(dev, 0, 0x0140, 0);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

void ScrollReset(TFT_t * dev, int width, int height) {
	//lcdResetScrollArea(dev, 320);
	//lcdResetScrollArea(dev, 240);
	lcdResetScrollArea(dev, height);
	lcdScroll(dev, 0);
}
#endif

#if CONFIG_ILI9225 || CONFIG_ILI9225G
TickType_t ScrollTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	uint8_t ascii[30];

	int lines = (height - fontHeight) / fontHeight;
	ESP_LOGD(__FUNCTION__, "height=%d fontHeight=%d lines=%d", height, fontHeight, lines);
	int ymax = (lines+1) * fontHeight;
	ESP_LOGD(__FUNCTION__, "ymax=%d",ymax);

	lcdSetFontDirection(dev, DIRECTION0);
	lcdFillScreen(dev, BLACK);

	// Reset scroll area
	lcdSetScrollArea(dev, 0, 0, 0);

	strcpy((char *)ascii, "Vertical Smooth Scroll");
	lcdDrawString(dev, fx, 0, fontHeight-1, ascii, RED);

	color = CYAN;
	uint16_t vsp = fontHeight*1;
	uint16_t ypos = fontHeight*2-1;
	//for(int i=0;i<30;i++) {
	for(int i=0;i<lines+10;i++) {
		ESP_LOGD(__FUNCTION__, "i=%d ypos=%d vsp=%d", i, ypos, vsp);
		sprintf((char *)ascii, "This is text line %d", i);
		if (i < lines) {
			lcdDrawString(dev, fx, 0, ypos, ascii, color);
		} else {
			lcdDrawFillRect(dev, 0, ypos-fontHeight, width-1, ypos, BLACK);
			//lcdSetScrollArea(dev, fontHeight, height-fontHeight, 0);
			lcdSetScrollArea(dev, fontHeight, ymax, 0);
			lcdScroll(dev, vsp);
			vsp = vsp + fontHeight;
			if (vsp > (ymax-fontHeight)) vsp = fontHeight*1;
			lcdDrawString(dev, fx, 0, ypos, ascii, color);
		}
		ypos = ypos + fontHeight;
		if (ypos > ymax) ypos = fontHeight*2-1;
		vTaskDelay(25);
	}

	// Reset scroll area
	//lcdSetScrollArea(dev, 0, 0, 0);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

void ScrollReset(TFT_t * dev, int width, int height) {
	lcdResetScrollArea(dev, 0);
}
#endif

#if CONFIG_ST7735
TickType_t ScrollTest(TFT_t * dev, FontxFile *fx, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint16_t color;
	uint8_t ascii[30];

	typedef struct {
		bool enable;
		uint16_t color;;
		char line[30];
	} SAVE_t;


	int lines = (height - fontHeight) / fontHeight;
	ESP_LOGD(__FUNCTION__, "height=%d fontHeight=%d lines=%d", height, fontHeight, lines);
	SAVE_t save[20];
	for(int i=0;i<lines;i++) {
		save[i].enable = false;
	}

	lcdSetFontDirection(dev, DIRECTION0);
	lcdFillScreen(dev, BLACK);

	strcpy((char *)ascii, "Vertical Scroll");
	lcdDrawString(dev, fx, 0, fontHeight-1, ascii, RED);

	color = CYAN;
	for(int i=0;i<20;i++) {
		sprintf((char *)ascii, "This is text line %d", i);
		int last = -1;
		bool renew = false;
		for(int j=(lines-1);j>=0;j--) {
			if (save[j].enable == false) last = j;
		}
		if (last == -1) {
			last = lines-1;
			renew = true;
			for(int j=0;j<lines-1;j++) {
				save[j].enable = save[j+1].enable;
				save[j].color = save[j+1].color;
				strcpy(save[j].line, save[j+1].line);
			}
		}
		save[last].enable = true;
		save[last].color = color;
		strcpy(save[last].line,  (char*)ascii);
		
		if (renew) {
			for(int j=0;j<lines;j++) {
				ESP_LOGD(__FUNCTION__, "enable[%d]=%d",j, save[j].enable);
				lcdDrawFillRect(dev, 0, fontHeight*(j+1)-1, width-1, fontHeight*(j+2)-1, BLACK);
				lcdDrawString(dev, fx, 0, fontHeight*(j+2)-1, (uint8_t *)save[j].line, save[j].color);
			}
		} else {
			lcdDrawString(dev, fx, 0, fontHeight*(last+2)-1, (uint8_t *)save[last].line, save[last].color);
		}
		vTaskDelay(25);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}
void ScrollReset(TFT_t * dev, int width, int height) {

}
#endif

TickType_t BMPTest(TFT_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, BLACK);

	// open BMP file
	esp_err_t ret;
	FILE* fp = fopen(file, "rb");
	if (fp == NULL) {
		ESP_LOGW(__FUNCTION__, "File not found [%s]", file);
		return 0;
	}

	// read bmp header
	bmpfile_t *result = (bmpfile_t*)malloc(sizeof(bmpfile_t));
	if (result == NULL) {
		ESP_LOGE(__FUNCTION__, "malloc fail");
		return 0;
	}

	ret = fread(result->header.magic, 1, 2, fp);
	assert(ret == 2);
	ESP_LOGD(__FUNCTION__,"result->header.magic=%c %c", result->header.magic[0], result->header.magic[1]);
	if (result->header.magic[0]!='B' || result->header.magic[1] != 'M') {
		ESP_LOGW(__FUNCTION__, "File is not BMP");
		free(result);
		fclose(fp);
		return 0;
	}
	ret = fread(&result->header.filesz, 4, 1 , fp);
	assert(ret == 1);
	ESP_LOGD(__FUNCTION__,"result->header.filesz=%"PRIu32, result->header.filesz);
	ret = fread(&result->header.creator1, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->header.creator2, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->header.offset, 4, 1, fp);
	assert(ret == 1);

	// read dib header
	ret = fread(&result->dib.header_sz, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.width, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.height, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.nplanes, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.depth, 2, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.compress_type, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.bmp_bytesz, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.hres, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.vres, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.ncolors, 4, 1, fp);
	assert(ret == 1);
	ret = fread(&result->dib.nimpcolors, 4, 1, fp);
	assert(ret == 1);

	if((result->dib.depth == 24) && (result->dib.compress_type == 0)) {
		// BMP rows are padded (if needed) to 4-byte boundary
		uint32_t rowSize = (result->dib.width * 3 + 3) & ~3;
		int w = result->dib.width;
		int h = result->dib.height;
		ESP_LOGD(__FUNCTION__,"w=%d h=%d", w, h);
		int _x;
		int _w;
		int _cols;
		int _cole;
		if (width >= w) {
			_x = (width - w) / 2;
			_w = w;
			_cols = 0;
			_cole = w - 1;
		} else {
			_x = 0;
			_w = width;
			_cols = (w - width) / 2;
			_cole = _cols + width - 1;
		}
		ESP_LOGD(__FUNCTION__,"_x=%d _w=%d _cols=%d _cole=%d",_x, _w, _cols, _cole);

		int _y;
		int _rows;
		int _rowe;
		if (height >= h) {
			_y = (height - h) / 2;
			_rows = 0;
			_rowe = h -1;
		} else {
			_y = 0;
			_rows = (h - height) / 2;
			_rowe = _rows + height - 1;
		}
		ESP_LOGD(__FUNCTION__,"_y=%d _rows=%d _rowe=%d", _y, _rows, _rowe);

#define BUFFPIXEL 20
		uint8_t sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
		uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * w);
		if (colors == NULL) {
			ESP_LOGE(__FUNCTION__, "malloc fail");
			free(result);
			fclose(fp);
			return 0;
		}

		for (int row=0; row<h; row++) { // For each scanline...
			if (row < _rows || row > _rowe) continue;
			// Seek to start of scan line.	It might seem labor-
			// intensive to be doing this on every line, but this
			// method covers a lot of gritty details like cropping
			// and scanline padding.	Also, the seek only takes
			// place if the file position actually needs to change
			// (avoids a lot of cluster math in SD library).
			// Bitmap is stored bottom-to-top order (normal BMP)
			int pos = result->header.offset + (h - 1 - row) * rowSize;
			fseek(fp, pos, SEEK_SET);
			int buffidx = sizeof(sdbuffer); // Force buffer reload

			int index = 0;
			for (int col=0; col<w; col++) { // For each pixel...
				if (buffidx >= sizeof(sdbuffer)) { // Indeed
					fread(sdbuffer, sizeof(sdbuffer), 1, fp);
					buffidx = 0; // Set index to beginning
				}
				if (col < _cols || col > _cole) continue;
				// Convert pixel from BMP to TFT format, push to display
				uint8_t b = sdbuffer[buffidx++];
				uint8_t g = sdbuffer[buffidx++];
				uint8_t r = sdbuffer[buffidx++];
				colors[index++] = rgb565(r, g, b);
			} // end for col
			ESP_LOGD(__FUNCTION__,"lcdDrawMultiPixels row=%d",row);
			//lcdDrawMultiPixels(dev, _x, row+_y, _w, colors);
			lcdDrawMultiPixels(dev, _x, _y, _w, colors);
			_y++;
		} // end for row
		free(colors);
	} // end if 
	free(result);
	fclose(fp);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t JPEGTest(TFT_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, BLACK);

	int _width = width;
	if (width > 240) _width = 240;
	int _height = height;
	if (height > 320) _height = 320;

	pixel_jpeg **pixels;
	int imageWidth;
	int imageHeight;
	esp_err_t err = decode_jpeg(&pixels, file, _width, _height, &imageWidth, &imageHeight);
	if (err == ESP_OK) {
		ESP_LOGI(__FUNCTION__, "imageWidth=%d imageHeight=%d", imageWidth, imageHeight);

		uint16_t jpegWidth = width;
		uint16_t offsetX = 0;
		if (width > imageWidth) {
			jpegWidth = imageWidth;
			offsetX = (width - imageWidth) / 2;
		}
		ESP_LOGD(__FUNCTION__, "jpegWidth=%d offsetX=%d", jpegWidth, offsetX);

		uint16_t jpegHeight = height;
		uint16_t offsetY = 0;
		if (height > imageHeight) {
			jpegHeight = imageHeight;
			offsetY = (height - imageHeight) / 2;
		}
		ESP_LOGD(__FUNCTION__, "jpegHeight=%d offsetY=%d", jpegHeight, offsetY);
		uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * jpegWidth);
		if (colors == NULL) {
			ESP_LOGE(__FUNCTION__, "malloc fail");
			release_image(&pixels, _width, _height);
			return 0;
		}

#if 0
		for(int y = 0; y < jpegHeight; y++){
			for(int x = 0;x < jpegWidth; x++){
				pixel_jpeg pixel = pixels[y][x];
				uint16_t color = rgb565(pixel.red, pixel.green, pixel.blue);
				lcdDrawPixel(dev, x+offsetX, y+offsetY, color);
			}
			vTaskDelay(1);
		}
#endif

		for(int y = 0; y < jpegHeight; y++){
			for(int x = 0;x < jpegWidth; x++){
				//pixel_jpeg pixel = pixels[y][x];
				//colors[x] = rgb565(pixel.red, pixel.green, pixel.blue);
				colors[x] = pixels[y][x];
			}
			lcdDrawMultiPixels(dev, offsetX, y+offsetY, jpegWidth, colors);
			vTaskDelay(1);
		}

		free(colors);
		release_image(&pixels, _width, _height);
		ESP_LOGD(__FUNCTION__, "Finish");
	} else {
		ESP_LOGE(__FUNCTION__, "decode_jpeg err=%d imageWidth=%d imageHeight=%d", err, imageWidth, imageHeight);
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

TickType_t PNGTest(TFT_t * dev, char * file, int width, int height) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	lcdFillScreen(dev, BLACK);

	int _width = width;
	if (width > 240) _width = 240;
	int _height = height;
	if (height > 320) _height = 320;

	// open PNG file
	FILE* fp = fopen(file, "rb");
	if (fp == NULL) {
		ESP_LOGW(__FUNCTION__, "File not found [%s]", file);
		return 0;
	}

	char buf[1024];
	size_t remain = 0;
	int len;

	pngle_t *pngle = pngle_new(_width, _height);
	if (pngle == NULL) {
		fclose(fp);
		return 0;
	}

	pngle_set_init_callback(pngle, png_init);
	pngle_set_draw_callback(pngle, png_draw);
	pngle_set_done_callback(pngle, png_finish);

	double display_gamma = 2.2;
	pngle_set_display_gamma(pngle, display_gamma);


	while (!feof(fp)) {
		if (remain >= sizeof(buf)) {
			ESP_LOGE(__FUNCTION__, "Buffer exceeded");
			while(1) vTaskDelay(1);
		}

		len = fread(buf + remain, 1, sizeof(buf) - remain, fp);
		if (len <= 0) {
			//printf("EOF\n");
			break;
		}

		int fed = pngle_feed(pngle, buf, remain + len);
		if (fed < 0) {
			ESP_LOGE(__FUNCTION__, "ERROR; %s", pngle_error(pngle));
			while(1) vTaskDelay(1);
		}

		remain = remain + len - fed;
		if (remain > 0) memmove(buf, buf + fed, remain);
	}

	fclose(fp);

	uint16_t pngWidth = width;
	uint16_t offsetX = 0;
	if (width > pngle->imageWidth) {
		pngWidth = pngle->imageWidth;
		offsetX = (width - pngle->imageWidth) / 2;
	}
	ESP_LOGD(__FUNCTION__, "pngWidth=%d offsetX=%d", pngWidth, offsetX);

	uint16_t pngHeight = height;
	uint16_t offsetY = 0;
	if (height > pngle->imageHeight) {
		pngHeight = pngle->imageHeight;
		offsetY = (height - pngle->imageHeight) / 2;
	}
	ESP_LOGD(__FUNCTION__, "pngHeight=%d offsetY=%d", pngHeight, offsetY);
	uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * pngWidth);
	if (colors == NULL) {
		ESP_LOGE(__FUNCTION__, "malloc fail");
		pngle_destroy(pngle, _width, _height);
		return 0;
	}

#if 0
	for(int y = 0; y < pngHeight; y++){
		for(int x = 0;x < pngWidth; x++){
			pixel_png pixel = pngle->pixels[y][x];
			uint16_t color = rgb565(pixel.red, pixel.green, pixel.blue);
			lcdDrawPixel(dev, x+offsetX, y+offsetY, color);
		}
	}
#endif

	for(int y = 0; y < pngHeight; y++){
		for(int x = 0;x < pngWidth; x++){
			//pixel_png pixel = pngle->pixels[y][x];
			//colors[x] = rgb565(pixel.red, pixel.green, pixel.blue);
			colors[x] = pngle->pixels[y][x];
		}
		lcdDrawMultiPixels(dev, offsetX, y+offsetY, pngWidth, colors);
		vTaskDelay(1);
	}
	free(colors);
	pngle_destroy(pngle, _width, _height);

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

void ShowPngImage(TFT_t * dev, char * file, int width, int height, int xpos, int ypos) {
	// open PNG file
	FILE* fp = fopen(file, "rb");
	if (fp == NULL) {
		ESP_LOGW(__FUNCTION__, "File not found [%s]", file);
		return;
	}

	char buf[1024];
	size_t remain = 0;
	int len;

	int _width = width;
	if (width > 240) _width = 240;
	int _height = height;
	if (height > 320) _height = 320;
	pngle_t *pngle = pngle_new(_width, _height);

	pngle_set_init_callback(pngle, png_init);
	pngle_set_draw_callback(pngle, png_draw);
	pngle_set_done_callback(pngle, png_finish);

	double display_gamma = 2.2;
	pngle_set_display_gamma(pngle, display_gamma);


	while (!feof(fp)) {
		if (remain >= sizeof(buf)) {
			ESP_LOGE(__FUNCTION__, "Buffer exceeded");
			while(1) vTaskDelay(1);
		}

		len = fread(buf + remain, 1, sizeof(buf) - remain, fp);
		if (len <= 0) {
			//printf("EOF\n");
			break;
		}

		int fed = pngle_feed(pngle, buf, remain + len);
		if (fed < 0) {
			ESP_LOGE(__FUNCTION__, "ERROR; %s", pngle_error(pngle));
			while(1) vTaskDelay(1);
		}

		remain = remain + len - fed;
		if (remain > 0) memmove(buf, buf + fed, remain);
	}

	fclose(fp);

	uint16_t pngWidth = pngle_get_width(pngle);
	uint16_t pngHeight = pngle_get_height(pngle);
	ESP_LOGD(__FUNCTION__, "pngWidth=%d pngHeight=%d", pngWidth, pngHeight);
	int _xpos = xpos - (pngHeight/2);
	int _ypos = ypos - (pngWidth/2);
	ESP_LOGD(__FUNCTION__, "xpos=%d ypos=%d _xpos=%d _ypos=%d", xpos, ypos, _xpos, _ypos);
	uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * pngWidth);
	if (colors == NULL) {
		ESP_LOGE(__FUNCTION__, "malloc fail");
		pngle_destroy(pngle, _width, _height);
		return;
	}

	for(int y = 0; y < pngHeight; y++){
		for(int x = 0;x < pngWidth; x++){
			//pixel_png pixel = pngle->pixels[y][x];
			//colors[x] = rgb565(pixel.red, pixel.green, pixel.blue);
			colors[x] = pngle->pixels[y][x];
		}
		lcdDrawMultiPixels(dev, _xpos, y+_ypos, pngWidth, colors);
		vTaskDelay(1);
	}
	free(colors);
	pngle_destroy(pngle, _width, _height);
	return;
}

TickType_t CodeTest(TFT_t * dev, FontxFile *fx, int width, int height, uint16_t start, uint16_t end) {
	TickType_t startTick, endTick, diffTick;
	startTick = xTaskGetTickCount();

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);
	uint8_t xmoji = width / fontWidth;
	uint8_t ymoji = height / fontHeight;
	//ESP_LOGI(__FUNCTION__,"xmoji=%d ymoji=%d",xmoji, ymoji);


	uint16_t color;
	lcdFillScreen(dev, BLACK);
	uint8_t code;

	color = CYAN;
	lcdSetFontDirection(dev, DIRECTION0);
	//code = 0xA0;
	code = start;
	for(int y=0;y<ymoji;y++) {
		uint16_t xpos = 0;
		uint16_t ypos =  fontHeight*(y+1)-1;
		for(int x=0;x<xmoji;x++) {
			xpos = lcdDrawCode(dev, fx, xpos, ypos, code, color);
			if (code == 0xFF) break;
			code++;
			if (code > end) break;
		}
		if (code == 0xFF) break;
		if (code > end) break;
	}

	endTick = xTaskGetTickCount();
	diffTick = endTick - startTick;
	ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
	return diffTick;
}

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
void TouchPosition(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	ESP_LOGW(__FUNCTION__, "Start TouchPosition");

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	TickType_t lastTouched = xTaskGetTickCount();
	lcdFillScreen(dev, BLACK);
	uint8_t ascii[24];
	strcpy((char *)ascii, "Touch any place");
	int ypos = ((height - fontHeight) / 2) - 1;
	int xpos = (width + (strlen((char *)ascii) * fontWidth)) / 2;
	lcdSetFontDirection(dev, DIRECTION180);
	lcdDrawString(dev, fx, xpos, ypos, ascii, WHITE);

	// Clear XPT2046
	int _xp;
	int _yp;
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) break;
	} // end while
	
	bool isRunning = true;
	while(isRunning) {
		if (touch_getxy(dev, &_xp, &_yp)) {
			lastTouched = xTaskGetTickCount();
			ESP_LOGI(__FUNCTION__, "TouchPosition _xp=%d _yp=%d", _xp, _yp);
		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) {
				isRunning = false;
			}
		}
	} // end while
	ESP_LOGW(__FUNCTION__, "End TouchPosition");
}

bool TouchGetCalibration(TFT_t * dev) {
	// Open NVS
	ESP_LOGI(__FUNCTION__, "Opening Non-Volatile Storage (NVS) handle... ");
	nvs_handle_t my_handle;
	esp_err_t err;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) opening NVS handle!", esp_err_to_name(err));
		return false;
	}
	ESP_LOGI(__FUNCTION__, "Opening Non-Volatile Storage (NVS) handle done");

	// Read NVS
	int16_t calibration;
	err = nvs_get_i16(my_handle, "calibration", &calibration);
	ESP_LOGI(__FUNCTION__, "nvs_get_i16(calibration)=%d", err);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
		
	err = nvs_get_i16(my_handle, "touch_min_xc", &dev->_min_xc);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_min_yc", &dev->_min_yc);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_max_xc", &dev->_max_xc);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_max_yc", &dev->_max_yc);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_min_xp", &dev->_min_xp);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_min_yp", &dev->_min_yp);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_max_xp", &dev->_max_xp);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}
	err = nvs_get_i16(my_handle, "touch_max_yp", &dev->_max_yp);
	if (err != ESP_OK) {
		nvs_close(my_handle);
		return false;
	}

	// Close NVS
	nvs_close(my_handle);

	return true;
}

void TouchCalibration(TFT_t * dev, FontxFile *fx, int width, int height) {
	if (dev->_calibration == false) return;

	// Read Touch Calibration from NVS
	bool ret = TouchGetCalibration(dev);
	ESP_LOGI(__FUNCTION__, "TouchGetCalibration=%d", ret);
	if (ret) {
		ESP_LOGI(__FUNCTION__, "Restore Touch Calibration from NVS");
		dev->_calibration = false;
		return;
	}

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint8_t ascii[24];
	int xpos = 0;
	int ypos = 0;

	// Calibration
	lcdFillScreen(dev, BLACK);
	dev->_min_xc = 15;
	dev->_min_yc = 15;
	//lcdDrawFillCircle(dev, 10, 10, 10, CYAN);
	lcdDrawFillCircle(dev, dev->_min_xc, dev->_min_yc, 10, CYAN);
	strcpy((char *)ascii, "Calibration");
	ypos = ((height - fontHeight) / 2) - 1;
	xpos = (width + (strlen((char *)ascii) * fontWidth)) / 2;
	lcdSetFontDirection(dev, DIRECTION180);
	lcdDrawString(dev, fx, xpos, ypos, ascii, WHITE);
	ypos = ypos - fontHeight;
	int _xpos = xpos;
	for(int i=0;i<10;i++) {
		lcdDrawFillCircle(dev, _xpos, ypos, fontWidth/2, RED);
		_xpos = _xpos - fontWidth - 5;
	}

	int16_t xp = INT16_MIN;
	int16_t yp = INT16_MAX;
	int counter = 0;

	// Clear XPT2046
	int _xp;
	int _yp;
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) break;
	} // end while

	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) continue;
		ESP_LOGI(__FUNCTION__, "counter=%d _xp=%d _yp=%d xp=%d yp=%d", counter, _xp, _yp, xp, yp);
		if (_xp > xp) xp = _xp;
		if (_yp < yp) yp = _yp;
		counter++;
		if (counter == 100) break;
		if ((counter % 10) == 0) {
			lcdDrawFillCircle(dev, xpos, ypos, fontWidth/2, GREEN);
			xpos = xpos - fontWidth - 5;
		}
	} // end while
	ESP_LOGI(__FUNCTION__, "_min_xp=%d _min_yp=%d", xp, yp);
	dev->_min_xp = xp;
	dev->_min_yp = yp;

	// Clear XPT2046
	lcdFillScreen(dev, BLACK);
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) break;
	} // end while

	lcdFillScreen(dev, BLACK);
	dev->_max_xc = width-10;
	dev->_max_yc = height-10;
	//lcdDrawFillCircle(dev, width-10, height-10, 10, CYAN);
	lcdDrawFillCircle(dev, dev->_max_xc, dev->_max_yc, 10, CYAN);
	ypos = ((height - fontHeight) / 2) - 1;
	xpos = (width + (strlen((char *)ascii) * fontWidth)) / 2;
	lcdDrawString(dev, fx, xpos, ypos, ascii, WHITE);
	ypos = ypos - fontHeight;
	_xpos = xpos;
	for(int i=0;i<10;i++) {
		lcdDrawFillCircle(dev, _xpos, ypos, fontWidth/2, RED);
		_xpos = _xpos - fontWidth - 5;
	}

	xp = INT16_MAX;
	yp = INT16_MIN;
	counter = 0;
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) continue;
		ESP_LOGI(__FUNCTION__, "counter=%d _xp=%d _yp=%d xp=%d yp=%d", counter, _xp, _yp, xp, yp);
		if (_xp < xp) xp = _xp;
		if (_yp > yp) yp = _yp;
		counter++;
		if (counter == 100) break;
		if ((counter % 10) == 0) {
			lcdDrawFillCircle(dev, xpos, ypos, fontWidth/2, GREEN);
			xpos = xpos - fontWidth - 5;
		}
	} // end while
	ESP_LOGI(__FUNCTION__, "max_xp=%d max_yp=%d", xp, yp);
	dev->_max_xp = xp;
	dev->_max_yp = yp;

	// Clear XPT2046
	lcdFillScreen(dev, BLACK);
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) break;
	} // end while
	dev->_calibration = false;

#if CONFIG_SAVE_CALIBRATION
	// Open NVS
	ESP_LOGI(__FUNCTION__, "Opening Non-Volatile Storage (NVS) handle... ");
	nvs_handle_t my_handle;
	esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) opening NVS handle!", esp_err_to_name(err));
		return;
	}
	ESP_LOGI(__FUNCTION__, "Opening Non-Volatile Storage (NVS) handle done");

	// Write Touch Calibration to NVS
	err = nvs_set_i16(my_handle, "touch_min_xc", dev->_min_xc);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_min_yc", dev->_min_yc);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_max_xc", dev->_max_xc);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_max_yc", dev->_max_yc);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_min_xp", dev->_min_xp);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_min_yp", dev->_min_yp);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_max_xp", dev->_max_xp);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "touch_max_yp", dev->_max_yp);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_set_i16(my_handle, "calibration", 1);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) writing NVS handle!", esp_err_to_name(err));
		nvs_close(my_handle);
		return;
	}
	err = nvs_commit(my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(__FUNCTION__, "Error (%s) commit NVS handle!", esp_err_to_name(err));
	}


	// Close NVS
	nvs_close(my_handle);
	ESP_LOGI(__FUNCTION__, "Write Touch Calibration done");
#endif

}

// Convert from touch position to touch coordinates
// xp:touch position x
// yp:touch position y
// xpos:touch coordinates x
// ypos:touch coordinates y
esp_err_t ConvertCoordinate(TFT_t * dev, int xp, int yp, int *xpos, int *ypos) {
	float _xd = dev->_max_xp - dev->_min_xp;
	float _yd = dev->_max_yp - dev->_min_yp;
	float _xs = dev->_max_xc - dev->_min_xc;
	float _ys = dev->_max_yc - dev->_min_yc;
	ESP_LOGD(__FUNCTION__, "_xs=%f _ys=%f", _xs, _ys);

	// Determine if within range
	if (dev->_max_xp > dev->_min_xp) {
		if (xp < dev->_min_xp && xp > dev->_max_xp) return ESP_FAIL;
	} else {
		if (xp < dev->_max_xp && xp > dev->_min_xp) return ESP_FAIL;
	}
	if (dev->_max_yp > dev->_min_yp) {
		if (yp < dev->_min_yp && yp > dev->_max_yp) return ESP_FAIL;
	} else {
		if (yp < dev->_max_yp && yp > dev->_min_yp) return ESP_FAIL;
	}

	// Convert from position to coordinate
	//_xpos = ( (float)(_xp - dev->_min_xp) / _xd * _xs ) + 10;
	//_ypos = ( (float)(_yp - dev->_min_yp) / _yd * _ys ) + 10;
	*xpos = ( (float)(xp - dev->_min_xp) / _xd * _xs ) + dev->_min_xc;
	*ypos = ( (float)(yp - dev->_min_yp) / _yd * _ys ) + dev->_min_yc;
	ESP_LOGD(__FUNCTION__, "*xpos=%d *ypos=%d", *xpos, *ypos);
	return ESP_OK;
}


void TouchPenTest(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint8_t ascii[24];
	int xpos = 0;
	int ypos = 0;

	lcdFillScreen(dev, BLACK);
	strcpy((char *)ascii, "Touch");
	ypos = ((height - fontHeight) / 2) - 1;
	xpos = (width + (strlen((char *)ascii) * fontWidth)) / 2;
	lcdSetFontDirection(dev, DIRECTION180);
	lcdDrawString(dev, fx, xpos, ypos, ascii, WHITE);

	sprintf((char *)ascii, "ACCURACY=%d", CONFIG_XPT_ACCURACY);
	ypos = ypos - fontHeight;
	xpos = (width + (strlen((char *)ascii) * fontWidth)) / 2;
	lcdDrawString(dev, fx, xpos, ypos, ascii, WHITE);

	// Clear XPT2046
	int _xp;
	int _yp;
	while(1) {
		if (touch_getxy(dev, &_xp, &_yp) == false) break;
	} // end while

	TickType_t lastTouched = xTaskGetTickCount();
	int _xpos = 0;
	int _ypos = 0;
	int _xpos_prev = 0;
	int _ypos_prev = 0;

	while(1){
		int _xp;
		int _yp;
		if (touch_getxy(dev, &_xp, &_yp)) {
			esp_err_t ret = ConvertCoordinate(dev, _xp, _yp, &_xpos, &_ypos);
			if (ret != ESP_OK) continue;
			lastTouched = xTaskGetTickCount();
			ESP_LOGI(__FUNCTION__, "_xpos=%d _xpos_prev=%d", _xpos, _xpos_prev);
			ESP_LOGI(__FUNCTION__, "_ypos=%d _ypos_prev=%d", _ypos, _ypos_prev);
			// Ignore if the new coordinates are close to the previous coordinates
			if ( (_xpos > _xpos_prev-CONFIG_XPT_ACCURACY && _xpos < _xpos_prev+CONFIG_XPT_ACCURACY) && 
				 (_ypos > _ypos_prev-CONFIG_XPT_ACCURACY && _ypos < _ypos_prev+CONFIG_XPT_ACCURACY) ) {
				ESP_LOGI(__FUNCTION__, "_xpos=%d _ypos=%d", _xpos, _ypos);
				lcdDrawFillCircle(dev, _xpos-1, _ypos-1, 3, CYAN);
			}

			_xpos_prev = _xpos;
			_ypos_prev = _ypos;

		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) break;
		} // end if
	} // end while
}

typedef struct {
	int x_center;
	int y_center;
	int x_low;
	int y_low;
	int x_high;
	int y_high;
	int radius;
	char text[32];
} AREA_t;

void ShowAllPngImage(TFT_t * dev, char * path, int max, FontxFile *fx, int width, int height, AREA_t *area) {
	lcdSetFontDirection(dev, DIRECTION0);
	lcdFillScreen(dev, WHITE);

	char file[64];
	int x_center = (width / 2) + 60;
	int y_center = 40 + ((height - 320) / 2);
	int fileNum = 0;

	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
		strcpy(file, "/icons/");
		strcat(file, pe->d_name);
		ShowPngImage(dev, file, width, height, x_center, y_center);

		area[fileNum].x_center = x_center;
		area[fileNum].y_center = y_center;
		strcpy(area[fileNum].text, pe->d_name);

		fileNum++;
		if (fileNum == max) break;
		y_center = y_center + 80;
		if (fileNum == 4) {
			x_center = (width / 2) - 60;
			y_center = 40 + ((height - 320) / 2);;
		}
	}
	closedir(dir);

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint8_t ascii[24];
	strcpy((char *)ascii, "Touch Icon");
	int xpos = ((width - fontHeight) / 2) - 1;
	int ypos = (height - strlen((char *)ascii) * fontWidth) / 2;
	lcdSetFontDirection(dev, DIRECTION90);
	lcdDrawString(dev, fx, xpos, ypos, ascii, BLACK);

	return;
}

void TouchIconTest(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	AREA_t area[8];
	// Show all small image
	ShowAllPngImage(dev, "/icons/", 8, fx, width, height, area);
	TickType_t lastTouched = xTaskGetTickCount();

	while(1) {
		int _xp = 0;
		int _yp = 0;
		if (touch_getxy(dev, &_xp, &_yp)) { 
			int _xpos = 0;
			int _ypos = 0;
			esp_err_t ret = ConvertCoordinate(dev, _xp, _yp, &_xpos, &_ypos);
			if (ret != ESP_OK) continue;
			ESP_LOGD(__FUNCTION__, "_xpos=%d _ypos=%d", _xpos, _ypos);
			lastTouched = xTaskGetTickCount();

			for (int index=0;index<8;index++) {
				// Calculate distance from center
				double _xd = _xpos - area[index].x_center;
				double _yd = _ypos - area[index].y_center;
				double _radius = sqrt((_xd*_xd) + (_yd*_yd));
				ESP_LOGI(__FUNCTION__, "area.x_center=%d area.y_center=%d _radius=%f", area[0].x_center, area[0].y_center, _radius);

				// Distance is in range
				if (_radius < 30.0) {
					// Show large image
					ESP_LOGI(__FUNCTION__, "area.text=[%s]", area[0].text);
					char file[64];
					strcpy(file, "/images/");
					strcat(file, area[index].text);
					ESP_LOGI(__FUNCTION__, "file=[%s]", file);
					PNGTest(dev, file, CONFIG_WIDTH, CONFIG_HEIGHT);
					vTaskDelay(500);

					// Show all small image
					ShowAllPngImage(dev, "/icons/", 8, fx, width, height, area);
					lastTouched = xTaskGetTickCount();
				}
			}
		
		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) break;
		} // end if

	} // end while
}

void ShowSoftKeyboard(TFT_t * dev, int page, char * input, FontxFile *fx, int width, int height, AREA_t *area) {
	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	int x_center = (width / 2) + 60;
	int y_center = 40 + ((height - 320) / 2);
	int offset = 0;
	uint8_t ascii[64];
	memset((char*)ascii, 0, sizeof(ascii));

	// Draw new page
	if (page >= 0) {
		lcdSetFontDirection(dev, DIRECTION90);
		lcdFillScreen(dev, BLACK);

		for (int i=0;i<10; i++) {
			int text = 0x30 + i;
			if (page == 1) text = text + 17;
			if (page == 2) text = text + 27;
			if (page == 3) text = text + 37;
			area[offset+i].x_center = x_center;
			area[offset+i].y_center = y_center;
			area[offset+i].radius = 25;
			area[offset+i].text[0] = text;
			area[offset+i].text[1] = 0;
			ascii[0] = text;
			//lcdDrawCircle(dev, area[offset+i].x_center, area[offset+i].y_center, area[offset+i].radius, RED);
			lcdDrawFillRect2(dev, area[offset+i].x_center-5, area[offset+i].y_center+5, area[offset+i].radius, CYAN);
			lcdDrawFillRect2(dev, area[offset+i].x_center, area[offset+i].y_center, area[offset+i].radius, BLUE);

			int xpos = x_center - (fontHeight / 2) - 2;
			int ypos = y_center - (fontWidth / 2);
			ESP_LOGD(__FUNCTION__, "i=%d xpos=%d ypos=%d", i, xpos, ypos);
			lcdDrawString(dev, fx, xpos, ypos, ascii, BLACK);

			y_center = y_center + 60;
			if (i == 4) {
				x_center = (width / 2) - 0;
				y_center = 40 + ((height - 320) / 2);;
			}
		} // end for

		strcpy((char *)ascii, "Touch Button");
		int xpos = (width - fontHeight);
		int ypos = (height - strlen((char *)ascii) * fontWidth) / 2;
		lcdSetFontDirection(dev, DIRECTION90);
		lcdDrawString(dev, fx, xpos, ypos, ascii, BLUE);

	} // end if

	
	// Draw entered text
	if (strlen(input) == 0) return;
	x_center = (width / 2) - 80;
	y_center = 20 + ((height - 320) / 2);
	strcpy((char *)ascii, input);
	lcdSetFontUnderLine(dev, CYAN);
	lcdDrawString(dev, fx, x_center, y_center, ascii, CYAN);
	lcdUnsetFontUnderLine(dev);

	return;
}

void TouchKeyTest(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	AREA_t area[10];
	char input[32];
	memset(input, 0, sizeof(input));
	int page = 0;
	TickType_t lastTouched = xTaskGetTickCount();

	ShowSoftKeyboard(dev, page, input, fx, width, height, area);
	while(1) {
		int _xp = 0;
		int _yp = 0;
		if (touch_getxy(dev, &_xp, &_yp)) {
			TickType_t diffTick = xTaskGetTickCount() - lastTouched;
			ESP_LOGD(__FUNCTION__, "diffTick=%"PRIu32,diffTick);
			if (diffTick < 20) continue;
			
			int _xpos = 0;
			int _ypos = 0;
			esp_err_t ret = ConvertCoordinate(dev, _xp, _yp, &_xpos, &_ypos);
			if (ret != ESP_OK) continue;
			ESP_LOGD(__FUNCTION__, "_xpos=%d _ypos=%d", _xpos, _ypos);
			lastTouched = xTaskGetTickCount();

			bool isMatch = false;
			for (int index=0;index<10;index++) {
				// Calculate distance from center
				double _xd = _xpos - area[index].x_center;
				double _yd = _ypos - area[index].y_center;
				double _radius = sqrt((_xd*_xd) + (_yd*_yd));
				ESP_LOGD(__FUNCTION__, "area.x_center=%d area.y_center=%d _radius=%f", area[0].x_center, area[0].y_center, _radius);

				// Distance is in range
				if (_radius < area[index].radius) {
					ESP_LOGI(__FUNCTION__, "area[%d].text=[%s]", index, area[index].text);
					isMatch = true;
					if (strlen(input) < 15) {
						// Erase shadow and button
						lcdDrawFillRect2(dev, area[index].x_center-5, area[index].y_center+5, area[index].radius, BLACK);
						lcdDrawFillRect2(dev, area[index].x_center, area[index].y_center, area[index].radius, BLUE);
						vTaskDelay(10);

						// Draw shadow and button
						lcdDrawFillRect2(dev, area[index].x_center-5, area[index].y_center+5, area[index].radius, CYAN);
						lcdDrawFillRect2(dev, area[index].x_center, area[index].y_center, area[index].radius, BLUE);

						// Draw character
						int xpos = area[index].x_center - (fontHeight / 2) - 2;
						int ypos = area[index].y_center - (fontWidth / 2);
						uint8_t ascii[2];
						ascii[0] = area[index].text[0];
						ascii[1] = 0;
						lcdDrawString(dev, fx, xpos, ypos, ascii, BLACK);

						// Add entered characters
						strcat(input, area[index].text);
					}
					ESP_LOGI(__FUNCTION__, "input=[%s]", input);
				}
			}
			if (isMatch) {
				ShowSoftKeyboard(dev, -1, input, fx, width, height, area);
			} else {
				page++;
				if (page == 4) page = 0;
				ShowSoftKeyboard(dev, page, input, fx, width, height, area);
			} 

		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) break;
		} // end if

	} // end while
}


void TouchMoveTest(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	lcdFillScreen(dev, BLACK);
	int x_center = (width / 2) - 1;
	int y_center = (height / 2) -1;
	ESP_LOGI(__FUNCTION__, "x_center=%d y_center=%d", x_center, y_center);
	//lcdDrawFillCircle(dev, x_center, y_center, 20, RED);
	//lcdDrawRect2(dev, x_center, y_center, 20, RED);
	lcdDrawFillRect2(dev, x_center, y_center, 20, RED);

	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);

	uint8_t ascii[24];
	strcpy((char *)ascii, "Touch Screen");
	int x_text = ((width - fontHeight) / 2) - 1;
	int y_text = (height - strlen((char *)ascii) * fontWidth) / 2;
	lcdSetFontDirection(dev, DIRECTION90);
	lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);

	int x_prev = x_center;
	int y_prev = y_center;
	TickType_t lastTouched = xTaskGetTickCount();
	while(1) {
		int _xp = 0;
		int _yp = 0;
		if (touch_getxy(dev, &_xp, &_yp)) {
			TickType_t diffTick = xTaskGetTickCount() - lastTouched;
			ESP_LOGD(__FUNCTION__, "diffTick=%"PRIu32,diffTick);
			if (diffTick < 20) continue;

			int x_new = 0;
			int y_new = 0;
			esp_err_t ret = ConvertCoordinate(dev, _xp, _yp, &x_new, &y_new);
			if (ret != ESP_OK) continue;
			ESP_LOGI(__FUNCTION__, "x_prev=%d y_prev=%d", x_prev, y_prev);
			ESP_LOGI(__FUNCTION__, "x_new=%d y_new=%d", x_new, y_new);
			lastTouched = xTaskGetTickCount();

			// Find the distance between two points
			int distance = (int)sqrt((x_new - x_prev) * (x_new - x_prev) + (y_new - y_prev) * (y_new - y_prev));
			ESP_LOGD(__FUNCTION__, "distance=%d",distance);

			// Find the angle between two points
			double radian = atan2(y_new - y_prev, x_new - x_prev);
			ESP_LOGD(__FUNCTION__, "radian=%f",radian);

			int x_move = x_prev;
			int y_move = y_prev;
			for (int _distance=10; _distance<distance;_distance=_distance+10) {
				lcdDrawFillRect2(dev, x_move, y_move, 20, BLACK);
				x_move = cos(radian) * _distance + x_prev;
				y_move = sin(radian) * _distance + y_prev;
				lcdDrawFillRect2(dev, x_move, y_move, 20, RED);
			}
			lcdDrawFillRect2(dev, x_move, y_move, 20, BLACK);
			lcdDrawFillRect2(dev, x_new, y_new, 20, RED);
			x_prev = x_new;;
			y_prev = y_new;;
			lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);

		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) break;
		} // end if

	} // end while
}

void TouchMakeMenu(TFT_t * dev, int page, FontxFile *fx, int width, int height, AREA_t *area) {
	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	ESP_LOGD(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);


	uint8_t ascii[24];
	int x_center = width - (fontHeight * 2);
	lcdSetFontDirection(dev, DIRECTION90);
	lcdFillScreen(dev, BLACK);

	int y_center = height / 8 * 1;
	//lcdDrawCircle(dev, x_center, y_center, 20, RED);
	strcpy((char *)ascii, "Menu1");
	int x_text = x_center;
	int y_text = y_center - (strlen((char *)ascii) * fontWidth / 2);
	int x_rect = x_text + fontHeight;
	int y_rect = y_text + (strlen((char *)ascii) * fontWidth);
	area[0].x_low = x_text;
	area[0].x_high = x_rect;
	area[0].y_low = y_text;
	area[0].y_high = y_rect;
	ESP_LOGD(__FUNCTION__, "x_low=%d x_high=%d y_low=%d y_high=%d", area[0].x_low, area[0].x_high, area[0].y_low, area[0].y_high);
	if (page == 0) {
		lcdDrawFillRect(dev, x_text, y_text, x_rect, y_rect, RED);
		lcdDrawString(dev, fx, x_text, y_text, ascii, BLACK);
		strcpy((char *)ascii, "Hello World");
		x_text = (width / 2) - 1;
		y_text = (height / 2) - (strlen((char *)ascii) * fontWidth / 2);
		lcdDrawString(dev, fx, x_text, y_text, ascii, CYAN);
	} else {
		lcdDrawRect(dev, x_text, y_text, x_rect, y_rect, WHITE);
		lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);
	}

	y_center = height / 8 * 3;
	//lcdDrawCircle(dev, x_center, y_center, 20, RED);
	strcpy((char *)ascii, "Menu2");
	x_text = x_center;
	y_text = y_center - (strlen((char *)ascii) * fontWidth / 2);
	x_rect = x_text + fontHeight;
	y_rect = y_text + (strlen((char *)ascii) * fontWidth);
	area[1].x_low = x_text;
	area[1].x_high = x_rect;
	area[1].y_low = y_text;
	area[1].y_high = y_rect;
	ESP_LOGD(__FUNCTION__, "x_low=%d x_high=%d y_low=%d y_high=%d", area[0].x_low, area[0].x_high, area[0].y_low, area[0].y_high);
	if (page == 1) {
		lcdDrawFillRect(dev, x_text, y_text, x_rect, y_rect, RED);
		lcdDrawString(dev, fx, x_text, y_text, ascii, BLACK);
		strcpy((char *)ascii, "Hello America");
		x_text = (width / 2) - 1;
		y_text = (height / 2) - (strlen((char *)ascii) * fontWidth / 2);
		lcdDrawString(dev, fx, x_text, y_text, ascii, CYAN);
	} else {
		lcdDrawRect(dev, x_text, y_text, x_rect, y_rect, WHITE);
		lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);
	}

	y_center = height / 8 * 5;
	//lcdDrawCircle(dev, x_center, y_center, 20, RED);
	strcpy((char *)ascii, "Menu3");
	x_text = x_center;
	y_text = y_center - (strlen((char *)ascii) * fontWidth / 2);
	x_rect = x_text + fontHeight;
	y_rect = y_text + (strlen((char *)ascii) * fontWidth);
	area[2].x_low = x_text;
	area[2].x_high = x_rect;
	area[2].y_low = y_text;
	area[2].y_high = y_rect;
	ESP_LOGD(__FUNCTION__, "x_low=%d x_high=%d y_low=%d y_high=%d", area[0].x_low, area[0].x_high, area[0].y_low, area[0].y_high);
	if (page == 2) {
		lcdDrawFillRect(dev, x_text, y_text, x_rect, y_rect, RED);
		lcdDrawString(dev, fx, x_text, y_text, ascii, BLACK);
		strcpy((char *)ascii, "Hello Japan");
		x_text = (width / 2) - 1;
		y_text = (height / 2) - (strlen((char *)ascii) * fontWidth / 2);
		lcdDrawString(dev, fx, x_text, y_text, ascii, CYAN);
	} else {
		lcdDrawRect(dev, x_text, y_text, x_rect, y_rect, WHITE);
		lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);
	}

	y_center = height / 8 * 7;
	//lcdDrawCircle(dev, x_center, y_center, 20, RED);
	strcpy((char *)ascii, "Menu4");
	x_text = x_center;
	y_text = y_center - (strlen((char *)ascii) * fontWidth / 2);
	x_rect = x_text + fontHeight;
	y_rect = y_text + (strlen((char *)ascii) * fontWidth);
	area[3].x_low = x_text;
	area[3].x_high = x_rect;
	area[3].y_low = y_text;
	area[3].y_high = y_rect;
	if (page == 3) {
		lcdDrawFillRect(dev, x_text, y_text, x_rect, y_rect, RED);
		lcdDrawString(dev, fx, x_text, y_text, ascii, BLACK);
		strcpy((char *)ascii, "Hello Africa");
		x_text = (width / 2) - 1;
		y_text = (height / 2) - (strlen((char *)ascii) * fontWidth / 2);
		lcdDrawString(dev, fx, x_text, y_text, ascii, CYAN);
	} else {
		lcdDrawRect(dev, x_text, y_text, x_rect, y_rect, WHITE);
		lcdDrawString(dev, fx, x_text, y_text, ascii, WHITE);
	}
}

void TouchMenuTest(TFT_t * dev, FontxFile *fx, int width, int height, TickType_t timeout) {
	AREA_t area[4];
	int page = 0;
	TouchMakeMenu(dev, page, fx, width, height, area);
	TickType_t lastTouched = xTaskGetTickCount();
	while(1) {
		int _xp = 0;
		int _yp = 0;
		if (touch_getxy(dev, &_xp, &_yp)) { 
			TickType_t diffTick = xTaskGetTickCount() - lastTouched;
			ESP_LOGD(__FUNCTION__, "diffTick=%"PRIu32,diffTick);
			if (diffTick < 20) continue;

			int _xpos = 0;
			int _ypos = 0;
			esp_err_t ret = ConvertCoordinate(dev, _xp, _yp, &_xpos, &_ypos);
			if (ret != ESP_OK) continue;
			ESP_LOGD(__FUNCTION__, "_xpos=%d _ypos=%d", _xpos, _ypos);
			lastTouched = xTaskGetTickCount();

			for (int index=0;index<4;index++) {
				ESP_LOGI(__FUNCTION__, "_xpos=%d x_low=%d x_high=%d", _xpos, area[index].x_low, area[index].x_high);
				ESP_LOGI(__FUNCTION__, "_ypos=%d y_low=%d y_high=%d", _ypos, area[index].y_low, area[index].y_high);
				if (_xpos >= area[index].x_low && _xpos <= area[index].x_high) {
					if (_ypos >= area[index].y_low && _ypos <= area[index].y_high) {
						page = index;
						ESP_LOGI(__FUNCTION__, "page=%d", page);
						TouchMakeMenu(dev, page, fx, width, height, area);
					}
				}
			} // end for
		} else {
			TickType_t current = xTaskGetTickCount();
			if (current - lastTouched > timeout) break;
		} // end if

	} // end while
}
#endif // CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS

void ILI9341(void *pvParameters)
{
	// set font file
	FontxFile fx16G[2];
	FontxFile fx24G[2];
	FontxFile fx32G[2];
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic

	FontxFile fx32L[2];
	InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latinc

	FontxFile fx16M[2];
	FontxFile fx24M[2];
	FontxFile fx32M[2];
	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo
	
	TFT_t dev;
#if CONFIG_XPT2046_ENABLE_SAME_BUS
	ESP_LOGI(TAG, "Enable Touch Contoller using the same SPI bus as TFT");
	int XPT_MISO_GPIO = CONFIG_XPT_MISO_GPIO;
	int XPT_CS_GPIO = CONFIG_XPT_CS_GPIO;
	int XPT_IRQ_GPIO = CONFIG_XPT_IRQ_GPIO;
	int XPT_SCLK_GPIO = -1;
	int XPT_MOSI_GPIO = -1;
#elif CONFIG_XPT2046_ENABLE_DIFF_BUS
	ESP_LOGI(TAG, "Enable Touch Contoller using the different SPI bus from TFT");
	int XPT_MISO_GPIO = CONFIG_XPT_MISO_GPIO;
	int XPT_CS_GPIO = CONFIG_XPT_CS_GPIO;
	int XPT_IRQ_GPIO = CONFIG_XPT_IRQ_GPIO;
	int XPT_SCLK_GPIO = CONFIG_XPT_SCLK_GPIO;
	int XPT_MOSI_GPIO = CONFIG_XPT_MOSI_GPIO;
#else
	ESP_LOGI(TAG, "Disable Touch Contoller");
	int XPT_MISO_GPIO = -1;
	int XPT_CS_GPIO = -1;
	int XPT_IRQ_GPIO = -1;
	int XPT_SCLK_GPIO = -1;
	int XPT_MOSI_GPIO = -1;
#endif
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_TFT_CS_GPIO, CONFIG_DC_GPIO, 
		CONFIG_RESET_GPIO, CONFIG_BL_GPIO, XPT_MISO_GPIO, XPT_CS_GPIO, XPT_IRQ_GPIO, XPT_SCLK_GPIO, XPT_MOSI_GPIO);

#if CONFIG_ILI9225
	uint16_t model = 0x9225;
#endif
#if CONFIG_ILI9225G
	uint16_t model = 0x9226;
#endif
#if CONFIG_ILI9340
	uint16_t model = 0x9340;
#endif
#if CONFIG_ILI9341
	uint16_t model = 0x9341;
#endif
#if CONFIG_ST7735
	uint16_t model = 0x7735;
#endif
#if CONFIG_ST7796
	uint16_t model = 0x7796;
#endif
	lcdInit(&dev, model, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

#if CONFIG_INVERSION
	ESP_LOGI(TAG, "Enable Display Inversion");
	lcdInversionOn(&dev);
#endif

#if CONFIG_RGB_COLOR
	ESP_LOGI(TAG, "Change BGR filter to RGB filter");
	lcdBGRFilter(&dev);
#endif

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
#if CONFIG_XPT_CHECK
	TouchPosition(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif
#endif

#if 0
	// for test
	while(1) {
		char file[64];
		lcdFillScreen(&dev, WHITE);
		strcpy(file, "/icons/twitter.png");
		ShowPngImage(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_WIDTH/2, CONFIG_HEIGHT/2);
		WAIT;
		strcpy(file, "/icons/facebook.png");
		ShowPngImage(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_WIDTH/2, CONFIG_HEIGHT/2);
		WAIT;

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
		TouchCalibration(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		TouchPenTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchKeyTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMenuTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMoveTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchIconTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif

		ArrowTest(&dev, fx16G, model, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;
	}
#endif

	while(1) {
		char file[32];

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
		TouchCalibration(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		TouchPenTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchKeyTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMenuTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
		TouchMoveTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);

#ifdef ENABLE_PNG
		TouchIconTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT, 1000);
#endif

#endif

#if 0
		while(1) {
			ArrowTest(&dev, fx16G, model, CONFIG_WIDTH, CONFIG_HEIGHT);
			WAIT;
		}
#endif

		FillTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ColorBarTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ArrowTest(&dev, fx16G, model, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		LineTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		CircleTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		RoundRectTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		RectAngleTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		TriangleTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			DirectionTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			DirectionTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			HorizontalTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			HorizontalTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			VerticalTest(&dev, fx24G, CONFIG_WIDTH, CONFIG_HEIGHT);
		} else {
			VerticalTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		}
		WAIT;

		FillRectTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		ColorTest(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		CodeTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 0x00, 0x7f);
		WAIT;

		CodeTest(&dev, fx32G, CONFIG_WIDTH, CONFIG_HEIGHT, 0x80, 0xff);
		WAIT;

		CodeTest(&dev, fx32L, CONFIG_WIDTH, CONFIG_HEIGHT, 0x80, 0xff);
		WAIT;

		if (CONFIG_WIDTH >= CONFIG_HEIGHT) {
			strcpy(file, "/images/esp32.bmp");
		} else {
			strcpy(file, "/images/esp32_ro.bmp");
		}
		BMPTest(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		strcpy(file, "/images/esp32.jpeg");
		JPEGTest(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		strcpy(file, "/images/esp_logo.png");
		PNGTest(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;

		lcdFillScreen(&dev, WHITE);
		strcpy(file, "/icons/twitter.png");
		ShowPngImage(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_WIDTH/2, CONFIG_HEIGHT/2);
		WAIT;
		strcpy(file, "/icons/facebook.png");
		ShowPngImage(&dev, file, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_WIDTH/2, CONFIG_HEIGHT/2);
		WAIT;

		ScrollTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		WAIT;
		ScrollReset(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);

		// Multi Font Test
		uint16_t color;
		uint8_t ascii[40];
		uint16_t margin = 10;
		lcdFillScreen(&dev, BLACK);
		color = WHITE;
		lcdSetFontDirection(&dev, DIRECTION0);
		uint16_t xpos = 0;
		uint16_t ypos = 15;
		int xd = 0;
		int yd = 1;
		if(CONFIG_WIDTH < CONFIG_HEIGHT) {
			lcdSetFontDirection(&dev, DIRECTION90);
			xpos = (CONFIG_WIDTH-1)-16;
			ypos = 0;
			xd = 1;
			yd = 0;
		}
		strcpy((char *)ascii, "16Dot Gothic Font");
		lcdDrawString(&dev, fx16G, xpos, ypos, ascii, color);

		xpos = xpos - (24 * xd) - (margin * xd);
		ypos = ypos + (16 * yd) + (margin * yd);
		strcpy((char *)ascii, "24Dot Gothic Font");
		lcdDrawString(&dev, fx24G, xpos, ypos, ascii, color);

		xpos = xpos - (32 * xd) - (margin * xd);
		ypos = ypos + (24 * yd) + (margin * yd);
		if (CONFIG_WIDTH >= 240) {
			strcpy((char *)ascii, "32Dot Gothic Font");
			lcdDrawString(&dev, fx32G, xpos, ypos, ascii, color);
			xpos = xpos - (32 * xd) - (margin * xd);
			ypos = ypos + (32 * yd) + (margin * yd);
		}

		xpos = xpos - (10 * xd) - (margin * xd);
		ypos = ypos + (10 * yd) + (margin * yd);
		strcpy((char *)ascii, "16Dot Mincyo Font");
		lcdDrawString(&dev, fx16M, xpos, ypos, ascii, color);

		xpos = xpos - (24 * xd) - (margin * xd);
		ypos = ypos + (16 * yd) + (margin * yd);
		strcpy((char *)ascii, "24Dot Mincyo Font");
		lcdDrawString(&dev, fx24M, xpos, ypos, ascii, color);

		if (CONFIG_WIDTH >= 240) {
			xpos = xpos - (32 * xd) - (margin * xd);
			ypos = ypos + (24 * yd) + (margin * yd);
			strcpy((char *)ascii, "32Dot Mincyo Font");
			lcdDrawString(&dev, fx32M, xpos, ypos, ascii, color);
		}
		lcdSetFontDirection(&dev, DIRECTION0);
		WAIT;

	} // end while

	// never reach here
	vTaskDelete(NULL);
}

static void listSPIFFS(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

esp_err_t mountSPIFFS(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret ==ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

#if 0
	ESP_LOGI(TAG, "Performing SPIFFS_check().");
	ret = esp_spiffs_check(conf.partition_label);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		return ret;
	} else {
			ESP_LOGI(TAG, "SPIFFS_check() successful");
	}
#endif

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Mount %s to %s success", path, label);
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	return ret;
}


void app_main(void)
{
	// Initialize NVS
	ESP_LOGI(TAG, "Initialize NVS");
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	ESP_LOGI(TAG, "Initializing SPIFFS");
	esp_err_t ret;
	ret = mountSPIFFS("/spiffs", "storage0", 10);
	if (ret != ESP_OK) return;
	listSPIFFS("/spiffs/");

	// Image file borrowed from here
	// https://www.flaticon.com/packs/social-media-343
	ret = mountSPIFFS("/icons", "storage1", 10);
	if (ret != ESP_OK) return;
	listSPIFFS("/icons/");

	ret = mountSPIFFS("/images", "storage2", 14);
	if (ret != ESP_OK) return;
	listSPIFFS("/images/");

	xTaskCreate(ILI9341, "ILI9341", 1024*6, NULL, 2, NULL);
}
