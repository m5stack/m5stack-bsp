if (ESP_PLATFORM)
    idf_component_register(
        SRCS "axp2101.c"
        INCLUDE_DIRS "."
    )
    add_definitions("-DAXP2101_INCLUDE_SDKCONFIG_H")
endif()
