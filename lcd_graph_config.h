#ifndef _CUSTOM_H_
#define _CUSTOM_H_
#include "arm_2d.h"

#ifdef   __cplusplus
extern "C"
{
#endif

#include "arm_2d_scene_cmsis_stream.h"

extern void wait_for_display();
extern void scene_cmsis_stream_loader(user_scene_cmsis_stream_t *scene) ;

/* Put anything you need in this file */


#ifdef   __cplusplus
}
#endif


#endif 
