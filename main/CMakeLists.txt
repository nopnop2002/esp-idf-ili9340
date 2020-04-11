set(COMPONENT_SRCS main.c ili9340.c decode_image.c pngle.c fontx.c)
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
spiffs_create_partition_image(storage ../font FLASH_IN_PROJECT)
