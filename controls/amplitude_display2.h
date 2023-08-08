/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

#ifndef __AMPLITUDE_DISPLAY2_H__
#define __AMPLITUDE_DISPLAY2_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"
#include "./__common.h"

#ifdef   __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

/* OOC header, please DO NOT modify  */
#ifdef __AMPLITUDE_DISPLAY2_IMPLEMENT__
#   undef   __AMPLITUDE_DISPLAY2_IMPLEMENT__
#   define  __ARM_2D_IMPL__
#elif defined(__AMPLITUDE_DISPLAY2_INHERIT__)
#   undef   __AMPLITUDE_DISPLAY2_INHERIT__
#   define __ARM_2D_INHERIT__
#endif
#include "arm_2d_utils.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*!
 * \brief a user class for user defined control
 */
typedef struct amplitude_display2_t amplitude_display2_t;

typedef struct amplitude_display2_cfg_t {
    uint16_t hwAmpValues;
    int8_t chPadX;
    int8_t chPadY;
    arm_2d_tile_t tileBar;
} amplitude_display2_cfg_t;

struct amplitude_display2_t {

ARM_PRIVATE(
    amplitude_display2_cfg_t tCFG;
    
    /* place your private member here, following two are examples */
    int64_t lTimestamp[1];
    uint8_t chOpacity;



)
    /* place your public member here */
    
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
ARM_NONNULL(1)
void amplitude_display2_init( amplitude_display2_t *ptThis, 
                              amplitude_display2_cfg_t *ptCFG);

extern
ARM_NONNULL(1)
void amplitude_display2_depose( amplitude_display2_t *ptThis);

extern
ARM_NONNULL(1)
void amplitude_display2_show(   amplitude_display2_t *ptThis,
                                const arm_2d_tile_t *ptTile, 
                                const arm_2d_region_t *ptRegion,
                                const q15_t *values,
                                COLOUR_INT tColour,
                                bool bIsNewFrame);


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#ifdef   __cplusplus
}
#endif

#endif
