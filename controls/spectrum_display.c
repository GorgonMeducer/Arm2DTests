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
#include "spectrum_display.h"

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

extern const arm_2d_tile_t c_tileSmallDotMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#include <stdio.h>

void spectrum_display_init(spectrum_display_t *ptCFG,int nbFFTBins)
{
   ptCFG->nbFFTbins = nbFFTBins;
}

#define BINWIDTH 6UL
#define PADX 4
#define PADY 4
#define PADFFT 6
void spectrum_display_show(spectrum_display_t *ptCFG,
    const arm_2d_tile_t *ptTarget, 
    arm_2d_region_t *spectrumRegion, 
    const q15_t *fftSpectrum,
    int width,
    int height,
    int bIsNewFrame)
{
    int_fast16_t iWidth = width;
    int_fast16_t iHeight = height;

    /* DIRTY TEST TO AVOID
    REDRAWING WHEN IN NAVIGATION DIRTY REGION

    */
    if (ptTarget->tRegion.tLocation.iY<0)
    {
        return;
    }
   
    arm_2d_region_t contentRegion = *spectrumRegion;

    iWidth -= 2*PADX;
    iHeight -= 2*PADY;
    contentRegion.tSize.iWidth = iWidth;
    contentRegion.tSize.iHeight = iHeight;

    int_fast16_t pad = (spectrumRegion->tSize.iWidth - iWidth)>>1;
    contentRegion.tLocation.iX += pad;

    pad = (spectrumRegion->tSize.iHeight - iHeight)>>1;
    contentRegion.tLocation.iY += pad;


    
    draw_round_corner_box(  ptTarget, 
                                    &contentRegion, 
                                    GLCD_COLOR_OLIVE, 
                                    70,
                                    bIsNewFrame);

    arm_2d_op_wait_async(NULL);
    
    // Conjugate part ignored
    uint32_t nbPts=(ptCFG->nbFFTbins>>1);
    if (nbPts > 0)
    {
       // 0x40000000 / 8192 
       uint32_t dx = (0x20000UL / nbPts)*(iWidth-2*PADFFT);
       //printf("%x\n",dx);
       uint32_t max=fftSpectrum[0];
       uint32_t pos=0;
       uint32_t lastPixel = contentRegion.tLocation.iX+PADFFT;
       arm_2d_region_t bar;

       for(int i=0;i<nbPts;i++)
       {
          if (fftSpectrum[i]>max)
          {
            max = fftSpectrum[i];
          }
          pos += dx;
          uint32_t deltaPixel = (pos >> 17);
          if (deltaPixel >= BINWIDTH)
          {
             uint32_t dy = ((uint32_t)iHeight * max) / 32767; 
             if (dy>=iHeight)
             {
                dy = iHeight;
             }
             bar.tSize.iHeight=dy;
             bar.tSize.iWidth=deltaPixel;;
             bar.tLocation.iX = lastPixel;
             bar.tLocation.iY = iHeight-dy+contentRegion.tLocation.iY;

#if 0
             arm_2d_fill_colour_with_opacity(ptTarget, 
                                            &bar, 
                                            (__arm_2d_color_t) {GLCD_COLOR_RED},
                                            170);
#endif
             if (dy>0)
             {
                 arm_2d_fill_colour(ptTarget, &bar, GLCD_COLOR_RED);
             }

             lastPixel += deltaPixel;
             pos = pos - (deltaPixel<<17);
             max = 0;
          }
       }
    }
    
    arm_2d_op_wait_async(NULL);

    
    draw_round_corner_border(   ptTarget, 
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
