add_library(ppkeys_core
    hotkey.c
    keyboard.c
    action_list.c
    macro_engine.c
    state_machine.c
)

target_include_directories(ppkeys_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ppkeys_core
    PRIVATE
        ppkeys_app
)
