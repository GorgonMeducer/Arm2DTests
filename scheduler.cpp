/*

Generated with CMSIS-Stream python scripts.
The generated code is not covered by CMSIS-Stream license.

The support classes and code are covered by CMSIS-Stream license.

*/


#include "arm_math.h"
#include "lcd_graph_config.h"
#include "GenericNodes.h"
#include "cg_status.h"
#include "AppNodes.h"
#include "scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION
#endif



CG_AFTER_INCLUDES


/*

Description of the scheduling. 

*/
static uint8_t schedule[3]=
{ 
2,1,0,
};

/***********

Node identification

************/
static void * identifiedNodes[NB_IDENTIFIED_NODES]={0};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 1

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
int16_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 1
CG_BEFORE_BUFFER
arm_2d_tile_t* buf2[BUFFERSIZE2]={0};


typedef struct {
FIFO<int16_t,FIFOSIZE0,1,0> *fifo0;
FIFO<arm_2d_tile_t*,FIFOSIZE1,1,0> *fifo1;
} fifos_t;

typedef struct {
    Arm2D<arm_2d_tile_t*,1> *Arm2D;
    AmplitudeWidget<int16_t,256,arm_2d_tile_t*,1> *amp;
    Signal<int16_t,256> *source;
} nodes_t;

CG_BEFORE_BUFFER
static fifos_t fifos={0};

CG_BEFORE_BUFFER
static nodes_t nodes={0};

void *get_scheduler_node(int32_t nodeID)
{
    if (nodeID >= NB_IDENTIFIED_NODES)
    {
        return(NULL);
    }
    if (nodeID < 0)
    {
        return(NULL);
    }
    return(identifiedNodes[nodeID]);
}

int init_scheduler()
{
    CG_BEFORE_FIFO_INIT;
    fifos.fifo0 = new FIFO<int16_t,FIFOSIZE0,1,0>(buf1);
    if (fifos.fifo0==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    fifos.fifo1 = new FIFO<arm_2d_tile_t*,FIFOSIZE1,1,0>(buf2);
    if (fifos.fifo1==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }

    CG_BEFORE_NODE_INIT;
    nodes.Arm2D = new Arm2D<arm_2d_tile_t*,1>({fifos.fifo1});
    if (nodes.Arm2D==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[ARM2D_ID]=(void*)nodes.Arm2D;
    nodes.Arm2D->setID(ARM2D_ID);
    nodes.amp = new AmplitudeWidget<int16_t,256,arm_2d_tile_t*,1>(*(fifos.fifo0),*(fifos.fifo1));
    if (nodes.amp==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[AMP_ID]=(void*)nodes.amp;
    nodes.amp->setID(AMP_ID);
    nodes.source = new Signal<int16_t,256>(*(fifos.fifo0));
    if (nodes.source==NULL)
    {
        return(CG_MEMORY_ALLOCATION_FAILURE);
    }
    identifiedNodes[SOURCE_ID]=(void*)nodes.source;
    nodes.source->setID(SOURCE_ID);

    return(CG_SUCCESS);

}

void free_scheduler()
{
    if (fifos.fifo0!=NULL)
    {
       delete fifos.fifo0;
    }
    if (fifos.fifo1!=NULL)
    {
       delete fifos.fifo1;
    }

    if (nodes.Arm2D!=NULL)
    {
        delete nodes.Arm2D;
    }
    if (nodes.amp!=NULL)
    {
        delete nodes.amp;
    }
    if (nodes.source!=NULL)
    {
        delete nodes.source;
    }
}


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t scheduler(int *error)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;


    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 3; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = nodes.Arm2D->run();
                }
                break;

                case 1:
                {
                   cgStaticError = nodes.amp->run();
                }
                break;

                case 2:
                {
                   cgStaticError = nodes.source->run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION;
            CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
