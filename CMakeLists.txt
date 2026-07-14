cmake_minimum_required(VERSION 3.20)

project(PPKeys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    # 消除C4819编码警告
    add_compile_options(/utf-8)
endif()

# 禁止CMake使用response file，MinGW对此特性支持有问题
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS OFF)

add_subdirectory(platform)
add_subdirectory(app)

add_executable(PPKeys
    main.c
)

target_link_libraries(PPKeys
    PRIVATE
        ppkeys_app
)