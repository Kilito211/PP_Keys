cmake_minimum_required(VERSION 3.20)

project(PPKeys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

if(MINGW)
    set(CMAKE_RC_FLAGS "-I ${CMAKE_SOURCE_DIR}/resource")
endif()

set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS OFF)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/app/include
    ${PROJECT_SOURCE_DIR}/core/include
    ${PROJECT_SOURCE_DIR}/platform/win32/include
    ${PROJECT_SOURCE_DIR}/ui/include
    ${PROJECT_SOURCE_DIR}/resource
    ${PROJECT_SOURCE_DIR}/config/include
)

add_subdirectory(core)
add_subdirectory(config)
add_subdirectory(platform)
add_subdirectory(ui)
add_subdirectory(app)

add_executable(PPKeys WIN32
    main.c
)

set(APP_RESOURCE
    ${CMAKE_SOURCE_DIR}/resource/app.rc
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${APP_RESOURCE}
)

target_link_libraries(PPKeys
    PRIVATE
        ppkeys_app
        ppkeys_platform
        ppkeys_core
        ppkeys_ui
        ppkeys_config
)
