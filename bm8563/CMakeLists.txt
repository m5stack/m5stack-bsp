idf_component_register(
    SRCS bm8563.c
    INCLUDE_DIRS .
    REQUIRES i2cdev log esp_idf_lib_helpers
)