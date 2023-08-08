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
#define __AMPLITUDE_DISPLAY2_IMPLEMENT__

#include "./arm_extra_controls.h"
#include "arm_2d_helper.h"
#include "amplitude_display2.h"
#include <assert.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8
#define arm_2d_draw_point_fast    arm_2d_c8bit_draw_point_fast

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16
#define arm_2d_draw_point_fast    arm_2d_rgb16_draw_point_fast

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32
#define arm_2d_draw_point_fast    arm_2d_rgb32_draw_point_fast

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
static void __draw_line(uint32_t dx, 
                        uint32_t dy,
                        const arm_2d_tile_t *ptTarget,
                        uint32_t ax,
                        q15_t ay,
                        q15_t by);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void amplitude_display2_init(amplitude_display2_t *ptThis, amplitude_display2_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(amplitude_display2_t));
    this.tCFG = *ptCFG;
}

ARM_NONNULL(1)
void amplitude_display2_depose( amplitude_display2_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void amplitude_display2_show( amplitude_display2_t *ptThis,
                            const arm_2d_tile_t *ptTile, 
                            const arm_2d_region_t *ptRegion,
                            const q15_t *values,
                            COLOUR_INT tColour,
                            bool bIsNewFrame)
{
    assert(NULL!= ptThis);

    
    if (bIsNewFrame) {
        /* clear bar buffer */
        memset(this.tCFG.tileBar.pchBuffer, 0, get_tile_buffer_size(this.tCFG.tileBar, uint8_t));

        arm_2d_size_t tBarSize = this.tCFG.tileBar.tRegion.tSize;

        int16_t iWidth = tBarSize.iWidth;
        int16_t iHeight = tBarSize.iHeight;
        uint32_t dx = (uint32_t)(1<<15) * iWidth / this.tCFG.hwAmpValues;
        uint32_t dy= iHeight >> 1; 

        uint32_t currentX= 0;
        q15_t currentY=0;
        currentY = (((q31_t) values[0] * ((iHeight>>1)-1)) >> 15);

        /* draw bar buffer */
        for(int i=1;i<this.tCFG.hwAmpValues;i++) {
            q15_t newY = (((q31_t) values[i] * ((iHeight>>1)-1)) >> 15);
            __draw_line(dx,dy,&this.tCFG.tileBar,currentX,currentY,newY);

            currentX = currentX+dx;
            currentY = newY;
        }
    }

    arm_2d_container(ptTile, __control, ptRegion) {
        /* put your drawing code inside here
         *    - &__control is the target tile (please do not use ptTile anymore)
         *    - __control_canvas is the canvas
         */

        arm_2d_align_centre(__control_canvas, this.tCFG.tileBar.tRegion.tSize) {

            draw_round_corner_box(
                                &__control, 
                                &__centre_region, 
                                GLCD_COLOR_OLIVE, 
                                70,
                                bIsNewFrame);


            arm_2d_fill_colour_with_mask(
                &__control,
                &__centre_region,
                &this.tCFG.tileBar,
                (__arm_2d_color_t) {tColour}
            );

            /* make sure the operation is complete */
            arm_2d_op_wait_async(NULL);
        }

        
    }

    arm_2d_op_wait_async(NULL);
}


static void __draw_line(uint32_t dx, 
                        uint32_t dy,
                        const arm_2d_tile_t *ptTarget,
                        uint32_t ax,
                        q15_t ay,
                        q15_t by)
{
     arm_2d_location_t tLocation;

     if (abs(by-ay)<=1) {
       tLocation.iX = ax>>15;tLocation.iY=dy-ay;
       //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
                   //printf("%d %d\n",tLocation.iX,tLocation.iY);

       arm_2d_c8bit_draw_point_fast(ptTarget,tLocation,255);
       uint32_t n=ax+dx;
       tLocation.iX = n>>15;tLocation.iY=dy-by;
       //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
                  //printf("%d %d\n",tLocation.iX,tLocation.iY);

       arm_2d_c8bit_draw_point_fast(ptTarget,tLocation,255);
     } else {
        int32_t refdelta = dx / (abs(by-ay)+1);
        int32_t delta;
        uint32_t x = ax;
        int start;
        int end;
        if (ay<by) {
            start = ay;
            end = by;
            delta=refdelta;
        } else {
            start = by;
            end = ay;
            x = ax + dx;
            delta=-refdelta;
        }
        for(int i=start;i<end;i++) {
            tLocation.iX = x>>15;tLocation.iY=dy-i;
            //printf("%d %d\n",tLocation.iX,tLocation.iY);
            //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
            arm_2d_c8bit_draw_point_fast(ptTarget,tLocation,255);

            x+=delta;
        }
     }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
