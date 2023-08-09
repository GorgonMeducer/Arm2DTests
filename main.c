/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/
#include <stdio.h>

#ifdef HOST
#include "Virtual_TFT_Port.h"
#else
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/clocks.h"

#include "st7789_lcd.h"
#endif

#include "arm_2d_helper.h"
#include "arm_2d_scenes.h"
#include "arm_2d_disp_adapters.h"

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#include "arm_2d_scene_cmsis_stream.h"

#include "lcd_graph_config.h"
#include "scheduler.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

extern void test();

/*============================ IMPLEMENTATION ================================*/


void wait_for_display()
{
    while (arm_fsm_rt_cpl != disp_adapter0_task()) ;
}




void scene_cmsis_stream_loader(user_scene_cmsis_stream_t *scene) 
{
    arm_2d_scene_cmsis_stream_init(&DISP0_ADAPTER,scene);
}





/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{

    
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
void init_scenes ()
{
#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);
    
    //arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
    //                                        ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    //arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);
    
    //arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    
}

void cmsis_stream_scheduling()
{
    int error;
    uint32_t nbSched = 0;

    printf("Start\n\r");

    init_scheduler();
    nbSched=scheduler(&error);
    printf("Number of schedule iterations = %d\n\r",nbSched);
    printf("Error code = %d\n\r",error);
    free_scheduler();
}

#ifdef HOST
int app_2d_main_thread (void *argument)
{



    init_scenes();
    

    cmsis_stream_scheduling();

    return 0;
}

int main(int argc, char* argv[])
{

    VT_init();


    printf("\r\nArm-2D PC Template\r\n");

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();


    SDL_CreateThread(app_2d_main_thread, "arm-2d thread", NULL);

    while (1) {
        VT_sdl_refresh_task();
        if(VT_is_request_quit()){
            break;
        }
    }

    VT_deinit();
    return 0;
}
#else 
int main(int argc, char* argv[])
{
    stdio_init_all();
    sleep_ms(2000);

    start_lcd();

    printf("\r\nArm-2D PC Template\r\n");
    printf("%d\n",sizeof(arm_2d_tile_t));

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    init_scenes();


   cmsis_stream_scheduling();


    return 0;
}
#endif 

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


