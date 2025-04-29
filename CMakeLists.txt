cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(main C CXX ASM)
pico_sdk_init()

add_executable(${PROJECT_NAME} main.c )

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_sources(${PROJECT_NAME} PRIVATE 
    main.c
    lib/push_button.c
    lib/ssd1306.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_clocks
    hardware_i2c
    hardware_adc
)
pico_add_extra_outputs(${PROJECT_NAME})