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

#define NB_SAMPLES 256 
#define NB_FFT 256

const q15_t samples[NB_SAMPLES]={0, 5880, 11148, 14557, 15551, 12938, 7604, 1082, -4957, -8462, -8246, -4159, 2832, 11350, 19344, 25186, 26672, 25042, 20802, 14436, 8997, 5870, 2533, 6496, 9387, 12694, 17171, 19751, 22417, 22377, 21470, 18885, 16383, 14818, 13497, 12465, 12955, 13773, 14973, 16578, 18190, 20367, 19145, 21667, 17084, 12560, 8550, 94, -1327, 3152, 4536, 8235, 15130, 18731, 20069, 22447, 17417, 10119, 1782, -5472, -11581, -12624, -10419, -6335, -1, 4379, 11625, 11197, 8872, 4984, -3692, -8762, -9138, -12017, -17138, -12781, -10373, -9288, -8558, -7625, -8253, -8950, -10356, -12805, -15044, -17894, -17696, -21111, -19107, -18565, -15028, -9887, -2585, -11362, -3458, -8830, -16384, -23353, -25837, -32509, -31053, -23838, -17788, -11473, -6239, 164, -51, -3796, -8244, -14733, -20903, -22389, -18274, -25087, -15841, -11366, -3446, -2627, -3743, 2653, -242, -4568, -5432, -7091, -10413, -7170, -5146, -4877, -1, 4152, 10623, 8572, 14857, 12699, 7339, 3661, -774, -2286, 4217, -3098, 5421, 10150, 13925, 14256, 26779, 19618, 19757, 13952, 10606, 5961, 6187, 4610, 5479, 10669, 17861, 26228, 31185, 22198, 28899, 19524, 16383, 10149, 2701, 7322, 12074, 13330, 14404, 19710, 26033, 18357, 25953, 18755, 17352, 12654, 7520, 4827, -975, 1210, 8291, 9436, 12569, 13301, 23764, 20351, 17718, 8129, 2960, 686, -9865, -6621, -3424, -6684, -1, 3971, 10580, 5628, 5131, 6410, -2488, -7114, -15574, -12642, -15904, -16439, -13273, -8618, -2103, -7660, 1194, 1435, -10686, -12681, -15992, -17349, -22320, -30464, -17904, -17131, -12513, -7734, -10504, -11000, -12327, -13929, -16384, -20781, -29921, -19991, -24396, -18147, -18465, -14368, -5828, -2586, -10052, -11207, -11906, -14746, -18242, -26581, -21172, -25062, -15056, -10126, -5811, 1114, 7052, 1797, 657, -4280, -6272, -14375, -11252, -17919, -14281, -2424};
q15_t fftAmp[NB_FFT>>1]={265, 316, 16601, 236, 227, 91, 63, 145, 199, 240, 99, 384, 84, 715, 1231, 1107, 1023, 821, 351, 473, 10142, 199, 195, 405, 418, 1464, 1313, 333, 213, 671, 115, 191, 207, 49, 219, 142, 40, 102, 258, 170, 113, 230, 225, 148, 277, 128, 368, 88, 153, 224, 202, 188, 406, 705, 105, 155, 470, 363, 151, 360, 34, 186, 47, 121, 167, 294, 394, 657, 524, 422, 224, 264, 403, 300, 221, 178, 132, 224, 575, 184, 182, 344, 118, 467, 77, 196, 186, 356, 337, 418, 194, 248, 316, 361, 270, 443, 228, 233, 426, 138, 414, 269, 148, 124, 36, 510, 590, 465, 398, 360, 155, 288, 338, 154, 92, 123, 249, 144, 447, 121, 330, 404, 96, 328, 17, 283, 249, 450};
/*============================ IMPLEMENTATION ================================*/


user_scene_cmsis_stream_t *streamScene;

user_scene_cmsis_stream_t *scene_cmsis_stream_loader(void) 
{
    return(arm_2d_scene_cmsis_stream_init(NB_FFT,NB_SAMPLES,&DISP0_ADAPTER));
}

typedef user_scene_cmsis_stream_t *scene_loader_t(void);

static scene_loader_t * const c_SceneLoaders[] = {
    scene_cmsis_stream_loader
};

static user_scene_cmsis_stream_t *currentScene=NULL;

/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{
    static uint_fast8_t s_chIndex = 0;

    if (s_chIndex >= dimof(c_SceneLoaders)) {
        s_chIndex = 0;
    }
    
    /* call loader */
    currentScene=c_SceneLoaders[s_chIndex]();
    s_chIndex++;
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
    
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_FADE_WHITE);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 3000);
    
    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif

    
}

#ifdef HOST
int app_2d_main_thread (void *argument)
{


    int px=0;
    int d=2;
    #define Q 2

    init_scenes();
    
    
    while(1) {
        if (arm_fsm_rt_cpl == disp_adapter0_task()) {
            VT_sdl_flush(1);
            px+=d;
            if (px>=(20<<Q))
            {
                px=(20<<Q);
                d=-d;
            }
            if (px<=-(20<<Q))
            {   
                px=-(20<<Q);
                d=-d;
            }
            arm2d_scene_cmsis_stream_new_pos(currentScene,px>>Q);

        }
        else
        {
            arm2d_scene_cmsis_stream_new_spectrum(currentScene,fftAmp);
            arm2d_scene_cmsis_stream_new_amplitude(currentScene,samples);
        }
        
        
    }

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
    start_lcd();

    printf("\r\nArm-2D PC Template\r\n");

    arm_irq_safe {
        arm_2d_init();
    }

    disp_adapter0_init();

    init_scenes();


    int px=0;
    int d=2;
    #define Q 2

    while(1) {
        if (arm_fsm_rt_cpl == disp_adapter0_task()) {
            px+=d;
            if (px>=(20<<Q))
            {
                px=(20<<Q);
                d=-d;
            }
            if (px<=-(20<<Q))
            {   
                px=-(20<<Q);
                d=-d;
            }
            arm2d_scene_cmsis_stream_new_pos(currentScene,px>>Q);

        }
        else
        {
            arm2d_scene_cmsis_stream_new_spectrum(currentScene,fftAmp);
            arm2d_scene_cmsis_stream_new_amplitude(currentScene,samples);
        }
    }


    return 0;
}
#endif 

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


