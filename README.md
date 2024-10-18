# esp-idf-ili9340
SPI TFT and XPT2046 touch screen controller driver for esp-idf.

# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.0 is required when using ESP32C2.   
ESP-IDF V5.1 is required when using ESP32C6.   

# Installation for ESP-IDF V4.4

```
git clone -b v4.4 https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
idf.py menuconfig
idf.py flash
```

__Note for ESP32-S2__   
The tjpgd library is not included in the ESP32-S2 ROM because the ROM of the ESP32-S2 is small.   
Therefore, JPEG files cannot be displayed.   

__Note for ESP32-C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32-C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   

# Installation for ESP-IDF V5.x

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

__Note for ESP32-S2/ESP32-C2__   
The tjpgd library is not included in the ESP32-S2/ESP32-C2 ROM.   
However, you can use [this](https://components.espressif.com/components/espressif/esp_jpeg) IDF component registry.   
JPEG files can be displayed.   

__Note for ESP32-C2__   
ESP32-C2 has less SRAM, so JPEG and PNG may not be displayed on higher resolution TFTs.   
```
E (256560) decode_jpeg: Error allocating memory for line 251
E (260630) pngle_new: Error allocating memory for line 160
```

__Note for ESP32-C6__   
ESP-IDF V5.1 is required when using ESP32-C6.   


# Configuration   
You have to set this config value with menuconfig.   
- CONFIG_WIDTH   
- CONFIG_HEIGHT   
- CONFIG_OFFSETX   
- CONFIG_OFFSETY   
- CONFIG_MOSI_GPIO   
- CONFIG_SCLK_GPIO   
- CONFIG_CS_GPIO   
- CONFIG_DC_GPIO   
- CONFIG_RESET_GPIO   
- CONFIG_BL_GPIO   
 __GPIO of ESP32 cannot supply too much current.__   
 __TFT backlight becomes brighter when powered by an external power source.__   

__TFT MISO is not use.__   

![config-menu](https://user-images.githubusercontent.com/6020549/101022538-db9fcb00-35b4-11eb-95a9-ad84a880c7fb.jpg)

---

# Tested TFT   
- M5Stack   
- Shenzhen Jingcal Intelligent 3.5" ST7796 480x320 (ESP32-3248S035)   
- Shenzhen Jingcal Intelligent 2.8" ILI9341 320x240 (ESP32-2432S028R)   
- 4.0" ST7796 480x320   
- 3.2" ILI9341 320x240   
- 2.8" ILI9341 320x240   
- 2.4" ILI9341 320x240   
- 2.2" ILI9340 320x240   
- 2.0" ILI9225 176x220   
- 2.0" ILI9225G 176x220   
- 1.8" ST7735 128x160   
- 1.77" ST7735 128x160   
- 1.44" ST7735 128x128   
- 0.96" ST7735 80x160   


### Generic Product   
![spi-tft](https://user-images.githubusercontent.com/6020549/59512687-3098de80-8ef4-11e9-9759-530ffe47d659.JPG)

__A note about RESET__   
Pull Up of the RESET pin may be required. I inserted a 100 ohm resistor between Vcc and RESET.   

### Shenzhen Jingcal Intelligent Product   
They can use touch screens.   
![Shenzhen_Jingcal_Intelligent](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/eda884bb-9fac-40ea-8396-d77bb0de074b)

__ESP32-2432S032/ESP32-4827S043/ESP32-8048S043 doesn't work because it's an RGB panel, not SPI.__   

---

# M5Stack

![config-m5stack](https://user-images.githubusercontent.com/6020549/101022626-f96d3000-35b4-11eb-8c60-a8a4896306d9.jpg)

![M5Stick-1](https://user-images.githubusercontent.com/6020549/57977479-7d43e380-7a34-11e9-99b6-6028500436e8.JPG)
![M5Stack-2](https://user-images.githubusercontent.com/6020549/59008264-f7fa6480-8864-11e9-92f6-ffbd979fa75e.JPG)
![M5Stick-3](https://user-images.githubusercontent.com/6020549/57977481-7d43e380-7a34-11e9-9bc6-8e415aa52c52.JPG)
![M5Stick-4](https://user-images.githubusercontent.com/6020549/57977482-7d43e380-7a34-11e9-8188-653cb02f3ba0.JPG)
![M5Stick-5](https://user-images.githubusercontent.com/6020549/57977483-7ddc7a00-7a34-11e9-936e-4d97b1411610.JPG)
![M5Stick-6](https://user-images.githubusercontent.com/6020549/57977484-7ddc7a00-7a34-11e9-8750-52db073c96c2.JPG)
![M5Stick-7](https://user-images.githubusercontent.com/6020549/57977485-7e751080-7a34-11e9-95f9-ffb12879d1b0.JPG)
![M5Stick-8](https://user-images.githubusercontent.com/6020549/57977486-7e751080-7a34-11e9-9ac8-c546f248fdec.JPG)
![M5Stick-9](https://user-images.githubusercontent.com/6020549/57977487-7e751080-7a34-11e9-9a3e-6a0bd7359efb.JPG)
![M5Stick-10](https://user-images.githubusercontent.com/6020549/57977488-7e751080-7a34-11e9-9a12-e2b70334604d.JPG)
![M5Stick-11](https://user-images.githubusercontent.com/6020549/57977489-7f0da700-7a34-11e9-9ea3-c0420a785a3e.JPG)

BMP file   
![M5Stack-BMP](https://user-images.githubusercontent.com/6020549/78413964-dcaee000-7654-11ea-88f2-e70662d761e1.JPG)

JPEG file(Cannot be displayed on ESP32S2)   
![M5Stack-JPEG](https://user-images.githubusercontent.com/6020549/78413968-e0426700-7654-11ea-9040-0fdfd0f2de2e.JPG)

PNG file   
![M5Stack-PNG](https://user-images.githubusercontent.com/6020549/78613610-40c8e280-78a7-11ea-95b0-a89ce14dc196.JPG)

PNG icon
![M5Stack-ICON](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/120af5a4-9b31-476a-a9db-258c2d0072e7)

---

# 4.0" ST7796S 480x320

![config-st7796](https://user-images.githubusercontent.com/6020549/101022680-0e49c380-35b5-11eb-914f-f522b33bb49a.jpg)

Left:4.0" Right:2.4"
![4_0_st779s](https://user-images.githubusercontent.com/6020549/85913118-e7040300-b86c-11ea-87fd-fdcd97c5bf26.JPG)

---

# Shenzhen Jingcal Intelligent 3.5" ST7796 480x320
Vendor part number is ESP32-3248S035.   

![config-ESP32-3248S035](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/68749d5a-9047-4725-bc06-281f37e44539)
![ESP32-3248S035-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/ee2b7eb5-2627-4bbf-b7a5-8a31005cf22d)
![ESP32-3248S035-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/56d7bc3b-ab03-4486-bd51-bc16d6138d1c)

---

# 3.2" ILI9341 320x240
# 2.8" ILI9341 320x240
# 2.4" ILI9341 320x240

![config-ili9341](https://user-images.githubusercontent.com/6020549/101022718-1b66b280-35b5-11eb-8103-7bbb1f0a7a30.jpg)

Left:3.2" Right:2.4"
![3_2_ili9341](https://user-images.githubusercontent.com/6020549/85909286-0b9fb100-b854-11ea-9935-942fe7344f25.JPG)

Left:2.8" Right:2.4"
![screen_2-8_240x320](https://user-images.githubusercontent.com/6020549/59007633-3f332600-8862-11e9-8d8f-bbfb303456f4.JPG)

---

# Shenzhen Jingcal Intelligent 2.8" ILI9341 320x240
Vendor part number is ESP32-2432S028R.   

![config-ESP32-2432S028R-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/317e365c-3c1a-4c6f-9c14-f07feacbd4d8)
![ESP32-2432S028R-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/e9e4b502-4063-49b2-b58a-d60aeccdd380)
![ESP32-2432S028R-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/d6d6cc27-463a-4ad5-a739-441d6ebdc037)


---

# 2.2" ILI9340 320x240

![config-ili9340](https://user-images.githubusercontent.com/6020549/101022746-2588b100-35b5-11eb-8eaa-1f86cc4c9f93.jpg)

Left:2.2" Right:2.4"
![screen_2-2_240x320](https://user-images.githubusercontent.com/6020549/59007758-d304f200-8862-11e9-8d61-fa4b734f4b9a.JPG)

---

# 2.0" ILI9225 176x220

![config-ili9225](https://user-images.githubusercontent.com/6020549/101022807-3a654480-35b5-11eb-9e3d-b2eb5e396f08.jpg)

Left:2.0" Right:2.4"
![2_0_ili9225](https://user-images.githubusercontent.com/6020549/85910092-004e8480-b858-11ea-98f9-2ba9efe5880a.JPG)

---

# 2.0" ILI9225G 176x220

![config-ili9225g](https://user-images.githubusercontent.com/6020549/101022833-42bd7f80-35b5-11eb-95d9-2e821d4757e6.jpg)

Left:2.0" Right:2.4"
![2_0_ili9225g](https://user-images.githubusercontent.com/6020549/85910094-03e20b80-b858-11ea-9e60-6efa28b0d644.JPG)

---

# 1.8" ST7735 128x160

![config-st7735-128x160-1](https://user-images.githubusercontent.com/6020549/101023253-dc852c80-35b5-11eb-97c8-4873200f85e7.jpg)

Left:1.8" Right:2.4"
![1_8_st7735](https://user-images.githubusercontent.com/6020549/85910349-3cceb000-b859-11ea-98a0-ecda6870d5aa.JPG)

---

# 1.8" ST7735 128x160

![config-st7735-128x160-2](https://user-images.githubusercontent.com/6020549/101023297-ec9d0c00-35b5-11eb-924b-a4aefc0dc9f2.jpg)
![screen_1-8_128x160-12](https://user-images.githubusercontent.com/6020549/59007829-21b28c00-8863-11e9-945d-91f1c2fa2d14.JPG)
![screen_1-8_128x160-13](https://user-images.githubusercontent.com/6020549/77221889-4cbb6180-6b91-11ea-9e3f-97d9ddafb82d.JPG)

---

# 1.8" ST7735 128x160

![config-st7735-128x160-3](https://user-images.githubusercontent.com/6020549/101023341-f6267400-35b5-11eb-8835-7ea3278ba6eb.jpg)
![screen_1-8_128x160-21](https://user-images.githubusercontent.com/6020549/59007852-3858e300-8863-11e9-8f03-ce4ae9eb652b.JPG)
![screen_1-8_128x160-31](https://user-images.githubusercontent.com/6020549/59970608-b9271700-95a5-11e9-94f8-062740fe135c.JPG)

---

# 1.77" ST7735 128x160

![config-st7735-128x160-4](https://user-images.githubusercontent.com/6020549/101023366-01799f80-35b6-11eb-8608-96bab43d909f.jpg)

Left:1.77" Right:1.8"
![screen_1-77_128x160](https://user-images.githubusercontent.com/6020549/59970576-bd066980-95a4-11e9-9f4a-88d69733f034.JPG)

---

# 1.44" ST7735 128x160

![config-st7735-128x160-5](https://user-images.githubusercontent.com/6020549/101023395-0a6a7100-35b6-11eb-887c-3ebfc93bb0eb.jpg)

Left:1.44" Right:2.0"
![screen_1-8_128x160-2](https://user-images.githubusercontent.com/6020549/59007892-63433700-8863-11e9-86a7-1b92e8b15efd.JPG)

---

# 1.44" ST7735 128x128
![config-st7735-128x128](https://user-images.githubusercontent.com/6020549/101023420-16eec980-35b6-11eb-9c75-ca90a8b7f99a.jpg)
![screen_1-44_128x128-12](https://user-images.githubusercontent.com/6020549/59007915-7eae4200-8863-11e9-901f-037cbc0baed2.JPG)
![screen_1-44_128x128-13](https://user-images.githubusercontent.com/6020549/77222202-775ae980-6b94-11ea-8eed-0f6829833da8.JPG)

__GRAM Offset may be different__

---

# 0.96" ST7735 80x160

![config-st7735-80x160](https://user-images.githubusercontent.com/6020549/101023455-240bb880-35b6-11eb-9008-e69c59efb825.jpg)

Left:1.44" Right:0.96"
![screen_0-96_80x160-3](https://user-images.githubusercontent.com/6020549/59007940-9ede0100-8863-11e9-85ba-6eca86b6f441.JPG)

---

# JPEG Decoder   
The ESP-IDF component includes Tiny JPEG Decompressor.   
The document of Tiny JPEG Decompressor is [here](http://elm-chan.org/fsw/tjpgd/00index.html).   
This can reduce the image to 1/2 1/4 1/8.   

---

# PNG Decoder   
The ESP-IDF component includes part of the miniz library, such as mz_crc32.   
But it doesn't support all of the miniz.   
The document of miniz library is [here](https://github.com/richgel999/miniz).   

And I ported the pngle library from [here](https://github.com/kikuchan/pngle).   
This can reduce the image to any size.   

---

# Font File   
This project uses the following as default fonts:   
- font/ILGH16XB.FNT // 8x16Dot Gothic
- font/ILGH24XB.FNT // 12x24Dot Gothic
- font/ILGH32XB.FNT // 16x32Dot Gothic
- font/ILMH16XB.FNT // 8x16Dot Mincyo
- font/ILMH24XB.FNT // 12x24Dot Mincyo
- font/ILMH32XB.FNT // 16x32Dot Mincyo

From 0x00 to 0x7f, the characters image of Alphanumeric are stored.   
From 0x80 to 0xff, the characters image of Japanese are stored.   
Changing this file will change the font.

You can add your original fonts.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer [this](http://elm-chan.org/docs/dosv/fontx_e.html) page about FONTX format.   

---

# Font File Editor(FONTX Editor)   
[There](http://elm-chan.org/fsw/fontxedit.zip) is a font file editor.   
This can be done on Windows 10.   
Developer page is [here](http://elm-chan.org/fsw_e.html).   

![fontx-editor-1](https://github.com/user-attachments/assets/76a8c96f-74c3-4583-a4f1-5664f0e81f3a)

# Convert from BDF font to FONTX font   
step1)   
download Font File Editor(FONTX Editor) from [here](http://elm-chan.org/fsw_e.html).   

step2)   
download BDF font file from Internet.   
I downloaded from [here](https://github.com/fcambus/spleen).   
fontxedit.exe can __ONLY__ import Monospaced bitmap fonts file.   
Monospaced bitmap fonts can also be downloaded [here](https://github.com/Tecate/bitmap-fonts).

step3)   
import the BDF font file into your fontxedit.exe.   
this tool can convert from BDF to FONTX.   
![fontx-editor-2](https://github.com/user-attachments/assets/3353bf23-01f0-455d-8c9c-b56d55b4dc9c)

step4)   
adjust font size.   
![fontx-editor-3](https://github.com/user-attachments/assets/0a99fb0b-1725-472e-8310-ca57362ae6d1)

step5)   
check font pattern.   
when you have made any changes, press the apply button.   
![fontx-editor-4](https://github.com/user-attachments/assets/44b8ed95-0c3e-4507-87fa-b94c3c2349de)

step6)   
save as .fnt file from your fontedit.exe.   
![fontx-editor-5](https://github.com/user-attachments/assets/db5b62a8-3a61-49aa-8505-b906067f1111)

step7)   
upload your font file to $HOME/esp-idf-ili9340/font directory.   

step8)   
add font to use   
```
FontxFile fx32L[2];
InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot LATIN
```
Font file that From 0x00 to 0x7f, the characters image of Standard ASCII are stored.   
![M5Statck-Font-1](https://user-images.githubusercontent.com/6020549/132110943-98e8f481-8840-4e96-b649-f525aa427826.JPG)

Font file that From 0x80 to 0xff, the characters image of Japanese are stored.   
![M5Statck-Font-2](https://user-images.githubusercontent.com/6020549/132110947-d161678f-0424-4934-91b6-22eb12a57435.JPG)

Font file that From 0x80 to 0xff, the characters image of Latin are stored.   
![M5Statck-Font-3](https://user-images.githubusercontent.com/6020549/132110951-e7faaaa9-cdf2-429d-8eea-d20fe0748524.JPG)

u8g2 library contains many BDF fonts.   
This was converted from emoticons21.bdf.   
![bdf-font-1](https://github.com/user-attachments/assets/0c5470df-ab99-4b73-b3ac-624b6e6a5230)

This was converted from Scroll-o-Sprites.bdf.   
![bdf-font-2](https://github.com/user-attachments/assets/41dd693d-cb3d-4928-b213-49bf7cad4f6a)

# Convert from TTF font to FONTX font  
step1)   
Download WFONTX64.exe from [here](https://github.com/nemuisan/WFONTX64/releases).
Developer page is [here](https://github.com/nemuisan/WFONTX64).   

step2)   
Select ttf font.   
___Please note that if you select a proportional font, some fonts may not convert correctly.___   
If you select a proportional font, some fonts will need to be modified using fontxedit.exe.   
Monospaced fonts can be converted correctly.   
You can find Monospaced fonts [here](https://en.wikipedia.org/wiki/List_of_monospaced_typefaces).   
![WFONTX64-1](https://github.com/user-attachments/assets/2193a3c4-021c-48e6-8486-2ce500bdac36)

step3)   
Enter Height, Width, FontX2 name.   
Specify half of Height for Width.   
Specify your favorite font name in the FontX2 name field using up to 8 characters.   
![WFONTX64-2](https://github.com/user-attachments/assets/c87a9ec9-8e28-4d34-8475-60b15a47fb22)

step4)   
Specify the file name to save.   
![WFONTX64-3](https://github.com/user-attachments/assets/9715d4bf-e460-41a6-9a4b-38c0f10020f7)

step5)   
Specify the font style as required.   
![WFONTX64-4](https://github.com/user-attachments/assets/0ff3072d-6a78-48ae-b855-60c692f8d771)

step6)   
Press the RUN button to convert TTF fonts to FONTX format.   
![WFONTX64-5](https://github.com/user-attachments/assets/d9797e3d-1fd6-4504-b161-c1280f1242c0)

step7)   
upload your font file to $HOME/esp-idf-ili9340/font directory.   

step8)   
add font to use   
```
    FontxFile fx16G[2];
    FontxFile fx24G[2];
    FontxFile fx32G[2];
    //InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
    //InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
    //InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
    InitFontx(fx16G,"/spiffs/Gigi16.FNT",""); // 8x16Dot Gigi
    InitFontx(fx24G,"/spiffs/Gigi24.FNT",""); // 12x24Dot Gigi
    InitFontx(fx32G,"/spiffs/Gigi32.FNT",""); // 16x32Dot Gigi

```

![ttf_font](https://github.com/user-attachments/assets/9f01959b-b083-4a67-806f-86909630ad32)

# How to add your color   
Change here.   
```
#define RED    rgb565(255,   0,   0) // 0xf800
#define GREEN  rgb565(  0, 255,   0) // 0x07e0
#define BLUE   rgb565(  0,   0, 255) // 0x001f
#define BLACK  rgb565(  0,   0,   0) // 0x0000
#define WHITE  rgb565(255, 255, 255) // 0xffff
#define GRAY   rgb565(128, 128, 128) // 0x8410
#define YELLOW rgb565(255, 255,   0) // 0xFFE0
#define CYAN   rgb565(  0, 156, 209) // 0x04FA
#define PURPLE rgb565(128,   0, 128) // 0x8010
```

---

# XPT2046 Touch Screen   
A library of XPT2046 Touch Screen is included in this project.   
There is a TFT equipped with XPT2046.   
![XPT2046-3](https://user-images.githubusercontent.com/6020549/144333924-5236bff3-3f4d-4be4-8e99-b6e31878e4f3.jpg)

XPT2046 shares the TFT and SPI bus.   
Use the menu to enable XPT2046.   
![config-xpt2046-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/61323301-4b6a-4012-84f8-ee95dadb2db0)

- Touch position accuacy   
The coordinates read from XPT2046 are physical coordinates.   
Physical coordinates are converted to logical coordinates.   
Then draw using logical coordinates.   
In TouchPenTest, when you touch the screen, a circle is drawn at the touched position, but if you touch the same position as the previous time, it is not drawn.   
This value is the threshold that determines whether the touch location is the same as the previous touch location.   
Decreasing this value will make the position more accurate, but less responsive.   
Increasing this value will make the position more inaccurate but more responsive.   


### HR2046 Chip   
There is a TFT equipped with HR2046.   
XPT2046 and HR2046 are very similar. But HR2046 does not work properly.   
![XPT2046-2](https://user-images.githubusercontent.com/6020549/144332571-717f33b1-df03-4a0a-9a23-c7c99b9d4d32.JPG)


### Wirering for XPT2046  

|TFT||ESP32|ESP32-S2/S3|ESP32-C2/C3||
|:-:|:-:|:-:|:-:|:-:|:-:|
|VCC|--|3.3V|3.3V|3V3||
|GND|--|GND|GND|GND||
|CS|--|GPIO14|GPIO34|GPIO2||
|RES|--|GPIO33|GPIO41|GPIO4|(*1)|
|D/C|--|GPIO27|GPIO40|GPIO3|(*1)|
|MOSI|--|GPIO23|GPIO35|GPIO0|(*1) (*2)|
|SCK|--|GPIO18|GPIO36|GPIO1|(*1) (*2)|
|LED|--|3.3V|3.3V|3.3V|(*1) (*3)|
|MISO|--|N/C|N/C|N/C||
|T_CLK|--|GPIO18|GPIO36|GPIO1|(*1) (*2)|
|T_CS|--|GPIO21|GPIO38|GPIO7|(*1) (*4)|
|T_DIN|--|GPIO23|GPIO35|GPIO0|(*1) (*2)|
|T_OUT|--|GPIO19|GPIO37|GPIO6|(*1) (*4)|
|T_IRQ|--|GPIO22|GPIO39|GPIO8|(*1) (*4)|

(*1) You can change it to any gpio using menuconfig. But some gpio's are input only.   

(*2) These are shared by TFT and XPT2046.   

(*3) It can be controlled using gpio. However, GPIO of ESP32 cannot supply too much current. TFT backlight becomes brighter when powered by an external power source.   

(*4) I found that there are limits to the GPIOs that can be used as touch panel controls.   

### Check if XPT2046 works properly   
You can check if XPT2046 works properly.   
![config-xpt2046-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/4fe1f528-c443-4e93-a22f-e85f9f02ef84)

If you touch it at this time, the touched coordinates will be displayed.   
If there is no touch for 10 seconds, it will end.   
![TouchPosition-1](https://user-images.githubusercontent.com/6020549/147400381-cdd61863-c619-45e0-95ee-2aba593b22da.jpg)

Move the touch-pen vertically and horizontally to check the X and Y coordinates.   
What you get here is the physical coordinates.   
See [here](https://github.com/nopnop2002/esp-idf-ili9340/issues/39) about physical coordinates.   
![TouchPosition-2](https://user-images.githubusercontent.com/6020549/147400385-20b035b4-653a-4605-9d36-6d325a1f68a3.jpg)

### Using ESP32-3248S035   
This module also has an XPT2046.   
![ESP32-3248S035-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/56d7bc3b-ab03-4486-bd51-bc16d6138d1c)
![ESP32-3248S035-3](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/bed91135-fd74-4789-949e-f9c8398b565f)

XPT2046 uses the same SPI bus as TFT.   
XPT2046's SCLK and MOSI use the same GPIO as the TFT.   
![config-ESP32-3248S035-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/7388f072-8e4f-43c0-b8c4-545bf774f0db)

### Using ESP32-2432S028R   
This module also has an XPT2046.   
![ESP32-2432S028R-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/af351771-6c08-4949-b541-1c9d35e97c8e)
![ESP32-2432S028R-3](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/4cdd0bed-a248-4a69-83aa-5c44e2f2e02d)

XPT2046 uses a different SPI bus than TFT.   
XPT2046's SCLK and MOSI use separate GPIOs from the TFT.   
![config-ESP32-2432S028R-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/65679e0c-c08f-4dca-bc2a-13f9f52dadd4)

### Calibration   
Keep touching the point.   
![XPT2046-1](https://user-images.githubusercontent.com/6020549/145127021-6f311ab1-0def-4577-9212-39b3848756ae.JPG)
![XPT2046-2](https://user-images.githubusercontent.com/6020549/145127026-be6496bc-90d6-48f7-8a0e-da7c1a7ac618.JPG)

### Draw with touch   
If there is no touch for 10 seconds, it will end.   
![TouchPen-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/83c25554-aa45-49cd-b8d7-6eda9303b31d)

### Button with touch   
You can only enter up to 15 characters.   
If there is no touch for 10 seconds, it will end.   
![TouchKeyboard](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/fdba50c7-e349-4fba-8b70-5d50a96aa708)

### Move with touch   
If there is no touch for 10 seconds, it will end.   
![TouchMove-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/d6cd524d-b576-4109-bcf7-98e26f36f99c)
![TouchMove-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/7be77525-9f61-4deb-9025-f502ab6df76a)

### Menu with Touch   
If there is no touch for 10 seconds, it will end.   
![TouchMenu-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/b46be78f-8a92-4725-b209-ee95775e2e25)
![TouchMenu-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/0c5f153f-3d83-4f3e-abde-124c7a275d65)

### Select with touch   
If there is no touch for 10 seconds, it will end.   
I borrowed the icon from [here](https://www.flaticon.com/packs/social-media-343).   
![TouchIcon-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/e0aa7955-adad-4b33-93aa-a283585e2cf1)
![TouchIcon-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/e909df95-1a02-4fa6-a973-b6baf24c9c20)

### Save calibration data to NVS   

Write calibration data to NVS.   
Read calibration data from NVS when starting the firmware and use it.   
If you use the same TFT, you don't need to calibrate again.   
To clear the calibration data recorded in NVS, execute the following command.   
```
idf.py erase_flash
```

![config-xpt2046-4](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/27bc35da-db6a-460d-ba8a-311d4bb72637)

---

# SPI Clock speed   
According to the datasheet, the minimum SPI clock cycles for each driver are as follows:   
Maximum SPI clock frequency is the reciprocal of this.   
|Driver|minimum SPI clock cycle|maximum SPI clock frequency|
|:-:|:-:|:-:|
|ILI9225|100ns|10MHz|
|ILI9340|66ns|15MHz|
|ILI9341|100ns|10MHz|
|ST7735|66ns|15MHz|
|ST7796|66ns|15MHz|

The SPI clock frequency used by this project is 10MHz.   
Higher SPI clock frequencies can be specified using ```spi_clock_speed()```.   
When using higher SPI clock frequencies, you need to be careful about the length of the wire cable.   
```
    int speed = 15000000; // 15MHz
    spi_clock_speed(speed);
    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_TFT_CS_GPIO, CONFIG_DC_GPIO,
        CONFIG_RESET_GPIO, CONFIG_BL_GPIO, XPT_MISO_GPIO, XPT_CS_GPIO, XPT_IRQ_GPIO, XPT_SCLK_GPIO, XPT_MOSI_GPIO);
    lcdInit(&dev, model, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);
```

# SPI BUS selection   
![config-spi-bus](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/bdfd8436-31c7-435f-b21d-3d4b8e5350d3)

The ESP32 series has three SPI BUSs.   
SPI1_HOST is used for communication with Flash memory.   
You can use SPI2_HOST and SPI3_HOST freely.   
When you use SDSPI(SD Card via SPI), SDSPI uses SPI2_HOST BUS.   
When using this module at the same time as SDSPI or other SPI device using SPI2_HOST, it needs to be changed to SPI3_HOST.   
When you don't use SDSPI, both SPI2_HOST and SPI3_HOST will work.   
Previously it was called HSPI_HOST / VSPI_HOST, but now it is called SPI2_HOST / SPI3_HOST.   

---

# Using SPI TFT Adapter   

I purchased this adapter on AliExpress.   
It comes with an ESP32 and costs $4.   
![14pin-adapter-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/f73208ee-2c41-4541-bffd-e731e14cbff3)
![14pin-adapter-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/a989ec43-41f4-4929-9352-6d148f1d6e60)

However, this adapter's T_IRQ is not connected anywhere.   
To use the touch panel, you need to add a jumper.   
![14pin-adapter-3](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/fba70ca5-1ab1-4f0c-8962-1b4efb195b35)

As a result, the GPIO will be:   
|TFT||ESP32||
|:-:|:-:|:-:|:-:|
|VCC|--|3.3V||
|GND|--|GND||
|CS|--|GPIO15||
|RES|--|GPIO04||
|D/C|--|GPIO02||
|MOSI|--|GPIO23||
|SCK|--|GPIO18||
|LED|--|3.3V|*1|
|MISO|--|N/C||
|T_CLK|--|GPIO18||
|T_CS|--|GPIO05||
|T_DIN|--|GPIO23||
|T_OUT|--|GPIO19||
|T_IRQ|--|GPIO21||

(*1) BL_EN is directly connected to 3.3V, so it cannot control BackLight.   

When not using the touch panel.   
![config-14pin-adapter-1](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/b9d4d2a1-5e58-439f-b2ef-69a85e1810ae)

When using the touch panel.   
![config-14pin-adapter-2](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/5b7664e2-c1c1-4cfa-9979-59856ea37d42)

![14pin-adapter-4](https://github.com/nopnop2002/esp-idf-ili9340/assets/6020549/9a098ae6-aa09-4ee5-9568-18f06260e4a9)


---

# Reference   
A TFT shield like this can be used with ESP-IDF.   
![TFT-Shield](https://user-images.githubusercontent.com/6020549/104253960-10a71380-54b9-11eb-8789-a12c2c769ab4.JPG)

It can be attached directly to this ESP32.   
![tzt-d1-esp32s3](https://github.com/nopnop2002/esp-idf-parallel-tft/assets/6020549/ba6772f1-c8b7-4e7d-9d0a-0fb8fb8cbea5)

https://github.com/nopnop2002/esp-idf-parallel-tft

