add_library(ppkeys_app
    app.c
    app_event.c
)

target_link_libraries(ppkeys_app
    PRIVATE
        user32
)
