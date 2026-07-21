add_library(ppkeys_config
    config.c
    ini.c
)

target_include_directories(ppkeys_config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
