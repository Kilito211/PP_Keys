add_library(ppkeys_core
    hotkey.c
    keyboard.c
    action_list.c
    macro_engine.c
    state_machine.c
    single_instance.c
    interception_wrapper.c
)

target_link_libraries(ppkeys_core
    PRIVATE
        ppkeys_platform
        ppkeys_ui
)
