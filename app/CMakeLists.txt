add_library(ppkeys_app
    app.c
    app_event.c
)

target_include_directories(ppkeys_app
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ppkeys_app
    PRIVATE
        ppkeys_platform
)