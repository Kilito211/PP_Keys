add_library(ppkeys_platform
    window.c
    message_loop.c
    ui_win32.c
)

target_include_directories(ppkeys_platform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ppkeys_platform
    PRIVATE
        user32
        ppkeys_core
)