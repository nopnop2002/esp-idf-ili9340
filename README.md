# esp-idf-ili9340
ILI9340 Driver for esp-idf

You have to set this config value with menuconfig.   
CONFIG_WIDTH   
CONFIG_HEIGHT   
CONFIG_OFFSETX   
CONFIG_OFFSETY   
CONFIG_CS_GPIO   
CONFIG_DC_GPIO   
CONFIG_RESET_GPIO   
CONFIG_BL_GPIO   

```
git clone https://github.com/nopnop2002/esp-idf-ili9340
cd esp-idf-ili9340/
make menuconfig
make flash
```

__MOSI is GPIO23.__   
__SCLK is GPIO18.__   
__MISO is not use.__   

---

# M5Stack

![config_M5Stick](https://user-images.githubusercontent.com/6020549/57977447-d7907480-7a33-11e9-9df6-f0882c113d37.jpg)

![M5Stick-1](https://user-images.githubusercontent.com/6020549/57977479-7d43e380-7a34-11e9-99b6-6028500436e8.JPG)
![M5Stick-2](https://user-images.githubusercontent.com/6020549/57977480-7d43e380-7a34-11e9-814e-f206c43e4a05.JPG)
![M5Stick-3](https://user-images.githubusercontent.com/6020549/57977481-7d43e380-7a34-11e9-9bc6-8e415aa52c52.JPG)
![M5Stick-4](https://user-images.githubusercontent.com/6020549/57977482-7d43e380-7a34-11e9-8188-653cb02f3ba0.JPG)
![M5Stick-5](https://user-images.githubusercontent.com/6020549/57977483-7ddc7a00-7a34-11e9-936e-4d97b1411610.JPG)
![M5Stick-6](https://user-images.githubusercontent.com/6020549/57977484-7ddc7a00-7a34-11e9-8750-52db073c96c2.JPG)
![M5Stick-7](https://user-images.githubusercontent.com/6020549/57977485-7e751080-7a34-11e9-95f9-ffb12879d1b0.JPG)
![M5Stick-8](https://user-images.githubusercontent.com/6020549/57977486-7e751080-7a34-11e9-9ac8-c546f248fdec.JPG)
![M5Stick-9](https://user-images.githubusercontent.com/6020549/57977487-7e751080-7a34-11e9-9a3e-6a0bd7359efb.JPG)
![M5Stick-10](https://user-images.githubusercontent.com/6020549/57977488-7e751080-7a34-11e9-9a12-e2b70334604d.JPG)
![M5Stick-11](https://user-images.githubusercontent.com/6020549/57977489-7f0da700-7a34-11e9-9ea3-c0420a785a3e.JPG)

---

# Generic ILI9340/9341

![config_ILI9140](https://user-images.githubusercontent.com/6020549/57977453-fd1d7e00-7a33-11e9-98f1-54cc2e54d5f0.jpg)

![ILI9140-1](https://user-images.githubusercontent.com/6020549/57977468-48379100-7a34-11e9-96f6-c155af9f19f0.JPG)

---

# GENERIC ST7735

![config_ST7735](https://user-images.githubusercontent.com/6020549/57977455-04dd2280-7a34-11e9-987d-8df3c52d8372.jpg)

![ST7735-1](https://user-images.githubusercontent.com/6020549/57977471-54235300-7a34-11e9-9d9b-e9ff330de9c2.JPG)

---

# GENERIC ILI9163C

![config_ILI9163C](https://user-images.githubusercontent.com/6020549/57977458-0e668a80-7a34-11e9-8061-5e9d76869545.jpg)

![ILI9163C-1](https://user-images.githubusercontent.com/6020549/57977472-62716f00-7a34-11e9-9514-07593661dd79.JPG)

---

# Font File   
You can add your original font file.   
The format of the font file is the FONTX format.   
Your font file is put in font directory.   
Your font file is uploaded to SPIFFS partition using meke flash.   

Please refer this page about FONTX format.   
http://elm-chan.org/docs/dosv/fontx_e.html

```
FontxFile yourFont[2];
InitFontx(yourFont,"/spiffs/your_font_file_name","");
lcdDrawString(&dev, yourFont, x, y, ascii, color);
```
