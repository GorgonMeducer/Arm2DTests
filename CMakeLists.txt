cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0079 NEW)

project(arm2d_demo_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_compile_options(
    -std=gnu11 -g
    -ffunction-sections 
    -fdata-sections
    -Ofast
    -flto)

add_link_options(-flto)


add_executable(arm2d_demo  main.c
        arm_2d_scene_cmsis_stream.c
        platform/Virtual_TFT_Port.c     
        platform/arm_2d_disp_adapter_0.c
        controls/spectrum_display.c 
        controls/amplitude_display.c  
)

target_compile_definitions(arm2d_demo PRIVATE _THREAD_SAFE SDL_DISABLE_ARM_NEON_H)

target_include_directories(arm2d_demo PUBLIC . platform templates controls)

option(HOST "" ON)
add_subdirectory(${DSP}/Source bin_dsp)


option(ARM2D_HOST "" ON)
option(ARM2D_CONTROLS "" ON)
option(ARM2D_HELPER "" ON)
option(ARM2D_LCD_PRINTF "" ON)
option(ARM2D_BENCHMARK_GENERIC "" OFF)
option(ARM2D_BENCHMARK_WATCHPANEL "" OFF)
option(ARM2D_ALPHA_BLENDING "" ON)
option(ARM2D_TRANSFORM "" ON)
add_subdirectory(../Arm-2D arm2d_demo_bin)
target_include_directories(ARM2D PUBLIC platform)

target_link_libraries(ARM2D PUBLIC CMSISDSP)
target_link_libraries(arm2d_demo PUBLIC ARM2D)

set(SDL2_INCLUDE_DIR /opt/homebrew/include)
set(SDL2_LIBRARY /opt/homebrew/lib)

target_include_directories(arm2d_demo PUBLIC ${SDL2_INCLUDE_DIR})
target_link_directories(arm2d_demo PUBLIC ${SDL2_LIBRARY})
target_link_libraries(arm2d_demo PUBLIC SDL2)

