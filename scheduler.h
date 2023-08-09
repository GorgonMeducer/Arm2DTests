/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/

#ifndef _SCHEDULER_H_ 
#define _SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif


#define NB_IDENTIFIED_NODES 3
#define ARM2D_ID 0
#define AMP_ID 1
#define SOURCE_ID 2

extern void *get_scheduler_node(int32_t nodeID);

extern int init_scheduler();
extern void free_scheduler();
extern uint32_t scheduler(int *error);

#ifdef   __cplusplus
}
#endif

#endif

