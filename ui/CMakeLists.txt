add_library(ppkeys_ui
    ui.c
    win32/ui_win32.c
)

target_link_libraries(ppkeys_ui
    PRIVATE
        user32
)
