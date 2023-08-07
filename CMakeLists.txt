cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0079 NEW)

option(HOST "" ON)

if (NOT HOST)
message("BUILD FOR PICO")
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)
endif()

project(arm2d_demo_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (NOT HOST)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})
pico_sdk_init()

endif()

if (HOST)
add_compile_options(
    -std=gnu11 -g
    -ffunction-sections 
    -fdata-sections
    -Ofast
    -flto)
else()
    add_compile_options(-Wall)
endif()

add_link_options(-flto)

add_executable(arm2d_demo  main.c
        arm_2d_scene_cmsis_stream.c
        controls/spectrum_display.c 
        controls/amplitude_display.c  
)

if (HOST)
    target_sources(arm2d_demo PUBLIC platform/Virtual_TFT_Port.c     
        platform/arm_2d_disp_adapter_0.c)
    target_compile_definitions(arm2d_demo PRIVATE _THREAD_SAFE SDL_DISABLE_ARM_NEON_H)
    target_include_directories(arm2d_demo PUBLIC . templates controls)
    add_subdirectory(${CMSISDSP}/Source bin_dsp)

    option(ARM2D_HOST "" ON)
    target_compile_definitions(arm2d_demo PUBLIC HOST)

endif()


target_include_directories(arm2d_demo PUBLIC . templates controls)



option(ARM2D_CONTROLS "" ON)
option(ARM2D_HELPER "" ON)
option(ARM2D_LCD_PRINTF "" ON)
option(ARM2D_BENCHMARK_GENERIC "" OFF)
option(ARM2D_BENCHMARK_WATCHPANEL "" OFF)
option(ARM2D_ALPHA_BLENDING "" ON)
option(ARM2D_TRANSFORM "" ON)
if (HOST)
add_subdirectory(${ARM2D} arm2d_demo_bin)
else()
add_subdirectory(${ARM2D_RP2040} arm2d_rp2040_bin)
endif()

if (HOST)
target_include_directories(ARM2D PUBLIC platform)

target_link_libraries(ARM2D PUBLIC CMSISDSP)
target_link_libraries(arm2d_demo PUBLIC ARM2D)

set(SDL2_INCLUDE_DIR /opt/homebrew/include)
set(SDL2_LIBRARY /opt/homebrew/lib)

target_include_directories(arm2d_demo PUBLIC ${SDL2_INCLUDE_DIR})
target_link_directories(arm2d_demo PUBLIC ${SDL2_LIBRARY})
target_link_libraries(arm2d_demo PUBLIC SDL2)
else()

    target_link_libraries(arm2d_demo 
    pico_stdlib 
    pico_multicore
    hardware_pio
    hardware_interp
    arm2d_rp2040
    )

    pico_enable_stdio_usb(arm2d_demo 1)
    pico_enable_stdio_uart(arm2d_demo 0)
    
# create map/bin/hex file etc.
pico_add_extra_outputs(arm2d_demo)



endif()
