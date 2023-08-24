if (ESP_PLATFORM)
    idf_component_register(
        SRCS "aw88298.c"
        INCLUDE_DIRS "."
    )
    add_definitions("-DAW88298_INCLUDE_SDKCONFIG_H")
endif()
