# esp-idf-ili9340
SPI TFT Driver for esp-idf

You have to set this config value with menuconfig.   
- CONFIG_WIDTH   
- CONFIG_HEIGHT   
- CONFIG_OFFSETX   
- CONFIG_OFFSETY   
- CONFIG_CS_GPIO   
- CONFIG_DC_GPIO   
- CONFIG_RESET_GPIO   
- CONFIG_BL_GPIO   

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
make menuconfig
make flash
```

![config](https://user-images.githubusercontent.com/6020549/59007960-ba490c00-8863-11e9-8df9-457262ac0e43.jpg)

__MOSI is GPIO23.__   
__SCLK is GPIO18.__   
__MISO is not use.__   

---

# Tested TFT

M5Stack   
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

![config_M5Stack](https://user-images.githubusercontent.com/6020549/59007566-f4191300-8861-11e9-95f3-88156964707f.jpg)

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
![M5Stack-BMP](https://user-images.githubusercontent.com/6020549/78413964-dcaee000-7654-11ea-88f2-e70662d761e1.JPG)
![M5Stack-JPEG](https://user-images.githubusercontent.com/6020549/78413968-e0426700-7654-11ea-9040-0fdfd0f2de2e.JPG)


---

# 2.8" ILI9341 320x240
# 2.4" ILI9341 320x240

![config_2-8_240x320](https://user-images.githubusercontent.com/6020549/59007585-072be300-8862-11e9-97bb-90d538167c0b.jpg)

Left:2.8" Right:2.4"
![screen_2-8_240x320](https://user-images.githubusercontent.com/6020549/59007633-3f332600-8862-11e9-8d8f-bbfb303456f4.JPG)

---

# 2.2" ILI9340 320x240

![config_2-2_240x320](https://user-images.githubusercontent.com/6020549/59007751-ce403e00-8862-11e9-9472-6d7783d3aedd.jpg)

Left:2.2" Right:2.4"
![screen_2-2_240x320](https://user-images.githubusercontent.com/6020549/59007758-d304f200-8862-11e9-8d61-fa4b734f4b9a.JPG)

---

# 2.0" ILI9225 176x220

![config_2-0_176x220](https://user-images.githubusercontent.com/6020549/59140156-06678e00-89d5-11e9-9b82-1077b2e61c65.jpg)

Left:2.2" Right:2.0"
![screen_2-0_176x220](https://user-images.githubusercontent.com/6020549/59007713-a05af980-8862-11e9-8ea2-d60b84ea49bf.JPG)

---

# 2.0" ILI9225G 176x220

![config_2-0_176x220-11](https://user-images.githubusercontent.com/6020549/60471069-f2ded880-9c9d-11e9-9cef-5253a017f305.jpg)

Left:2.0" 9225G Right:2.0" 9225
![screen_2-0_176x220-11](https://user-images.githubusercontent.com/6020549/60471071-f70af600-9c9d-11e9-8f5d-4d2b64fa0e09.JPG)

---

# 1.8" ST7735 128x160

![config_1-8_128x160-1](https://user-images.githubusercontent.com/6020549/59007797-fdef4600-8862-11e9-9048-6628e56a6fe8.jpg)

Left:1.8" Right:2.0"
![screen_1-8_128x160-2](https://user-images.githubusercontent.com/6020549/59007803-0778ae00-8863-11e9-9a5c-c217a1bfab66.JPG)

---

# 1.8" ST7735 128x160

![config_1-8_128x160-11](https://user-images.githubusercontent.com/6020549/59007826-1cedd800-8863-11e9-974d-d4b79abf8083.jpg)
![screen_1-8_128x160-12](https://user-images.githubusercontent.com/6020549/59007829-21b28c00-8863-11e9-945d-91f1c2fa2d14.JPG)
![screen_1-8_128x160-13](https://user-images.githubusercontent.com/6020549/77221889-4cbb6180-6b91-11ea-9e3f-97d9ddafb82d.JPG)

---

# 1.8" ST7735 128x160

![config_1-8_128x160-21](https://user-images.githubusercontent.com/6020549/59007843-32630200-8863-11e9-9801-b8d19b0546d5.jpg)
![screen_1-8_128x160-21](https://user-images.githubusercontent.com/6020549/59007852-3858e300-8863-11e9-8f03-ce4ae9eb652b.JPG)
![screen_1-8_128x160-31](https://user-images.githubusercontent.com/6020549/59970608-b9271700-95a5-11e9-94f8-062740fe135c.JPG)

---

# 1.77" ST7735 128x160

![config_1-77_128x160](https://user-images.githubusercontent.com/6020549/59970575-bb3ca600-95a4-11e9-8d8c-82f7feb33332.jpg)

Left:1.77" Right:1.8"
![screen_1-77_128x160](https://user-images.githubusercontent.com/6020549/59970576-bd066980-95a4-11e9-9f4a-88d69733f034.JPG)

---

# 1.44" ST7735 128x128

![config_1-8_128x160-1](https://user-images.githubusercontent.com/6020549/59007886-5aeafc00-8863-11e9-98f3-9b39967ee58d.jpg)

Left:1.44" Right:2.0"
![screen_1-8_128x160-2](https://user-images.githubusercontent.com/6020549/59007892-63433700-8863-11e9-86a7-1b92e8b15efd.JPG)

---

# 1.44" ST7735 128x128

![config_1-44_128x128-11](https://user-images.githubusercontent.com/6020549/59007912-7a822480-8863-11e9-92ce-f2ca6f69b4d3.jpg)
![screen_1-44_128x128-12](https://user-images.githubusercontent.com/6020549/59007915-7eae4200-8863-11e9-901f-037cbc0baed2.JPG)
![screen_1-44_128x128-13](https://user-images.githubusercontent.com/6020549/77222202-775ae980-6b94-11ea-8eed-0f6829833da8.JPG)

---

# 0.96" ST7735 80x160

![config_0-96_80x160-1](https://user-images.githubusercontent.com/6020549/59007930-95549900-8863-11e9-8a02-ba2224fe2a96.jpg)

Left:1.44" Right:0.96"
![screen_0-96_80x160-3](https://user-images.githubusercontent.com/6020549/59007940-9ede0100-8863-11e9-85ba-6eca86b6f441.JPG)

---

# JPEG Decorder   
I ported from [here](https://github.com/espressif/esp-idf/blob/master/examples/peripherals/spi_master/lcd/main/decode_image.c). 

Does not support image reduction.   

---

# Font File   
You can add your original font file.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer [this](http://elm-chan.org/docs/dosv/fontx_e.html) page about FONTX format.   

```
FontxFile yourFont[2];
InitFontx(yourFont,"/spiffs/your_font_file_name","");
uint8_t ascii[10];
strcpy((char *)ascii, "MyFont");
uint16_t color = RED;
lcdDrawString(&dev, yourFont, x, y, ascii, color);
```
