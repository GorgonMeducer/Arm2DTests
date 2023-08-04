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

#ifndef __AMPLITUDE_DISPLAY_H__
#define __AMPLITUDE_DISPLAY_H__

/*============================ INCLUDES ======================================*/
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct amplitude_display_t {
    uint32_t nbAmpValues;
} amplitude_display_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
void amplitude_display_init(amplitude_display_t *ptCFG,int nbAmpValues);

extern
void amplitude_display_show(amplitude_display_t *ptCFG,
	const arm_2d_tile_t *ptTarget, 
	arm_2d_region_t *amplitudeRegion,
	const q15_t *values,
	int width,
	int height,
	int bIsNewFrame);

#ifdef   __cplusplus
}
#endif

#endif
