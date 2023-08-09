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
#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d_helper.h"
#include "arm_2d.h"
#include "amplitude_display.h"

#include <assert.h>

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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


void amplitude_display_init(amplitude_display_t *ptCFG,int nbAmpValues)
{
   ptCFG->nbAmpValues = nbAmpValues;

}
#include <stdio.h>
#define COLOR GLCD_COLOR_BLUE
static void drawLine(uint32_t dx, uint32_t dy,const arm_2d_tile_t *ptTarget,uint32_t ax,q15_t ay,q15_t by)
{
     arm_2d_location_t tLocation;

     if (abs(by-ay)<=1)
     {
       tLocation.iX = ax>>15;tLocation.iY=dy-ay;
       //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
                   //printf("%d %d\n",tLocation.iX,tLocation.iY);

       arm_2d_rgb16_draw_point_fast(ptTarget,tLocation,COLOR);
       uint32_t n=ax+dx;
       tLocation.iX = n>>15;tLocation.iY=dy-by;
       //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
                  //printf("%d %d\n",tLocation.iX,tLocation.iY);

       arm_2d_rgb16_draw_point_fast(ptTarget,tLocation,COLOR);
     }
     else
     {
        int32_t refdelta = dx / (abs(by-ay)+1);
        int32_t delta;
        uint32_t x = ax;
        int start;
        int end;
        if (ay<by)
        {
            start = ay;
            end = by;
            delta=refdelta;
        }
        else
        {
            start = by;
            end = ay;
            x = ax + dx;
            delta=-refdelta;
        }
        for(int i=start;i<end;i++)
        {
            tLocation.iX = x>>15;tLocation.iY=dy-i;
            //printf("%d %d\n",tLocation.iX,tLocation.iY);
            //arm_2d_rgb565_draw_point(ptTarget,tLocation,COLOR);
            arm_2d_rgb16_draw_point_fast(ptTarget,tLocation,COLOR);

            x+=delta;
        }
     }
}

#include <stdio.h>
#define PADX 4
#define PADY 4
void amplitude_display_show(amplitude_display_t *ptCFG,
    const arm_2d_tile_t *ptTarget, 
    arm_2d_region_t *amplitudeRegion,
    const q15_t *values,
    int width,
    int height,
    int bIsNewFrame)
{
    int_fast16_t iWidth = width;
    int_fast16_t iHeight = height;
   
   /*printf("AMP: %d %d %d %d\n",amplitudeRegion->tLocation.iX,
    amplitudeRegion->tLocation.iY,
    amplitudeRegion->tSize.iWidth,
    amplitudeRegion->tSize.iHeight);*/

   arm_2d_region_t tValidRegion;
   arm_2d_location_t tOffset;

   const arm_2d_tile_t *root = arm_2d_tile_get_root( ptTarget,
                                            &tValidRegion, 
                                            &tOffset);

    
    if (root == NULL)
    {
        return ;
    }
    else 
    {
        arm_2d_region_t outRegion;
        tValidRegion.tLocation.iX = tOffset.iX;
        tValidRegion.tLocation.iY = tOffset.iY;
        /*printf("VAL: %d %d %d %d\n",
    tOffset.iX,
    tOffset.iY,
    tValidRegion.tSize.iWidth,
    tValidRegion.tSize.iHeight);
        printf("AMP: %d %d %d %d\n",
    amplitudeRegion->tLocation.iX,
    amplitudeRegion->tLocation.iY,
    amplitudeRegion->tSize.iWidth,
    amplitudeRegion->tSize.iHeight);*/
if (!arm_2d_region_intersect(   &tValidRegion,
                           amplitudeRegion,
                            &outRegion))
{
    return;
}
        /* THIS TEST IS NOT UNDERSTOOD 
           WHY DO I HAVE CRASH IN CASE THE REGION
           IS CORRESPONDING TO NAVIGATION LAYER.
           ANYWAY, I DON'T WANT TO REDRAW THIS CONTROL
           WHEN IN NAVIGATION LAYER BECAUSE IT IS 
           CONSUMING TOO MANY CYCLES FOR NOTHING.
           SO THERE MUST BE A CLEANER WAY TO AVOID
           REDRAWING IN THIS CASE

        */
       //if (tOffset.iY > amplitudeRegion->tSize.iHeight)
        if (root->tRegion.tSize.iHeight < 240)
        {
            return;
        }
    }
   //const arm_2d_tile_t *root = ptTarget;

    arm_2d_region_t contentRegion = *amplitudeRegion;

    iWidth -= 2*PADX;
    iHeight -= 2*PADY;
    contentRegion.tSize.iWidth = iWidth;
    contentRegion.tSize.iHeight = iHeight;

    int_fast16_t pad = (amplitudeRegion->tSize.iWidth - iWidth)>>1;
    contentRegion.tLocation.iX += pad;

    pad = (amplitudeRegion->tSize.iHeight - iHeight)>>1;
    contentRegion.tLocation.iY += pad;


    
    draw_round_corner_box(  root, 
                                    &contentRegion, 
                                    GLCD_COLOR_OLIVE, 
                                    70,
                                    bIsNewFrame);

    arm_2d_op_wait_async(NULL);
    

    uint32_t dx = (uint32_t)(1<<15) * iWidth / ptCFG->nbAmpValues;
    uint32_t dy=contentRegion.tLocation.iY+(iHeight>>1); 

    uint32_t currentX=contentRegion.tLocation.iX<<15;
    q15_t currentY=0;
    currentY = (((q31_t) values[0] * ((iHeight>>1)-1)) >> 15);

    arm_2d_location_t tLocation;
    for(int i=1;i<ptCFG->nbAmpValues;i++)
    {
        // >> 1 because half scale of LCD
        q15_t newY = (((q31_t) values[i] * ((iHeight>>1)-1)) >> 15);
        drawLine(dx,dy,root,currentX,currentY,newY);
        //tLocation.iX = currentX>>15;tLocation.iY=dy-newY;
        //arm_2d_rgb565_draw_point(ptTarget,tLocation,GLCD_COLOR_RED);
        currentX = currentX+dx;
        currentY = newY;
    }

    
    draw_round_corner_border(   root, 
                              &contentRegion, 
                                GLCD_COLOR_BLACK, 
                                (arm_2d_border_opacity_t)
                                {255,255,255,255},
                                (arm_2d_corner_opacity_t)
                                {255,255,255,255});
    
    arm_2d_op_wait_async(NULL);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
