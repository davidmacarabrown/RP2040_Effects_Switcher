cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

project(main)

# include_directories(<nameofdir>) to add directories
include_directories("global")
include_directories("output_manager")
include_directories("input_manager")
include_directories("state_manager")
include_directories("input_handler")

include_directories("storage_manager")
include_directories("display_manager")

include_directories("hardware/HT16K33")
include_directories("hardware/CAT24C32")
include_directories("hardware/MCP23017")
include_directories("hardware/WEH001602A")

include_directories("data_containers/MIDI")
include_directories("data_containers/patch_data")
include_directories("data_containers/system_data")

include_directories("core_0")
include_directories("core_1")

#include_directories(utilities/command_input)

#brings in a quick and easy debug function to put in as a "breakpoint" to test if a certain part of code is being reached, it will spin there forever blinking the built-in LED
include_directories(debug)

file(GLOB SOURCES
    *.h
    *.cpp
    */*.h
    */*.cpp
    */*/*.h
    */*/*.cpp
    )

# add_executable(<nameofdir> <mainfile> <anyotherfiles>)
add_executable(main main.cpp ${SOURCES})

target_link_libraries(main
    pico_stdlib
    pico_stdio_usb
    hardware_sync
    hardware_i2c
    pico_multicore
    pico_util
    )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
pico_add_extra_outputs(main)