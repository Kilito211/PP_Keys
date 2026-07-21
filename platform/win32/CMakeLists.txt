add_library(ppkeys_platform
    window.c
    message_loop.c
    tray.c
    win_util.c
    voice.c
    status_overlay.c
)

target_link_libraries(ppkeys_platform
    PRIVATE
        user32
        ole32
        shell32
)
