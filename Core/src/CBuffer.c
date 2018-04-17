#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "CBuffer.h"
#include "ASiftParameter.h"
#include "ISLog.h"
static float* gCBufferList[Buffer_Number] = {0};

float** getBufferList()
{
    return gCBufferList;
}

int initCBufferList()
{
    int i;
    SurfParameter* mSurfParameter = NULL;
    mSurfParameter = getSurfarameter();
    gCBufferList[Buffer_gray_src] = (float*)calloc(1, sizeof(float) * mSurfParameter->srcSize.width * mSurfParameter->srcSize.height);
	CHECK_ALLOC_P(gCBufferList[Buffer_gray_src]);
    gCBufferList[Buffer_integral_image] = (float*)calloc(1, sizeof(float) * mSurfParameter->srcSize.width * mSurfParameter->srcSize.height);
	CHECK_ALLOC_P(gCBufferList[Buffer_integral_image]);
    for(i = 0; i < mSurfParameter->pyramids; i ++)
    {
        gCBufferList[Buffer_pyramid_image + i] = (float*)calloc(1, sizeof(float) * mSurfParameter->responseFilter[i].size.width * mSurfParameter->responseFilter[i].size.height);
		CHECK_ALLOC_P(gCBufferList[Buffer_pyramid_image + i]);
    }
    return 0;
}

int unitCBufferList()
{
    int i = 0;
    for(i = 0; i < Buffer_Number; i ++)
    {
        if(gCBufferList[i] != NULL)
        {
            free(gCBufferList[i]);
            gCBufferList[i] = NULL;
        }
    }
    return 0;
}