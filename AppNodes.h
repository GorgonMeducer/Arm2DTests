#ifndef _APPNODES_H_
#define _APPNODES_H_

#ifdef HOST
#include "Virtual_TFT_Port.h"
#endif 

#include "arm_2d.h"


#include <cstdio>
#include <cstring>

template<typename IN, int inputSize>
class Arm2D;


template<>
class Arm2D<arm_2d_tile_t*,1>:public NodeBase
{
public:
    Arm2D(std::initializer_list<FIFOBase<arm_2d_tile_t*>*> src):
    mSrcList(src)
    {
        mArm2DList = std::vector<arm_2d_tile_t*>{src.size(),NULL};

        mScene = (user_scene_cmsis_stream_t *)malloc(sizeof(user_scene_cmsis_stream_t));
        scene_cmsis_stream_loader(mScene);
        
    };

    ~Arm2D()
    {
       free(mScene);
    }

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    void update_tiles()
    {
        if (mScene)
        {
            for(int i=0;i<numberOfReadBuffers();i++)
            {
                mArm2DList[i]=getReadBuffer(i)[0];
            }
            arm2d_scene_cmsis_stream_new_tiles(mScene,
                mArm2DList.data(),numberOfReadBuffers());
        }
    }

#ifdef HOST
    int run() final
    {
        wait_for_display();
        VT_sdl_flush(1);
        update_tiles();
        
        return(CG_SUCCESS);
    };
#else 

    int run() final
    {
        wait_for_display();
        update_tiles();
        return(CG_SUCCESS);
    };
#endif

protected:
    user_scene_cmsis_stream_t *mScene;
private:
  std::vector<FIFOBase<arm_2d_tile_t*>*> mSrcList;
  std::vector<arm_2d_tile_t*> mArm2DList;

  arm_2d_tile_t **getReadBuffer(int id=1){return mSrcList[id]->getReadBuffer(1);};
  bool willUnderflow(int id=1){return mSrcList[id]->willUnderflowWith(1);};
  
  int numberOfReadBuffers(){return mSrcList.size();};

};


template<typename OUT,int outputSize>
class Signal;

static const int16_t ref_samples[256]={0, 5880, 11148, 14557, 15551, 12938, 7604, 1082, -4957, -8462, -8246, -4159, 2832, 11350, 19344, 25186, 26672, 25042, 20802, 14436, 8997, 5870, 2533, 6496, 9387, 12694, 17171, 19751, 22417, 22377, 21470, 18885, 16383, 14818, 13497, 12465, 12955, 13773, 14973, 16578, 18190, 20367, 19145, 21667, 17084, 12560, 8550, 94, -1327, 3152, 4536, 8235, 15130, 18731, 20069, 22447, 17417, 10119, 1782, -5472, -11581, -12624, -10419, -6335, -1, 4379, 11625, 11197, 8872, 4984, -3692, -8762, -9138, -12017, -17138, -12781, -10373, -9288, -8558, -7625, -8253, -8950, -10356, -12805, -15044, -17894, -17696, -21111, -19107, -18565, -15028, -9887, -2585, -11362, -3458, -8830, -16384, -23353, -25837, -32509, -31053, -23838, -17788, -11473, -6239, 164, -51, -3796, -8244, -14733, -20903, -22389, -18274, -25087, -15841, -11366, -3446, -2627, -3743, 2653, -242, -4568, -5432, -7091, -10413, -7170, -5146, -4877, -1, 4152, 10623, 8572, 14857, 12699, 7339, 3661, -774, -2286, 4217, -3098, 5421, 10150, 13925, 14256, 26779, 19618, 19757, 13952, 10606, 5961, 6187, 4610, 5479, 10669, 17861, 26228, 31185, 22198, 28899, 19524, 16383, 10149, 2701, 7322, 12074, 13330, 14404, 19710, 26033, 18357, 25953, 18755, 17352, 12654, 7520, 4827, -975, 1210, 8291, 9436, 12569, 13301, 23764, 20351, 17718, 8129, 2960, 686, -9865, -6621, -3424, -6684, -1, 3971, 10580, 5628, 5131, 6410, -2488, -7114, -15574, -12642, -15904, -16439, -13273, -8618, -2103, -7660, 1194, 1435, -10686, -12681, -15992, -17349, -22320, -30464, -17904, -17131, -12513, -7734, -10504, -11000, -12327, -13929, -16384, -20781, -29921, -19991, -24396, -18147, -18465, -14368, -5828, -2586, -10052, -11207, -11906, -14746, -18242, -26581, -21172, -25062, -15056, -10126, -5811, 1114, 7052, 1797, 657, -4280, -6272, -14375, -11252, -17919, -14281, -2424};

template<>
class Signal<int16_t,256>: 
public GenericSource<int16_t,256>
{
public:
    Signal(FIFOBase<int16_t> &dst):
    GenericSource<int16_t,256>(dst){
    };

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final{
        int16_t *b=this->getWriteBuffer();
        arm_scale_q15(ref_samples,g,0,b,256);
    
        if (mode==1)
        {
           g = g / 2;
           if (g <= 0x800)
           {
             mode = 2;
           }
        }
        else
        {
             g = g * 2;
             if (g > 0x4000)
             {
                mode = 1;
             }
        }
        return(CG_SUCCESS);
    };
protected:
int16_t g = 0x7FFF;
int mode = 1;

};


template<typename IN, int inputSize,
         typename OUT,int outputSize>
class AmplitudeWidget;

extern "C" {
    extern void draw_amplitude(arm_2d_tile_t *ptTile,
        q15_t *values,
        int mWidth,
        int mHeight,
        int inputSize);

}
template<int inputSize>
class AmplitudeWidget<int16_t,inputSize,
                     arm_2d_tile_t*,1>:
public GenericNode<int16_t,inputSize,
                   arm_2d_tile_t*,1>
{
public:
    /* Constructor needs the input and output FIFOs */
    AmplitudeWidget(FIFOBase<int16_t> &src,
                   FIFOBase<arm_2d_tile_t*> &dst,
                   int width = 240 - 24, 
                   int height = 120 - 16):
    GenericNode<int16_t,inputSize,
                arm_2d_tile_t*,1>(src,dst),mWidth(width),mHeight(height){

                memset(&mTile,0,sizeof(mTile));

                mTile.tRegion = {
                    .tSize = {(int16_t)(width), (int16_t)(height)},
                };

                mTile.tInfo.bIsRoot = true;
                mTile.tInfo.bHasEnforcedColour = true;
                mTile.pchBuffer = (uint8_t*)malloc(width*height*sizeof(uint16_t)),
                mTile.tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_RGB565;
                //printf("%016llX\n",(uint64_t)mTile.pchBuffer);
    }

    ~AmplitudeWidget()
    {
        free(mTile.pchBuffer);
    }

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    
    
    
    int run() final{
        int16_t *values=this->getReadBuffer();
        arm_2d_tile_t **b=this->getWriteBuffer();
        memset(mTile.pchBuffer, 0xFF, mWidth*mHeight*sizeof(uint16_t));

        draw_amplitude(&mTile,values,mWidth,mHeight,inputSize);

        b[0]= &mTile;
        return(CG_SUCCESS);
    };
protected:
   arm_2d_tile_t mTile;
   int mWidth,mHeight;
};

#endif
