# esp-idf-ili9340
SPI TFT Driver for esp-idf

# Installation for ESP32

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
idf.py set-target esp32
idf.py menuconfig
idf.py flash
```

# Installation for ESP32-S2

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
idf.py set-target esp32s2
idf.py menuconfig
idf.py flash
```
__Note__   
tjpgd library does not exist in ESP32-S2 ROM.   
Therefore, the JPEG file cannot be displayed.   


# Installation for ESP32-C3

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
idf.py set-target esp32c3
idf.py menuconfig
idf.py flash
```

__Note__   
For some reason GPIO06, GPIO08, GPIO09, GPIO19 cannot be used for SPI clock pins on my board.   
I looked at the ESP32-C3 datasheet, but I don't understand why.   
This may be a problem only for my board.   


# ESP-IDF component version   
Standalone ESP-IDF component version is [here](https://github.com/JarrettR/esp-idf-ili9340).


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
 __The TFT backlight is more stable when supplied from an external power source.__   
 __TFT MISO is not use.__   

![config-menu](https://user-images.githubusercontent.com/6020549/101022538-db9fcb00-35b4-11eb-95a9-ad84a880c7fb.jpg)

---

# Tested TFT

M5Stack   
4.0" ST7796S 480x320   
3.2" ILI9341 320x240   
2.8" ILI9341 320x240   
2.4" ILI9341 320x240   
2.2" ILI9340 320x240   
2.0" ILI9225 176x220   
2.0" ILI9225G 176x220   
1.8" ST7735 128x160   
1.77" ST7735 128x160   
1.44" ST7735 128x128   
0.96" ST7735 80x160   

![spi-tft](https://user-images.githubusercontent.com/6020549/59512687-3098de80-8ef4-11e9-9759-530ffe47d659.JPG)

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

---

# 4.0" ST7796S 480x320

![config-st7796](https://user-images.githubusercontent.com/6020549/101022680-0e49c380-35b5-11eb-914f-f522b33bb49a.jpg)

Left:4.0" Right:2.4"
![4_0_st779s](https://user-images.githubusercontent.com/6020549/85913118-e7040300-b86c-11ea-87fd-fdcd97c5bf26.JPG)

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

![FontxEditor](https://user-images.githubusercontent.com/6020549/78731275-3b889800-797a-11ea-81ba-096dbf07c4b8.png)

This library uses the following as default fonts:   
- font/ILGH16XB.FNT // 8x16Dot Gothic
- font/ILGH24XB.FNT // 12x24Dot Gothic
- font/ILGH32XB.FNT // 16x32Dot Gothic
- font/ILMH16XB.FNT // 8x16Dot Mincyo
- font/ILMH24XB.FNT // 12x24Dot Mincyo
- font/ILMH32XB.FNT // 16x32Dot Mincyo

From 0x00 to 0x7f, the characters image of Alphanumeric are stored.   
From 0x80 to 0xff, the characters image of Japanese are stored.   
Changing this file will change the font.

# How to build your own font file   
step1)   
download fontxedit.exe.   

step2)   
download BDF font file from Internet.   
I downloaded from [here](https://github.com/fcambus/spleen).   
fontxedit.exe can __ONLY__ import Monospaced bitmap fonts file.   
Monospaced bitmap fonts can also be downloaded [here](https://github.com/Tecate/bitmap-fonts).

step3)   
import the BDF font file into your fontxedit.exe.   
this tool can convert from BDF to FONTX.   
![FONTX-EDITTOR-1](https://user-images.githubusercontent.com/6020549/112736427-d7e5e900-8f95-11eb-80d5-11dd9df42903.jpg)

step4)   
adjust font size.   
![FONTX-EDITTOR-2](https://user-images.githubusercontent.com/6020549/112736434-e6cc9b80-8f95-11eb-8b8e-b523746c1c96.jpg)

step5)   
check font pattern.   
![FONTX-EDITTOR-13](https://user-images.githubusercontent.com/6020549/112746492-11e0da80-8fea-11eb-94f1-8d299b2dc756.jpg)

step6)   
save as .fnt file from your fontedit.exe.   
![FONTX-EDITTOR-14](https://user-images.githubusercontent.com/6020549/112746501-2329e700-8fea-11eb-9a3a-4481c1a14ddc.jpg)

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

---

# Reference   
You can use TFT Shield like this:   
![TFT-Shield](https://user-images.githubusercontent.com/6020549/104253960-10a71380-54b9-11eb-8789-a12c2c769ab4.JPG)

https://github.com/nopnop2002/esp-idf-parallel-tft

