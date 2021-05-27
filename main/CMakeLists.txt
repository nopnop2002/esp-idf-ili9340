set(srcs "main.c"
    "ili9340"
    "fontx.c"
    )

# Only ESP32 has enough memory to do jpeg decoding
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs "decode_image.c" "pngle.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
