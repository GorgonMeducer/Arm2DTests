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

#include "arm_2d.h"

#define __USER_SCENE_CMSIS_STREAM_IMPLEMENT__
#include "arm_2d_scene_cmsis_stream.h"

#include "arm_2d_helper.h"
#include "arm_extra_controls.h"

#include <stdlib.h>
#include <string.h>

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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"  
#elif __IS_COMPILER_ARM_COMPILER_5__
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wunused-value"
#endif

/*============================ MACROS ========================================*/

/*
 * note: If you want to change the background picture, you can simply update 
 *       the macro c_tileCMSISStreamPanel to your own tile of the background picture.
 * 
 */

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif



/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tilecmsisLOGORGB565;

//extern const arm_2d_tile_t c_tileCMSISLogo;
//extern const arm_2d_tile_t c_tileCMSISLogoMask;
//extern const arm_2d_tile_t c_tileCMSISLogoA2Mask;
//extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void arm2d_scene_cmsis_stream_new_spectrum(user_scene_cmsis_stream_t *ptScene,
    const q15_t *fftSpectrum)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ptThis->fftSpectrum = fftSpectrum;
}

void arm2d_scene_cmsis_stream_new_amplitude(user_scene_cmsis_stream_t *ptScene,
    const q15_t *amplitude)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ptThis->amplitude = amplitude;
}

void arm2d_scene_cmsis_stream_new_speed(user_scene_cmsis_stream_t *ptScene,
    const int speed)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ptThis->speedPos = speed;
}

static void __on_scene_cmsis_stream_depose(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ARM_2D_UNUSED(ptThis);
    
    ptScene->ptPlayer = NULL;
    
    /* reset timestamp */
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    if (!this.bUserAllocated) {
        free(ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene cmsis stream                                                                    *
 *----------------------------------------------------------------------------*/

static void __on_scene_cmsis_stream_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_cmsis_stream_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_cmsis_stream_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    //ARM_2D_UNUSED(ptThis);

    #define MAXPOS 70
    ptThis->oldPos = ptThis->pos;
    ptThis->pos += ptThis->speedPos;
    if (ptThis->pos > MAXPOS)
    {
        ptThis->pos = MAXPOS;
        ptThis->speedPos = -ptThis->speedPos;
    }

    if (ptThis->pos <-MAXPOS)
    {
        ptThis->pos = -MAXPOS;
        ptThis->speedPos = -ptThis->speedPos;
    }

    ptThis->ptDirtyRegion[0].tRegion.tLocation.iY = ptThis->originDirty+ptThis->oldPos;
    ptThis->ptDirtyRegion[1].tRegion.tLocation.iY = ptThis->originDirty+ptThis->pos;
    
}

static void __on_scene_cmsis_stream_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    /* switch to next scene after 10s */
    /*if (arm_2d_helper_is_time_out(10000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
    */
}

static void __before_scene_cmsis_stream_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_cmsis_stream_background_handler)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    /*-----------------------draw back ground begin-----------------------*/
   

    /*-----------------------draw back ground end  -----------------------*/
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_cmsis_stream_handler)
{
    user_scene_cmsis_stream_t *ptThis = (user_scene_cmsis_stream_t *)pTarget;
    ARM_2D_UNUSED(ptTile);
    ARM_2D_UNUSED(bIsNewFrame);
    
    arm_2d_canvas(ptTile, __canvas) {
    /*-----------------------draw the foreground begin-----------------------*/
        
        /* following code is just a demo, you can remove them */
        
        arm_2d_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);


        /* draw the cmsis logo using mask in the centre of the screen */
        
        arm_2d_align_centre(__canvas, c_tilecmsisLOGORGB565.tRegion.tSize) {

            arm_2d_region_t logo;

            logo = __centre_region;
            logo.tLocation.iY = logo.tLocation.iY + ptThis->pos;

            /*arm_2d_tile_copy_with_src_mask(&c_tileCMSISLogo,
                                           &c_tileCMSISLogoMask,
                                           ptTile,
                                           &logo,
                                           ARM_2D_CP_MODE_COPY);*/


            arm_2d_tile_copy(&c_tilecmsisLOGORGB565,
                             ptTile,
                             &logo,
                             ARM_2D_CP_MODE_COPY);
            arm_2d_op_wait_async(NULL);
            
            

            }
        
        arm_2d_layout(__canvas) {
        __item_line_vertical(__GLCD_CFG_SCEEN_WIDTH__,__GLCD_CFG_SCEEN_HEIGHT__>>1) {
            spectrum_display_show(&(ptThis->tSpectrum),
                ptTile,&__item_region,
                ptThis->fftSpectrum,
                __item_region.tSize.iWidth - 10,
                __item_region.tSize.iHeight,
                bIsNewFrame);
            
          }
        __item_line_vertical(__GLCD_CFG_SCEEN_WIDTH__,__GLCD_CFG_SCEEN_HEIGHT__>>1) {
            amplitude_display_show(&(ptThis->tAmplitude),
                ptTile,&__item_region,
                ptThis->amplitude,
                __item_region.tSize.iWidth - 10,
                __item_region.tSize.iHeight,
                bIsNewFrame);
          }
        }
        

        /* draw text at the top-left corner */

        //arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
        //arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
        //arm_lcd_text_set_draw_region(NULL);
        //arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
        //arm_lcd_text_location(0,0);
        //arm_lcd_puts("Scene CMSIS-Stream");

    /*-----------------------draw the foreground end  -----------------------*/
    }
    
    arm_2d_op_wait_async(NULL);

    return arm_fsm_rt_cpl;
}

#include <stdio.h>
ARM_NONNULL(3)
user_scene_cmsis_stream_t *__arm_2d_scene_cmsis_stream_init(int nbFFTBins,   int nbAmps,arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_cmsis_stream_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    /*! define dirty regions */
    IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

        /* the dirty region for logo display*/
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),
        ADD_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            0  /* initialize at runtime later */
        ),

        /* add the last region:
         * it is the top left corner for text display
         */
        #if 0 
        ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
            .tLocation = {
                .iX = 0,
                .iY = 0,
            },
            .tSize = {
                .iWidth = 240,
                .iHeight = 240,
            },
        ),
        #endif
        

    END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)
    
    s_tDirtyRegions[dimof(s_tDirtyRegions)-1].ptNext = NULL;

    /* get the screen region */
    arm_2d_region_t tScreen
        = arm_2d_helper_pfb_get_display_area(
            &ptDispAdapter->use_as__arm_2d_helper_pfb_t);
    
   

    arm_2d_align_centre(tScreen, c_tilecmsisLOGORGB565.tRegion.tSize) {
        s_tDirtyRegions[0].tRegion = __centre_region;
        s_tDirtyRegions[1].tRegion = __centre_region;
    }

    arm_2d_layout(tScreen) {
        __item_line_vertical(__GLCD_CFG_SCEEN_WIDTH__,__GLCD_CFG_SCEEN_HEIGHT__>>1) {
            s_tDirtyRegions[2].tRegion=__item_region;

          }
        __item_line_vertical(__GLCD_CFG_SCEEN_WIDTH__,__GLCD_CFG_SCEEN_HEIGHT__>>1) {
            s_tDirtyRegions[3].tRegion=__item_region;
          }
        }
    
    if (NULL == ptThis) {
        ptThis = (user_scene_cmsis_stream_t *)malloc(sizeof(user_scene_cmsis_stream_t));
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
        
    } else {
        bUserAllocated = true;
        
    }

    memset(ptThis, 0, sizeof(user_scene_cmsis_stream_t));
       
    
    *ptThis = (user_scene_cmsis_stream_t){
        .use_as__arm_2d_scene_t = {
            /* Please uncommon the callbacks if you need them
             */
            //.fnBackground   = &__pfb_draw_scene_cmsis_stream_background_handler,
            .fnScene        = &__pfb_draw_scene_cmsis_stream_handler,
            .ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_cmsis_stream_background_start,
            //.fnOnBGComplete = &__on_scene_cmsis_stream_background_complete,
            .fnOnFrameStart = &__on_scene_cmsis_stream_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_cmsis_stream_switching_out,
            .fnOnFrameCPL   = &__on_scene_cmsis_stream_frame_complete,
            .fnDepose       = &__on_scene_cmsis_stream_depose,
        },
        .bUserAllocated = bUserAllocated,
    };

    ptThis->fftSpectrum = NULL;
    spectrum_display_init(&(ptThis->tSpectrum),nbFFTBins);
    amplitude_display_init(&(ptThis->tAmplitude),nbAmps);
    ptThis->pos=0;
    ptThis->oldPos=0;
    ptThis->originDirty = s_tDirtyRegions[0].tRegion.tLocation.iY;



    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

