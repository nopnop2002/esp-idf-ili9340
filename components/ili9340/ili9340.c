#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "ili9340.h"

#define TAG "ILI9340"
#define	_DEBUG_ 0

#if CONFIG_SPI2_HOST
#define TFT_ID SPI2_HOST
#elif CONFIG_SPI3_HOST
#define TFT_ID SPI3_HOST
#else
#define TFT_ID SPI2_HOST // When not to use menuconfig
#define XPT_ID SPI3_HOST // When not to use menuconfig
#endif

#define SPI_DEFAULT_FREQUENCY SPI_MASTER_FREQ_10M; // 10MHz

//static const int TFT_MOSI = 23;
//static const int TFT_SCLK = 18;

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
//static const int SPI_Frequency = SPI_MASTER_FREQ_10M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_20M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_26M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_40M;

int clock_speed_hz = SPI_DEFAULT_FREQUENCY;

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
static const int XPT_Frequency = 1*1000*1000;
//static const int XPT_Frequency = 2*1000*1000;
//static const int XPT_Frequency = 4*1000*1000;

//#define XPT_MISO 19
//#define XPT_CS	4
//#define XPT_IRQ 5
#endif

void spi_clock_speed(int speed) {
    ESP_LOGI(TAG, "SPI clock speed=%d MHz", speed/1000000);
    clock_speed_hz = speed;
}

void spi_master_init(TFT_t * dev, int16_t TFT_MOSI, int16_t TFT_SCLK, int16_t TFT_CS, int16_t GPIO_DC, int16_t GPIO_RESET, int16_t GPIO_BL,
	int16_t XPT_MISO, int16_t XPT_CS, int16_t XPT_IRQ, int16_t XPT_SCLK, int16_t XPT_MOSI)
{
	esp_err_t ret;

	ESP_LOGI(TAG, "TFT_MOSI=%d",TFT_MOSI);
	ESP_LOGI(TAG, "TFT_SCLK=%d",TFT_SCLK);
	ESP_LOGI(TAG, "TFT_CS=%d",TFT_CS);
	gpio_reset_pin( TFT_CS );
	gpio_set_direction( TFT_CS, GPIO_MODE_OUTPUT );
	//gpio_set_level( TFT_CS, 0 );
	gpio_set_level( TFT_CS, 1 );

	ESP_LOGI(TAG, "GPIO_DC=%d",GPIO_DC);
	gpio_reset_pin( GPIO_DC );
	gpio_set_direction( GPIO_DC, GPIO_MODE_OUTPUT );
	gpio_set_level( GPIO_DC, 0 );

	ESP_LOGI(TAG, "GPIO_RESET=%d",GPIO_RESET);
	if ( GPIO_RESET >= 0 ) {
		gpio_reset_pin( GPIO_RESET );
		gpio_set_direction( GPIO_RESET, GPIO_MODE_OUTPUT );
		gpio_set_level( GPIO_RESET, 0 );
		vTaskDelay( pdMS_TO_TICKS( 100 ) );
		gpio_set_level( GPIO_RESET, 1 );
	}

	ESP_LOGI(TAG, "GPIO_BL=%d",GPIO_BL);
	if ( GPIO_BL >= 0 ) {
		gpio_reset_pin( GPIO_BL );
		gpio_set_direction( GPIO_BL, GPIO_MODE_OUTPUT );
		gpio_set_level( GPIO_BL, 0 );
	}

#if CONFIG_XPT2046_ENABLE_SAME_BUS
	spi_bus_config_t tft_buscfg = {
		.sclk_io_num = TFT_SCLK,
		.mosi_io_num = TFT_MOSI,
		.miso_io_num = XPT_MISO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
#else
	spi_bus_config_t tft_buscfg = {
		.sclk_io_num = TFT_SCLK,
		.mosi_io_num = TFT_MOSI,
		.miso_io_num = -1,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
#endif

	ret = spi_bus_initialize( TFT_ID, &tft_buscfg, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize(TFT) ret=%d TFT_ID=%d",ret, TFT_ID);
	assert(ret==ESP_OK);

	spi_device_interface_config_t tft_devcfg={
		//.clock_speed_hz = SPI_Frequency,
		.clock_speed_hz = clock_speed_hz,
		.spics_io_num = TFT_CS,
		.queue_size = 7,
		.flags = SPI_DEVICE_NO_DUMMY,
	};

	spi_device_handle_t tft_handle;
	ret = spi_bus_add_device( TFT_ID, &tft_devcfg, &tft_handle);
	ESP_LOGD(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
	dev->_dc = GPIO_DC;
	dev->_bl = GPIO_BL;
	dev->_TFT_Handle = tft_handle;

#if CONFIG_XPT2046_ENABLE_DIFF_BUS
	ESP_LOGI(TAG, "XPT_SCLK=%d",XPT_SCLK);
	ESP_LOGI(TAG, "XPT_MOSI=%d",XPT_MOSI);
	ESP_LOGI(TAG, "XPT_MISO=%d",XPT_MISO);
	spi_bus_config_t xpt_buscfg = {
		.sclk_io_num = XPT_SCLK,
		.mosi_io_num = XPT_MOSI,
		.miso_io_num = XPT_MISO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	ret = spi_bus_initialize( XPT_ID, &xpt_buscfg, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize(XPT) ret=%d XPT_ID=%d",ret, XPT_ID);
	assert(ret==ESP_OK);
#endif

#if CONFIG_XPT2046_ENABLE_SAME_BUS || CONFIG_XPT2046_ENABLE_DIFF_BUS
	ESP_LOGI(TAG, "XPT_CS=%d",XPT_CS);
	gpio_reset_pin( XPT_CS );
	gpio_set_direction( XPT_CS, GPIO_MODE_OUTPUT );
	gpio_set_level( XPT_CS, 1 );

	// set the IRQ as a input
	ESP_LOGI(TAG, "XPT_IRQ=%d",XPT_IRQ);
	gpio_config_t io_conf = {};
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.pin_bit_mask = (1ULL<<XPT_IRQ);
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);
	//gpio_reset_pin( XPT_IRQ );
	//gpio_set_direction( XPT_IRQ, GPIO_MODE_DEF_INPUT );

	spi_device_interface_config_t xpt_devcfg={
		.clock_speed_hz = XPT_Frequency,
		.spics_io_num = XPT_CS,
		.queue_size = 7,
		.flags = SPI_DEVICE_NO_DUMMY,
	};

	spi_device_handle_t xpt_handle;
#if CONFIG_XPT2046_ENABLE_SAME_BUS
	ret = spi_bus_add_device( TFT_ID, &xpt_devcfg, &xpt_handle);
#else
	ret = spi_bus_add_device( XPT_ID, &xpt_devcfg, &xpt_handle);
#endif
	ESP_LOGD(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
	dev->_XPT_Handle = xpt_handle;
	dev->_irq = XPT_IRQ;
	dev->_calibration = true;
#endif
}


bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength)
{
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Data;
#if 1
		ret = spi_device_transmit( SPIHandle, &SPITransaction );
#endif
#if 0
		ret = spi_device_polling_transmit( SPIHandle, &SPITransaction );
#endif
		assert(ret==ESP_OK); 
	}

	return true;
}

bool spi_master_write_comm_byte(TFT_t * dev, uint8_t cmd)
{
	static uint8_t Byte = 0;
	Byte = cmd;
	gpio_set_level( dev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, &Byte, 1 );
}

bool spi_master_write_comm_word(TFT_t * dev, uint16_t cmd)
{
	static uint8_t Byte[2];
	Byte[0] = (cmd >> 8) & 0xFF;
	Byte[1] = cmd & 0xFF;
	gpio_set_level( dev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 2 );
}


bool spi_master_write_data_byte(TFT_t * dev, uint8_t data)
{
	static uint8_t Byte = 0;
	Byte = data;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, &Byte, 1 );
}


bool spi_master_write_data_word(TFT_t * dev, uint16_t data)
{
	static uint8_t Byte[2];
	Byte[0] = (data >> 8) & 0xFF;
	Byte[1] = data & 0xFF;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 2);
}

bool spi_master_write_addr(TFT_t * dev, uint16_t addr1, uint16_t addr2)
{
	static uint8_t Byte[4];
	Byte[0] = (addr1 >> 8) & 0xFF;
	Byte[1] = addr1 & 0xFF;
	Byte[2] = (addr2 >> 8) & 0xFF;
	Byte[3] = addr2 & 0xFF;
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, 4);
}

bool spi_master_write_color(TFT_t * dev, uint16_t color, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (color >> 8) & 0xFF;
		Byte[index++] = color & 0xFF;
	}
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, size*2);
}

// Add 202001
bool spi_master_write_colors(TFT_t * dev, uint16_t * colors, uint16_t size)
{
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (colors[i] >> 8) & 0xFF;
		Byte[index++] = colors[i] & 0xFF;
	}
	gpio_set_level( dev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( dev->_TFT_Handle, Byte, size*2);
}


void delayMS(int ms) {
	int _ms = ms + (portTICK_PERIOD_MS - 1);
	TickType_t xTicksToDelay = _ms / portTICK_PERIOD_MS;
	ESP_LOGD(TAG, "ms=%d _ms=%d portTICK_PERIOD_MS=%"PRIu32" xTicksToDelay=%"PRIu32,ms,_ms,portTICK_PERIOD_MS,xTicksToDelay);
	vTaskDelay(xTicksToDelay);
}


void lcdWriteRegisterWord(TFT_t * dev, uint16_t addr, uint16_t data)
{
	spi_master_write_comm_word(dev, addr);
	spi_master_write_data_word(dev, data);
}


void lcdWriteRegisterByte(TFT_t * dev, uint8_t addr, uint16_t data)
{
	spi_master_write_comm_byte(dev, addr);
	spi_master_write_data_word(dev, data);
}



void lcdInit(TFT_t * dev, uint16_t model, int width, int height, int offsetx, int offsety)
{
	dev->_model = model;
	dev->_width = width;
	dev->_height = height;
	dev->_offsetx = offsetx;
	dev->_offsety = offsety;
	dev->_font_direction = DIRECTION0;
	dev->_font_fill = false;
	dev->_font_underline = false;

	if (dev->_model == 0x7796) {
		ESP_LOGI(TAG,"Your TFT is ST7796");
		ESP_LOGI(TAG,"Screen width:%d",width);
		ESP_LOGI(TAG,"Screen height:%d",height);
		spi_master_write_comm_byte(dev, 0xC0);	//Power Control 1
		spi_master_write_data_byte(dev, 0x10);
		spi_master_write_data_byte(dev, 0x10);

		spi_master_write_comm_byte(dev, 0xC1);	//Power Control 2
		spi_master_write_data_byte(dev, 0x41);
	
		spi_master_write_comm_byte(dev, 0xC5);	//VCOM Control 1
		spi_master_write_data_byte(dev, 0x00);
		spi_master_write_data_byte(dev, 0x22);
		spi_master_write_data_byte(dev, 0x80);
		spi_master_write_data_byte(dev, 0x40);

		spi_master_write_comm_byte(dev, 0x36);	//Memory Access Control
		spi_master_write_data_byte(dev, 0x48);	//Right top start, BGR color filter panel
		//spi_master_write_data_byte(dev, 0x68);	//Right top start, BGR color filter panel

		spi_master_write_comm_byte(dev, 0xB0);	//Interface Mode Control
		spi_master_write_data_byte(dev, 0x00);

		spi_master_write_comm_byte(dev, 0xB1);	//Frame Rate Control
		spi_master_write_data_byte(dev, 0xB0);
		spi_master_write_data_byte(dev, 0x11);

		spi_master_write_comm_byte(dev, 0xB4);	//Display Inversion Control
		spi_master_write_data_byte(dev, 0x02);

		spi_master_write_comm_byte(dev, 0xB6);	//Display Function Control
		spi_master_write_data_byte(dev, 0x02);
		spi_master_write_data_byte(dev, 0x02);
		spi_master_write_data_byte(dev, 0x3B);

		spi_master_write_comm_byte(dev, 0xB7);	//Entry Mode Set
		spi_master_write_data_byte(dev, 0xC6);

		spi_master_write_comm_byte(dev, 0x3A);	//Interface Pixel Format
		spi_master_write_data_byte(dev, 0x55);

		spi_master_write_comm_byte(dev, 0xF7);	//Adjust Control 3
		spi_master_write_data_byte(dev, 0xA9);
		spi_master_write_data_byte(dev, 0x51);
		spi_master_write_data_byte(dev, 0x2C);
		spi_master_write_data_byte(dev, 0x82);

		spi_master_write_comm_byte(dev, 0x11);	//Sleep Out
		delayMS(120);

		spi_master_write_comm_byte(dev, 0x29);	//Display ON
	} // endif 0x7796

	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735) {
		if (dev->_model == 0x9340)
			ESP_LOGI(TAG,"Your TFT is ILI9340");
		if (dev->_model == 0x9341)
			ESP_LOGI(TAG,"Your TFT is ILI9341");
		if (dev->_model == 0x7735)
			ESP_LOGI(TAG,"Your TFT is ST7735");
		ESP_LOGI(TAG,"Screen width:%d",width);
		ESP_LOGI(TAG,"Screen height:%d",height);
		spi_master_write_comm_byte(dev, 0xC0);	//Power Control 1
		spi_master_write_data_byte(dev, 0x23);

		spi_master_write_comm_byte(dev, 0xC1);	//Power Control 2
		spi_master_write_data_byte(dev, 0x10);
	
		spi_master_write_comm_byte(dev, 0xC5);	//VCOM Control 1
		spi_master_write_data_byte(dev, 0x3E);
		spi_master_write_data_byte(dev, 0x28);
	
		spi_master_write_comm_byte(dev, 0xC7);	//VCOM Control 2
		spi_master_write_data_byte(dev, 0x86);

		spi_master_write_comm_byte(dev, 0x36);	//Memory Access Control
		spi_master_write_data_byte(dev, 0x08);	//Right top start, BGR color filter panel
		//spi_master_write_data_byte(dev, 0x00);//Right top start, RGB color filter panel

		spi_master_write_comm_byte(dev, 0x3A);	//Pixel Format Set
		spi_master_write_data_byte(dev, 0x55);	//65K color: 16-bit/pixel

		spi_master_write_comm_byte(dev, 0x20);	//Display Inversion OFF

		spi_master_write_comm_byte(dev, 0xB1);	//Frame Rate Control
		spi_master_write_data_byte(dev, 0x00);
		spi_master_write_data_byte(dev, 0x18);

		spi_master_write_comm_byte(dev, 0xB6);	//Display Function Control
		spi_master_write_data_byte(dev, 0x08);
		spi_master_write_data_byte(dev, 0xA2);	// REV:1 GS:0 SS:0 SM:0
		spi_master_write_data_byte(dev, 0x27);
		spi_master_write_data_byte(dev, 0x00);

		spi_master_write_comm_byte(dev, 0x26);	//Gamma Set
		spi_master_write_data_byte(dev, 0x01);

		spi_master_write_comm_byte(dev, 0xE0);	//Positive Gamma Correction
		spi_master_write_data_byte(dev, 0x0F);
		spi_master_write_data_byte(dev, 0x31);
		spi_master_write_data_byte(dev, 0x2B);
		spi_master_write_data_byte(dev, 0x0C);
		spi_master_write_data_byte(dev, 0x0E);
		spi_master_write_data_byte(dev, 0x08);
		spi_master_write_data_byte(dev, 0x4E);
		spi_master_write_data_byte(dev, 0xF1);
		spi_master_write_data_byte(dev, 0x37);
		spi_master_write_data_byte(dev, 0x07);
		spi_master_write_data_byte(dev, 0x10);
		spi_master_write_data_byte(dev, 0x03);
		spi_master_write_data_byte(dev, 0x0E);
		spi_master_write_data_byte(dev, 0x09);
		spi_master_write_data_byte(dev, 0x00);

		spi_master_write_comm_byte(dev, 0xE1);	//Negative Gamma Correction
		spi_master_write_data_byte(dev, 0x00);
		spi_master_write_data_byte(dev, 0x0E);
		spi_master_write_data_byte(dev, 0x14);
		spi_master_write_data_byte(dev, 0x03);
		spi_master_write_data_byte(dev, 0x11);
		spi_master_write_data_byte(dev, 0x07);
		spi_master_write_data_byte(dev, 0x31);
		spi_master_write_data_byte(dev, 0xC1);
		spi_master_write_data_byte(dev, 0x48);
		spi_master_write_data_byte(dev, 0x08);
		spi_master_write_data_byte(dev, 0x0F);
		spi_master_write_data_byte(dev, 0x0C);
		spi_master_write_data_byte(dev, 0x31);
		spi_master_write_data_byte(dev, 0x36);
		spi_master_write_data_byte(dev, 0x0F);

		spi_master_write_comm_byte(dev, 0x11);	//Sleep Out
		delayMS(120);

		spi_master_write_comm_byte(dev, 0x29);	//Display ON
	} // endif 0x9340/0x9341/0x7735

	if (dev->_model == 0x9225) {
		ESP_LOGI(TAG,"Your TFT is ILI9225");
		ESP_LOGI(TAG,"Screen width:%d",width);
		ESP_LOGI(TAG,"Screen height:%d",height);
		lcdWriteRegisterByte(dev, 0x10, 0x0000); // Set SAP,DSTB,STB
		lcdWriteRegisterByte(dev, 0x11, 0x0000); // Set APON,PON,AON,VCI1EN,VC
		lcdWriteRegisterByte(dev, 0x12, 0x0000); // Set BT,DC1,DC2,DC3
		lcdWriteRegisterByte(dev, 0x13, 0x0000); // Set GVDD
		lcdWriteRegisterByte(dev, 0x14, 0x0000); // Set VCOMH/VCOML voltage
		delayMS(40);

		// Power-on sequence
		lcdWriteRegisterByte(dev, 0x11, 0x0018); // Set APON,PON,AON,VCI1EN,VC
		lcdWriteRegisterByte(dev, 0x12, 0x6121); // Set BT,DC1,DC2,DC3
		lcdWriteRegisterByte(dev, 0x13, 0x006F); // Set GVDD
		lcdWriteRegisterByte(dev, 0x14, 0x495F); // Set VCOMH/VCOML voltage
		lcdWriteRegisterByte(dev, 0x10, 0x0800); // Set SAP,DSTB,STB
		delayMS(10);
		lcdWriteRegisterByte(dev, 0x11, 0x103B); // Set APON,PON,AON,VCI1EN,VC
		delayMS(50);

		lcdWriteRegisterByte(dev, 0x01, 0x011C); // set the display line number and display direction
		lcdWriteRegisterByte(dev, 0x02, 0x0100); // set 1 line inversion
		lcdWriteRegisterByte(dev, 0x03, 0x1030); // set GRAM write direction and BGR=1.
		lcdWriteRegisterByte(dev, 0x07, 0x0000); // Display off
		lcdWriteRegisterByte(dev, 0x08, 0x0808); // set the back porch and front porch
		lcdWriteRegisterByte(dev, 0x0B, 0x1100); // set the clocks number per line
		lcdWriteRegisterByte(dev, 0x0C, 0x0000); // CPU interface
		//lcdWriteRegisterByte(dev, 0x0F, 0x0D01); // Set Osc
		lcdWriteRegisterByte(dev, 0x0F, 0x0801); // Set Osc
		lcdWriteRegisterByte(dev, 0x15, 0x0020); // Set VCI recycling
		lcdWriteRegisterByte(dev, 0x20, 0x0000); // RAM Address
		lcdWriteRegisterByte(dev, 0x21, 0x0000); // RAM Address

		// Set GRAM area
		lcdWriteRegisterByte(dev, 0x30, 0x0000);
		lcdWriteRegisterByte(dev, 0x31, 0x00DB);
		lcdWriteRegisterByte(dev, 0x32, 0x0000);
		lcdWriteRegisterByte(dev, 0x33, 0x0000);
		lcdWriteRegisterByte(dev, 0x34, 0x00DB);
		lcdWriteRegisterByte(dev, 0x35, 0x0000);
		lcdWriteRegisterByte(dev, 0x36, 0x00AF);
		lcdWriteRegisterByte(dev, 0x37, 0x0000);
		lcdWriteRegisterByte(dev, 0x38, 0x00DB);
		lcdWriteRegisterByte(dev, 0x39, 0x0000);

		// Adjust GAMMA Curve
		lcdWriteRegisterByte(dev, 0x50, 0x0000);
		lcdWriteRegisterByte(dev, 0x51, 0x0808);
		lcdWriteRegisterByte(dev, 0x52, 0x080A);
		lcdWriteRegisterByte(dev, 0x53, 0x000A);
		lcdWriteRegisterByte(dev, 0x54, 0x0A08);
		lcdWriteRegisterByte(dev, 0x55, 0x0808);
		lcdWriteRegisterByte(dev, 0x56, 0x0000);
		lcdWriteRegisterByte(dev, 0x57, 0x0A00);
		lcdWriteRegisterByte(dev, 0x58, 0x0710);
		lcdWriteRegisterByte(dev, 0x59, 0x0710);

		lcdWriteRegisterByte(dev, 0x07, 0x0012);
		delayMS(50); // Delay 50ms
		lcdWriteRegisterByte(dev, 0x07, 0x1017);
	} // endif 0x9225

	if (dev->_model == 0x9226) {
		ESP_LOGI(TAG,"Your TFT is ILI9225G");
		ESP_LOGI(TAG,"Screen width:%d",width);
		ESP_LOGI(TAG,"Screen height:%d",height);
		//lcdWriteRegisterByte(dev, 0x01, 0x011c);
		lcdWriteRegisterByte(dev, 0x01, 0x021c);
		lcdWriteRegisterByte(dev, 0x02, 0x0100);
		lcdWriteRegisterByte(dev, 0x03, 0x1030);
		lcdWriteRegisterByte(dev, 0x08, 0x0808); // set BP and FP
		lcdWriteRegisterByte(dev, 0x0B, 0x1100); // frame cycle
		lcdWriteRegisterByte(dev, 0x0C, 0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
		lcdWriteRegisterByte(dev, 0x0F, 0x1401); // Set frame rate----0801
		lcdWriteRegisterByte(dev, 0x15, 0x0000); // set system interface
		lcdWriteRegisterByte(dev, 0x20, 0x0000); // Set GRAM Address
		lcdWriteRegisterByte(dev, 0x21, 0x0000); // Set GRAM Address
		//*************Power On sequence ****************//
		delayMS(50);
		lcdWriteRegisterByte(dev, 0x10, 0x0800); // Set SAP,DSTB,STB----0A00
		lcdWriteRegisterByte(dev, 0x11, 0x1F3F); // Set APON,PON,AON,VCI1EN,VC----1038
		delayMS(50);
		lcdWriteRegisterByte(dev, 0x12, 0x0121); // Internal reference voltage= Vci;----1121
		lcdWriteRegisterByte(dev, 0x13, 0x006F); // Set GVDD----0066
		lcdWriteRegisterByte(dev, 0x14, 0x4349); // Set VCOMH/VCOML voltage----5F60
		//-------------- Set GRAM area -----------------//
		lcdWriteRegisterByte(dev, 0x30, 0x0000);
		lcdWriteRegisterByte(dev, 0x31, 0x00DB);
		lcdWriteRegisterByte(dev, 0x32, 0x0000);
		lcdWriteRegisterByte(dev, 0x33, 0x0000);
		lcdWriteRegisterByte(dev, 0x34, 0x00DB);
		lcdWriteRegisterByte(dev, 0x35, 0x0000);
		lcdWriteRegisterByte(dev, 0x36, 0x00AF);
		lcdWriteRegisterByte(dev, 0x37, 0x0000);
		lcdWriteRegisterByte(dev, 0x38, 0x00DB);
		lcdWriteRegisterByte(dev, 0x39, 0x0000);
		// ----------- Adjust the Gamma Curve ----------//
		lcdWriteRegisterByte(dev, 0x50, 0x0001);
		lcdWriteRegisterByte(dev, 0x51, 0x200B);
		lcdWriteRegisterByte(dev, 0x52, 0x0000);
		lcdWriteRegisterByte(dev, 0x53, 0x0404);
		lcdWriteRegisterByte(dev, 0x54, 0x0C0C);
		lcdWriteRegisterByte(dev, 0x55, 0x000C);
		lcdWriteRegisterByte(dev, 0x56, 0x0101);
		lcdWriteRegisterByte(dev, 0x57, 0x0400);
		lcdWriteRegisterByte(dev, 0x58, 0x1108);
		lcdWriteRegisterByte(dev, 0x59, 0x050C);
		delayMS(50);
		lcdWriteRegisterByte(dev, 0x07,0x1017);
	} // endif 0x9226

	if(dev->_bl >= 0) {
		gpio_set_level( dev->_bl, 1 );
	}
}


// Draw pixel
// x:X coordinate
// y:Y coordinate
// color:color
void lcdDrawPixel(TFT_t * dev, uint16_t x, uint16_t y, uint16_t color){
	if (x >= dev->_width) return;
	if (y >= dev->_height) return;

	uint16_t _x = x + dev->_offsetx;
	uint16_t _y = y + dev->_offsety;

	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_addr(dev, _x, _x);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_addr(dev, _y, _y);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		spi_master_write_data_word(dev, color);
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x7735) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_data_word(dev, _x);
		spi_master_write_data_word(dev, _x);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_data_word(dev, _y);
		spi_master_write_data_word(dev, _y);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		spi_master_write_data_word(dev, color);
	} // endif 0x7735

	if (dev->_model == 0x9225) {
		lcdWriteRegisterByte(dev, 0x20, _x);
		lcdWriteRegisterByte(dev, 0x21, _y);
		spi_master_write_comm_byte(dev, 0x22);	// Memory Write
		spi_master_write_data_word(dev, color);
	} // endif 0x9225

	if (dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x36, _x);
		lcdWriteRegisterByte(dev, 0x37, _x);
		lcdWriteRegisterByte(dev, 0x38, _y);
		lcdWriteRegisterByte(dev, 0x39, _y);
		lcdWriteRegisterByte(dev, 0x20, _x);
		lcdWriteRegisterByte(dev, 0x21, _y); 
		spi_master_write_comm_byte(dev, 0x22);
		spi_master_write_data_word(dev, color);
	} // endif 0x9226
}

// Add 202001
// Draw multi pixel
// x:X coordinate
// y:Y coordinate
// size:Number of colors
// colors:colors
void lcdDrawMultiPixels(TFT_t * dev, uint16_t x, uint16_t y, uint16_t size, uint16_t * colors) {
	if (x+size > dev->_width) return;
	if (y >= dev->_height) return;

	ESP_LOGD(TAG,"offset(x)=%d offset(y)=%d",dev->_offsetx,dev->_offsety);
	uint16_t _x1 = x + dev->_offsetx;
	uint16_t _x2 = _x1 + (size-1);
	uint16_t _y1 = y + dev->_offsety;
	uint16_t _y2 = _y1;
	ESP_LOGD(TAG,"_x1=%d _x2=%d _y1=%d _y2=%d",_x1, _x2, _y1, _y2);

	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_addr(dev, _x1, _x2);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_addr(dev, _y1, _y2);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		spi_master_write_colors(dev, colors, size);
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x7735) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_data_word(dev, _x1);
		spi_master_write_data_word(dev, _x2);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_data_word(dev, _y1);
		spi_master_write_data_word(dev, _y2);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		spi_master_write_colors(dev, colors, size);
	} // 0x7735

	if (dev->_model == 0x9225) {
		for(int j=_y1;j<=_y2;j++){
			lcdWriteRegisterByte(dev, 0x20, _x1);
			lcdWriteRegisterByte(dev, 0x21, j);
			spi_master_write_comm_byte(dev, 0x22);	// Memory Write
			spi_master_write_colors(dev, colors, size);
		}
	} // endif 0x9225

	if (dev->_model == 0x9226) {
		for(int j=_x1;j<=_x2;j++) {
			lcdWriteRegisterByte(dev, 0x36, j);
			lcdWriteRegisterByte(dev, 0x37, j);
			lcdWriteRegisterByte(dev, 0x38, _y2);
			lcdWriteRegisterByte(dev, 0x39, _y1);
			lcdWriteRegisterByte(dev, 0x20, j);
			lcdWriteRegisterByte(dev, 0x21, _y1);
			spi_master_write_comm_byte(dev, 0x22);
			spi_master_write_colors(dev, colors, size);
		}
	} // endif 0x9226

}



// Draw rectangle of filling
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color
void lcdDrawFillRect(TFT_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	if (x1 >= dev->_width) return;
	if (x2 >= dev->_width) x2=dev->_width-1;
	if (y1 >= dev->_height) return;
	if (y2 >= dev->_height) y2=dev->_height-1;

	ESP_LOGD(TAG,"offset(x)=%d offset(y)=%d",dev->_offsetx,dev->_offsety);
	uint16_t _x1 = x1 + dev->_offsetx;
	uint16_t _x2 = x2 + dev->_offsetx;
	uint16_t _y1 = y1 + dev->_offsety;
	uint16_t _y2 = y2 + dev->_offsety;

	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_addr(dev, _x1, _x2);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_addr(dev, _y1, _y2);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		for(int i=_x1;i<=_x2;i++) {
			uint16_t size = _y2-_y1+1;
			spi_master_write_color(dev, color, size);
		}
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x7735) {
		spi_master_write_comm_byte(dev, 0x2A);	// set column(x) address
		spi_master_write_data_word(dev, _x1);
		spi_master_write_data_word(dev, _x2);
		spi_master_write_comm_byte(dev, 0x2B);	// set Page(y) address
		spi_master_write_data_word(dev, _y1);
		spi_master_write_data_word(dev, _y2);
		spi_master_write_comm_byte(dev, 0x2C);	// Memory Write
		for(int i=_x1;i<=_x2;i++) {
			uint16_t size = _y2-_y1+1;
			spi_master_write_color(dev, color, size);
		}
	} // 0x7735

	if (dev->_model == 0x9225) {
		for(int j=_y1;j<=_y2;j++){
			lcdWriteRegisterByte(dev, 0x20, _x1);
			lcdWriteRegisterByte(dev, 0x21, j);
			spi_master_write_comm_byte(dev, 0x22);	// Memory Write
			uint16_t size = _x2-_x1+1;
			spi_master_write_color(dev, color, size);
		}
	} // endif 0x9225

	if (dev->_model == 0x9226) {
		for(int j=_x1;j<=_x2;j++) {
			lcdWriteRegisterByte(dev, 0x36, j);
			lcdWriteRegisterByte(dev, 0x37, j);
			lcdWriteRegisterByte(dev, 0x38, _y2);
			lcdWriteRegisterByte(dev, 0x39, _y1);
			lcdWriteRegisterByte(dev, 0x20, j);
			lcdWriteRegisterByte(dev, 0x21, _y1); 
			spi_master_write_comm_byte(dev, 0x22);
			uint16_t size = _y2-_y1+1;
			spi_master_write_color(dev, color, size);
#if 0
			for(int i=_y1;i<=_y2;i++) {
				spi_master_write_data_word(dev, color);
			}
#endif
		}
	} // endif 0x9226

}

// x0:Center X coordinate
// y0:Center Y coordinate
// size:Square size
// color:color
void lcdDrawFillRect2(TFT_t * dev, uint16_t x0, uint16_t y0, uint16_t size, uint16_t color) {
	uint16_t x1 = x0-size;
	uint16_t y1 = y0-size;
	uint16_t x2 = x0+size;
	uint16_t y2 = y0+size;
	lcdDrawFillRect(dev, x1, y1, x2, y2, color);
}

// Display OFF
void lcdDisplayOff(TFT_t * dev) {
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x28);
	} // endif 0x9340/0x9341/0x7735/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x07, 0x1014);
	} // endif 0x9225/0x9226

}
 
// Display ON
void lcdDisplayOn(TFT_t * dev) {
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x29);
	} // endif 0x9340/0x9341/0x7735/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x07, 0x1017);
	} // endif 0x9225/0x9226

}

// Display Inversion OFF
void lcdInversionOff(TFT_t * dev) {
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x20);
	} // endif 0x9340/0x9341/0x7735/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x07, 0x1017);
	} // endif 0x9225/0x9226
}

// Display Inversion ON
void lcdInversionOn(TFT_t * dev) {
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x21);
	} // endif 0x9340/0x9341/0x7735/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x07, 0x1013);
	} // endif 0x9225/0x9226
}

// Change Memory Access Control
void lcdBGRFilter(TFT_t * dev) {
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7735 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x36);	//Memory Access Control
		spi_master_write_data_byte(dev, 0x00);	//Right top start, RGB color filter panel
	} // endif 0x9340/0x9341/0x7735/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x03, 0x0030); // set GRAM write direction and BGR=0.
	} // endif 0x9225/0x9226
}
// Fill screen
// color:color
void lcdFillScreen(TFT_t * dev, uint16_t color) {
	lcdDrawFillRect(dev, 0, 0, dev->_width-1, dev->_height-1, color);
}

// Draw line
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color 
void lcdDrawLine(TFT_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int i;
	int dx,dy;
	int sx,sy;
	int E;

	/* distance between two points */
	dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
	dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

	/* direction of two point */
	sx = ( x2 > x1 ) ? 1 : -1;
	sy = ( y2 > y1 ) ? 1 : -1;

	/* inclination < 1 */
	if ( dx > dy ) {
		E = -dx;
		for ( i = 0 ; i <= dx ; i++ ) {
			lcdDrawPixel(dev, x1, y1, color);
			x1 += sx;
			E += 2 * dy;
			if ( E >= 0 ) {
			y1 += sy;
			E -= 2 * dx;
		}
	}

	/* inclination >= 1 */
	} else {
		E = -dy;
		for ( i = 0 ; i <= dy ; i++ ) {
			lcdDrawPixel(dev, x1, y1, color);
			y1 += sy;
			E += 2 * dx;
			if ( E >= 0 ) {
				x1 += sx;
				E -= 2 * dy;
			}
		}
	}
}

// Draw rectangle
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color
void lcdDrawRect(TFT_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	lcdDrawLine(dev, x1, y1, x2, y1, color);
	lcdDrawLine(dev, x2, y1, x2, y2, color);
	lcdDrawLine(dev, x2, y2, x1, y2, color);
	lcdDrawLine(dev, x1, y2, x1, y1, color);
}

// x0:Center X coordinate
// y0:Center Y coordinate
// size:Square size
// color:color
void lcdDrawRect2(TFT_t * dev, uint16_t x0, uint16_t y0, uint16_t size, uint16_t color) {
	lcdDrawLine(dev, x0-size, y0-size, x0+size, y0-size, color);
	lcdDrawLine(dev, x0+size, y0-size, x0+size, y0+size, color);
	lcdDrawLine(dev, x0+size, y0+size, x0-size, y0+size, color);
	lcdDrawLine(dev, x0-size, y0+size, x0-size, y0-size, color);
}


// Draw rectangle with angle
// xc:Center X coordinate
// yc:Center Y coordinate
// w:Width of rectangle
// h:Height of rectangle
// angle:Angle of rectangle
// color:color

//When the origin is (0, 0), the point (x1, y1) after rotating the point (x, y) by the angle is obtained by the following calculation.
// x1 = x * cos(angle) - y * sin(angle)
// y1 = x * sin(angle) + y * cos(angle)
void lcdDrawRectAngle(TFT_t * dev, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color) {
		double xd,yd,rd;
		int x1,y1;
		int x2,y2;
		int x3,y3;
		int x4,y4;
		rd = -angle * M_PI / 180.0;
		xd = 0.0 - w/2;
		yd = h/2;
		x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		yd = 0.0 - yd;
		x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		xd = w/2;
		yd = h/2;
		x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		yd = 0.0 - yd;
		x4 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y4 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		lcdDrawLine(dev, x1, y1, x2, y2, color);
		lcdDrawLine(dev, x1, y1, x3, y3, color);
		lcdDrawLine(dev, x2, y2, x4, y4, color);
		lcdDrawLine(dev, x3, y3, x4, y4, color);
}


// Draw triangle
// xc:Center X coordinate
// yc:Center Y coordinate
// w:Width of triangle
// h:Height of triangle
// angle:Angle of triangle
// color:color

//When the origin is (0, 0), the point (x1, y1) after rotating the point (x, y) by the angle is obtained by the following calculation.
// x1 = x * cos(angle) - y * sin(angle)
// y1 = x * sin(angle) + y * cos(angle)
void lcdDrawTriangle(TFT_t * dev, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color) {
		double xd,yd,rd;
		int x1,y1;
		int x2,y2;
		int x3,y3;
		rd = -angle * M_PI / 180.0;
		xd = 0.0;
		yd = h/2;
		x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		xd = w/2;
		yd = 0.0 - yd;
		x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		xd = 0.0 - w/2;
		x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		lcdDrawLine(dev, x1, y1, x2, y2, color);
		lcdDrawLine(dev, x1, y1, x3, y3, color);
		lcdDrawLine(dev, x2, y2, x3, y3, color);
}

// Draw regular polygon
// xc:Center X coordinate
// yc:Center Y coordinate
// n:Number of slides
// r:radius
// angle:Angle of regular polygon
// color:color
void lcdDrawRegularPolygon(TFT_t * dev, uint16_t xc, uint16_t yc, uint16_t n, uint16_t r, uint16_t angle, uint16_t color)
{
	double xd,yd,rd;
	int x1,y1;
	int x2,y2;
	int i;

	rd = -angle * M_PI / 180.0;
	for(i=0;i<n;i++) {
		xd = r * cos(2 * M_PI * i / n);
		yd = r * sin(2 * M_PI * i / n);
		x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		xd = r * cos(2 * M_PI * (i+1) / n);
		yd = r * sin(2 * M_PI * (i+1) / n);
		x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
		y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

		lcdDrawLine(dev, x1, y1, x2, y2, color);
	}
}

// Draw circle
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void lcdDrawCircle(TFT_t * dev, uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int x;
	int y;
	int err;
	int old_err;

	x=0;
	y=-r;
	err=2-2*r;
	do{
		lcdDrawPixel(dev, x0-x, y0+y, color); 
		lcdDrawPixel(dev, x0-y, y0-x, color); 
		lcdDrawPixel(dev, x0+x, y0-y, color); 
		lcdDrawPixel(dev, x0+y, y0+x, color); 
		if ((old_err=err)<=x) err+=++x*2+1;
		if (old_err>y || err>x) err+=++y*2+1;
	} while(y<0);
}

// Draw circle of filling
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void lcdDrawFillCircle(TFT_t * dev, uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int x;
	int y;
	int err;
	int old_err;
	int ChangeX;

	x=0;
	y=-r;
	err=2-2*r;
	ChangeX=1;
	do{
		if(ChangeX) {
			lcdDrawLine(dev, x0-x, y0-y, x0-x, y0+y, color);
			lcdDrawLine(dev, x0+x, y0-y, x0+x, y0+y, color);
		} // endif
		ChangeX=(old_err=err)<=x;
		if (ChangeX) err+=++x*2+1;
		if (old_err>y || err>x) err+=++y*2+1;
	} while(y<=0);
} 

// Draw rectangle with round corner
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// r:radius
// color:color
void lcdDrawRoundRect(TFT_t * dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color) {
	int x;
	int y;
	int err;
	int old_err;
	unsigned char temp;

	if(x1>x2) {
		temp=x1; x1=x2; x2=temp;
	} // endif
		
	if(y1>y2) {
		temp=y1; y1=y2; y2=temp;
	} // endif

	ESP_LOGD(TAG, "x1=%d x2=%d delta=%d r=%d",x1, x2, x2-x1, r);
	ESP_LOGD(TAG, "y1=%d y2=%d delta=%d r=%d",y1, y2, y2-y1, r);
	if (x2-x1 < r) return; // Add 20190517
	if (y2-y1 < r) return; // Add 20190517

	x=0;
	y=-r;
	err=2-2*r;

	do{
		if(x) {
			lcdDrawPixel(dev, x1+r-x, y1+r+y, color); 
			lcdDrawPixel(dev, x2-r+x, y1+r+y, color); 
			lcdDrawPixel(dev, x1+r-x, y2-r-y, color); 
			lcdDrawPixel(dev, x2-r+x, y2-r-y, color);
		} // endif 
		if ((old_err=err)<=x) err+=++x*2+1;
		if (old_err>y || err>x) err+=++y*2+1;
	} while(y<0);

	ESP_LOGD(TAG, "x1+r=%d x2-r=%d",x1+r, x2-r);
	lcdDrawLine(dev, x1+r, y1, x2-r, y1, color);
	lcdDrawLine(dev, x1+r, y2, x2-r, y2, color);
	ESP_LOGD(TAG, "y1+r=%d y2-r=%d",y1+r, y2-r);
	lcdDrawLine(dev, x1, y1+r, x1, y2-r, color);
	lcdDrawLine(dev, x2, y1+r, x2, y2-r, color);	
} 

// Draw arrow
// x0:Start X coordinate
// y0:Start Y coordinate
// x1:End X coordinate
// y1:End Y coordinate
// w:Width of the botom
// color:color
// Thanks http://k-hiura.cocolog-nifty.com/blog/2010/11/post-2a62.html
void lcdDrawArrow(TFT_t * dev, uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color) {
	double Vx= x1 - x0;
	double Vy= y1 - y0;
	double v = sqrt(Vx*Vx+Vy*Vy);
	//printf("v=%f\n",v);
	double Ux= Vx/v;
	double Uy= Vy/v;

	uint16_t L[2],R[2];
	L[0]= x1 - Uy*w - Ux*v;
	L[1]= y1 + Ux*w - Uy*v;
	R[0]= x1 + Uy*w - Ux*v;
	R[1]= y1 - Ux*w - Uy*v;
	//printf("L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);

	//lcdDrawLine(x0,y0,x1,y1,color);
	lcdDrawLine(dev, x1, y1, L[0], L[1], color);
	lcdDrawLine(dev, x1, y1, R[0], R[1], color);
	lcdDrawLine(dev, L[0], L[1], R[0], R[1], color);
}


// Draw arrow of filling
// x0:Start X coordinate
// y0:Start Y coordinate
// x1:End X coordinate
// y1:End Y coordinate
// w:Width of the botom
// color:color
void lcdDrawFillArrow(TFT_t * dev, uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color) {
	double Vx= x1 - x0;
	double Vy= y1 - y0;
	double v = sqrt(Vx*Vx+Vy*Vy);
	//printf("v=%f\n",v);
	double Ux= Vx/v;
	double Uy= Vy/v;

	uint16_t L[2],R[2];
	L[0]= x1 - Uy*w - Ux*v;
	L[1]= y1 + Ux*w - Uy*v;
	R[0]= x1 + Uy*w - Ux*v;
	R[1]= y1 - Ux*w - Uy*v;
	//printf("L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);

	lcdDrawLine(dev, x0, y0, x1, y1, color);
	lcdDrawLine(dev, x1, y1, L[0], L[1], color);
	lcdDrawLine(dev, x1, y1, R[0], R[1], color);
	lcdDrawLine(dev, L[0], L[1], R[0], R[1], color);

	int ww;
	for(ww=w-1;ww>0;ww--) {
		L[0]= x1 - Uy*ww - Ux*v;
		L[1]= y1 + Ux*ww - Uy*v;
		R[0]= x1 + Uy*ww - Ux*v;
		R[1]= y1 - Ux*ww - Uy*v;
		//printf("Fill>L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);
		lcdDrawLine(dev, x1, y1, L[0], L[1], color);
		lcdDrawLine(dev, x1, y1, R[0], R[1], color);
	}
}

// Draw ASCII character
// x:X coordinate
// y:Y coordinate
// ascii:ascii code
// color:color
int lcdDrawChar(TFT_t * dev, FontxFile *fxs, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) {
	uint16_t xx,yy,bit,ofs;
	unsigned char fonts[128]; // font pattern
	unsigned char pw, ph;
	int h,w;
	uint16_t mask;
	bool rc;

	if(_DEBUG_)printf("_font_direction=%d\n",dev->_font_direction);
	rc = GetFontx(fxs, ascii, fonts, &pw, &ph);
	if(_DEBUG_)printf("GetFontx rc=%d pw=%d ph=%d\n",rc,pw,ph);
	if (!rc) return 0;

	int16_t xd1 = 0;
	int16_t yd1 = 0;
	int16_t xd2 = 0;
	int16_t yd2 = 0;
	int16_t xss = 0;
	int16_t yss = 0;
	int16_t xsd = 0;
	int16_t ysd = 0;
	int16_t next = 0;
	int16_t x0	= 0;
	int16_t x1	= 0;
	int16_t y0	= 0;
	int16_t y1	= 0;
	if (dev->_font_direction == 0) {
		xd1 = +1;
		yd1 = +1; //-1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y - (ph - 1);
		xsd =  1;
		ysd =  0;
		next = x + pw;

		x0	= x;
		y0	= y - (ph-1);
		x1	= x + (pw-1);
		y1	= y;
	} else if (dev->_font_direction == 2) {
		xd1 = -1;
		yd1 = -1; //+1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y + ph + 1;
		xsd =  1;
		ysd =  0;
		next = x - pw;

		x0	= x - (pw-1);
		y0	= y;
		x1	= x;
		y1	= y + (ph-1);
	} else if (dev->_font_direction == 1) {
		xd1 =  0;
		yd1 =  0;
		xd2 = -1;
		yd2 = +1; //-1;
		xss =  x + ph;
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y + pw; //y - pw;

		x0	= x;
		y0	= y;
		x1	= x + (ph-1);
		y1	= y + (pw-1);
	} else if (dev->_font_direction == 3) {
		xd1 =  0;
		yd1 =  0;
		xd2 = +1;
		yd2 = -1; //+1;
		xss =  x - (ph - 1);
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y - pw; //y + pw;

		x0	= x - (ph-1);
		y0	= y - (pw-1);
		x1	= x;
		y1	= y;
	}

	if (dev->_font_fill) lcdDrawFillRect(dev, x0, y0, x1, y1, dev->_font_fill_color);

	int bits;
	if(_DEBUG_)printf("xss=%d yss=%d\n",xss,yss);
	ofs = 0;
	yy = yss;
	xx = xss;
	for(h=0;h<ph;h++) {
		if(xsd) xx = xss;
		if(ysd) yy = yss;
		//for(w=0;w<(pw/8);w++) {
		bits = pw;
		for(w=0;w<((pw+4)/8);w++) {
			mask = 0x80;
			for(bit=0;bit<8;bit++) {
				bits--;
				if (bits < 0) continue;
				//if(_DEBUG_)printf("xx=%d yy=%d mask=%02x fonts[%d]=%02x\n",xx,yy,mask,ofs,fonts[ofs]);
				if (fonts[ofs] & mask) {
					lcdDrawPixel(dev, xx, yy, color);
				} else {
					//if (dev->_font_fill) lcdDrawPixel(dev, xx, yy, dev->_font_fill_color);
				}
				if (h == (ph-2) && dev->_font_underline)
					lcdDrawPixel(dev, xx, yy, dev->_font_underline_color);
				if (h == (ph-1) && dev->_font_underline)
					lcdDrawPixel(dev, xx, yy, dev->_font_underline_color);
				xx = xx + xd1;
				yy = yy + yd2;
				mask = mask >> 1;
			}
			ofs++;
		}
		yy = yy + yd1;
		xx = xx + xd2;
	}

	if (next < 0) next = 0;
	return next;
}

int lcdDrawString(TFT_t * dev, FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color) {
	int length = strlen((char *)ascii);
	if(_DEBUG_)printf("lcdDrawString length=%d\n",length);
	for(int i=0;i<length;i++) {
		if(_DEBUG_)printf("ascii[%d]=%x x=%d y=%d\n",i,ascii[i],x,y);
		if (dev->_font_direction == 0)
			x = lcdDrawChar(dev, fx, x, y, ascii[i], color);
		if (dev->_font_direction == 1)
			y = lcdDrawChar(dev, fx, x, y, ascii[i], color);
		if (dev->_font_direction == 2)
			x = lcdDrawChar(dev, fx, x, y, ascii[i], color);
		if (dev->_font_direction == 3)
			y = lcdDrawChar(dev, fx, x, y, ascii[i], color);
	}
	if (dev->_font_direction == 0) return x;
	if (dev->_font_direction == 2) return x;
	if (dev->_font_direction == 1) return y;
	if (dev->_font_direction == 3) return y;
	return 0;
}


// Draw character using code
// x:X coordinate
// y:Y coordinate
// code:character code
// color:color
int lcdDrawCode(TFT_t * dev, FontxFile *fx, uint16_t x,uint16_t y,uint8_t code,uint16_t color) {
	if(_DEBUG_)printf("code=%x x=%d y=%d\n",code,x,y);
	if (dev->_font_direction == 0)
		x = lcdDrawChar(dev, fx, x, y, code, color);
	if (dev->_font_direction == 1)
		y = lcdDrawChar(dev, fx, x, y, code, color);
	if (dev->_font_direction == 2)
		x = lcdDrawChar(dev, fx, x, y, code, color);
	if (dev->_font_direction == 3)
		y = lcdDrawChar(dev, fx, x, y, code, color);
	if (dev->_font_direction == 0) return x;
	if (dev->_font_direction == 2) return x;
	if (dev->_font_direction == 1) return y;
	if (dev->_font_direction == 3) return y;
	return 0;
}

#if 0
// Draw SJIS character
// x:X coordinate
// y:Y coordinate
// sjis:SJIS code
// color:color
int lcdDrawSJISChar(TFT_t * dev, FontxFile *fxs, uint16_t x,uint16_t y,uint16_t sjis,uint16_t color) {
	uint16_t xx,yy,bit,ofs;
	unsigned char fonts[128]; // font pattern
	unsigned char pw, ph;
	int h,w;
	uint16_t mask;
	bool rc;

	if(_DEBUG_)printf("_font_direction=%d\n",dev->_font_direction);
	//sjis = UTF2SJIS(utf8);
	//if(_DEBUG_)printf("sjis=%04x\n",sjis);

	rc = GetFontx(fxs, sjis, fonts, &pw, &ph); // SJIS -> Font pattern
	if(_DEBUG_)printf("GetFontx rc=%d pw=%d ph=%d\n",rc,pw,ph);
	if (!rc) return 0;

	uint16_t xd1, yd1;
	uint16_t xd2, yd2;
	uint16_t xss, yss;
	uint16_t xsd, ysd;
	int next;
	if (dev->_font_direction == 0) {
		xd1 = +1;
		yd1 = -1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y + ph - 1;
		xsd =  1;
		ysd =  0;
		next = x + pw;
	} else if (dev->_font_direction == 2) {
		xd1 = -1;
		yd1 = +1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y - ph + 1;
		xsd =  1;
		ysd =  0;
		next = x - pw;
	} else if (dev->_font_direction == 1) {
		xd1 =  0;
		yd1 =  0;
		xd2 = -1;
		yd2 = -1;
		xss =  x + ph;
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y - pw;
	} else if (dev->_font_direction == 3) {
		xd1 =  0;
		yd1 =  0;
		xd2 = +1;
		yd2 = +1;
		xss =  x - ph - 1;
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y + pw;
	}

	int bits;
	if(_DEBUG_)printf("xss=%d yss=%d\n",xss,yss);
	ofs = 0;
	yy = yss;
	xx = xss;
	for(h=0;h<ph;h++) {
		if(xsd) xx = xss;
		if(ysd) yy = yss;
		//for(w=0;w<(pw/8);w++) {
		bits = pw;
		for(w=0;w<((pw+4)/8);w++) {
			mask = 0x80;
			for(bit=0;bit<8;bit++) {
				bits--;
				if (bits < 0) continue;
				//if(_DEBUG_)printf("xx=%d yy=%d mask=%02x fonts[%d]=%02x\n",xx,yy,mask,ofs,fonts[ofs]);
				if (fonts[ofs] & mask) {
					lcdDrawPixel(dev, xx, yy, color);
				} else {
					if (dev->_font_fill) lcdDrawPixel(dev, xx, yy, dev->_font_fill_color);
				}
				if (h == (ph-2) && dev->_font_underline)
					lcdDrawPixel(xx, yy, dev->_font_underline_color);
				if (h == (ph-1) && dev->_font_underline)
					lcdDrawPixel(xx, yy, dev->_font_underline_color);
				xx = xx + xd1;
				yy = yy + yd2;
				mask = mask >> 1;
			}
			ofs++;
		}
		yy = yy + yd1;
		xx = xx + xd2;
	}

	if (next < 0) next = 0;
	return next;
}

// Draw UTF8 character
// x:X coordinate
// y:Y coordinate
// utf8:UTF8 code
// color:color
int lcdDrawUTF8Char(TFT_t * dev, FontxFile *fx, uint16_t x,uint16_t y,uint8_t *utf8,uint16_t color) {
	uint16_t sjis[1];

	sjis[0] = UTF2SJIS(utf8);
	if(_DEBUG_)printf("sjis=%04x\n",sjis[0]);
	return lcdDrawSJISChar(dev, fx, x, y, sjis[0], color);
}

// Draw UTF8 string
// x:X coordinate
// y:Y coordinate
// utfs:UTF8 string
// color:color
int lcdDrawUTF8String(TFT_t * dev, FontxFile *fx, uint16_t x, uint16_t y, unsigned char *utfs, uint16_t color) {

	int i;
	int spos;
	uint16_t sjis[64];
	spos = String2SJIS(utfs, strlen((char *)utfs), sjis, 64);
	if(_DEBUG_)printf("spos=%d\n",spos);
	for(i=0;i<spos;i++) {
		if(_DEBUG_)printf("sjis[%d]=%x y=%d\n",i,sjis[i],y);
		if (dev->_font_direction == 0)
			x = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 1)
			y = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 2)
			x = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 3)
			y = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
	}
	if (dev->_font_direction == 0) return x;
	if (dev->_font_direction == 2) return x;
	if (dev->_font_direction == 1) return y;
	if (dev->_font_direction == 3) return y;
	return 0;
}
#endif

// Set font direction
// dir:Direction
void lcdSetFontDirection(TFT_t * dev, uint16_t dir) {
	dev->_font_direction = dir;
}

// Set font filling
// color:fill color
void lcdSetFontFill(TFT_t * dev, uint16_t color) {
	dev->_font_fill = true;
	dev->_font_fill_color = color;
}

// UnSet font filling
void lcdUnsetFontFill(TFT_t * dev) {
	dev->_font_fill = false;
}

// Set font underline
// color:frame color
void lcdSetFontUnderLine(TFT_t * dev, uint16_t color) {
	dev->_font_underline = true;
	dev->_font_underline_color = color;
}

// UnSet font underline
void lcdUnsetFontUnderLine(TFT_t * dev) {
	dev->_font_underline = false;
}

// Backlight OFF
void lcdBacklightOff(TFT_t * dev) {
	if(dev->_bl >= 0) {
		gpio_set_level( dev->_bl, 0 );
	}
}

// Backlight ON
void lcdBacklightOn(TFT_t * dev) {
	if(dev->_bl >= 0) {
		gpio_set_level( dev->_bl, 1 );
	}
}

// Vertical Scrolling Definition
// tfa:Top Fixed Area
// vsa:Vertical Scrolling Area
// bfa:Bottom Fixed Area
void lcdSetScrollArea(TFT_t * dev, uint16_t tfa, uint16_t vsa, uint16_t bfa){
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x33);	// Vertical Scrolling Definition
		spi_master_write_data_word(dev, tfa);
		spi_master_write_data_word(dev, vsa);
		spi_master_write_data_word(dev, bfa);
		//spi_master_write_comm_byte(dev, 0x12);	// Partial Mode ON
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x31, vsa);	// Specify scroll end and step at the scroll display
		lcdWriteRegisterByte(dev, 0x32, tfa);	// Specify scroll start and step at the scroll display
#if 0
		spi_master_write_comm_byte(dev, 0x31);	// Specify scroll end address at the scroll display
		spi_master_write_data_word(dev, vsa);
		spi_master_write_comm_byte(dev, 0x32);	// Specify scroll start address at the scroll display
		spi_master_write_data_word(dev, tfa);
#endif
	} // endif 0x9225/0x9226
}

void lcdResetScrollArea(TFT_t * dev, uint16_t vsa){
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x33);	// Vertical Scrolling Definition
		spi_master_write_data_word(dev, 0);
		//spi_master_write_data_word(dev, 0x140);
		spi_master_write_data_word(dev, vsa);
		spi_master_write_data_word(dev, 0);
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x31, 0x0);	// Specify scroll end and step at the scroll display
		lcdWriteRegisterByte(dev, 0x32, 0x0);	// Specify scroll start and step at the scroll display
		//lcdWriteRegisterByte(dev, 0x31, vsa);	// Specify scroll end and step at the scroll display
		//lcdWriteRegisterByte(dev, 0x32, tfa);	// Specify scroll start and step at the scroll display
	} // endif 0x9225/0x9226
}

// Vertical Scrolling Start Address
// vsp:Vertical Scrolling Start Address
void lcdScroll(TFT_t * dev, uint16_t vsp){
	if (dev->_model == 0x9340 || dev->_model == 0x9341 || dev->_model == 0x7796) {
		spi_master_write_comm_byte(dev, 0x37);	// Vertical Scrolling Start Address
		spi_master_write_data_word(dev, vsp);
	} // endif 0x9340/0x9341/0x7796

	if (dev->_model == 0x9225 || dev->_model == 0x9226) {
		lcdWriteRegisterByte(dev, 0x33, vsp);	// Vertical Scrolling Start Address
#if 0
		spi_master_write_comm_byte(dev, 0x33);	// Vertical Scrolling Start Address
		spi_master_write_data_word(dev, vsp);
#endif
	} // endif 0x9225/0x9226
}

#define MAX_LEN 3
#define	XPT_START	0x80
#define XPT_XPOS	0x50
#define XPT_YPOS	0x10
#define XPT_8BIT  0x80
#define XPT_SER		0x04
#define XPT_DEF		0x03


int xptGetit(TFT_t * dev, int cmd){
	char rbuf[MAX_LEN];
	char wbuf[MAX_LEN];

	memset(wbuf, 0, sizeof(rbuf));
	memset(rbuf, 0, sizeof(rbuf));
	wbuf[0] = cmd;
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
	SPITransaction.length = MAX_LEN * 8;
	SPITransaction.tx_buffer = wbuf;
	SPITransaction.rx_buffer = rbuf;
#if 1
	ret = spi_device_transmit( dev->_XPT_Handle, &SPITransaction );
#else
	ret = spi_device_polling_transmit( dev->_XPT_Handle, &SPITransaction );
#endif
	assert(ret==ESP_OK); 
	ESP_LOGD(TAG, "rbuf[0]=%02x rbuf[1]=%02x rbuf[2]=%02x", rbuf[0], rbuf[1], rbuf[2]);
	// 12bit Conversion
	//int pos = (rbuf[1]<<8)+rbuf[2];
	int pos = (rbuf[1]<<4)+(rbuf[2]>>4);
	return(pos);
}

bool touch_getxy(TFT_t *dev, int *xp, int *yp) {
	int level = gpio_get_level(dev->_irq);
	ESP_LOGD(__FUNCTION__, "gpio_get_level=%d", level);
	if (level == 1) return false; // Not touched
	*xp = xptGetit(dev, (XPT_START | XPT_XPOS | XPT_SER) );
	*yp = xptGetit(dev, (XPT_START | XPT_YPOS | XPT_SER) );
	return true;
}
