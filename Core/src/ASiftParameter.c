#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include "ASiftParameter.h"

#define M_PI 3.1415927f
static SurfParameter gSurfParameter = {0};

SurfParameter* getSurfarameter()
{
    return &gSurfParameter;
}

int initSurfParameter(int SrcWidth, int SrcHeight)
{
    int i;

    gSurfParameter.octaves = OCTAVES;                //组数
    gSurfParameter.intervals = INTERVALS;              //每组的层数
    gSurfParameter.initSampStep = INIT_SAMPLE;           //降采样步长
    gSurfParameter.thres = THRES;                //阈值
    gSurfParameter.descroptNumber = 1;         

    gSurfParameter.srcSize.width = SrcWidth;
    gSurfParameter.srcSize.height = SrcHeight;
#if 1
    for(i = 0; i < 4; i ++)
    {
        gSurfParameter.responseFilter[i].size.width = gSurfParameter.srcSize.width / 2;
        gSurfParameter.responseFilter[i].size.height = gSurfParameter.srcSize.height / 2;
        gSurfParameter.responseFilter[i].step = INIT_SAMPLE;
    }
    for(i = 0; i < 2; i ++)
    {
        gSurfParameter.responseFilter[4 + i].size.width = gSurfParameter.srcSize.width / 4;
        gSurfParameter.responseFilter[4 + i].size.height = gSurfParameter.srcSize.height / 4;
        gSurfParameter.responseFilter[4 + i].step = INIT_SAMPLE * INIT_SAMPLE;
    }
    for(i = 0; i < 2; i ++)
    {
        gSurfParameter.responseFilter[6 + i].size.width = gSurfParameter.srcSize.width / 8;
        gSurfParameter.responseFilter[6 + i].size.height = gSurfParameter.srcSize.height / 8;
        gSurfParameter.responseFilter[6 + i].step = INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE;
    }
    for(i = 0; i < 2; i ++)
    {
        gSurfParameter.responseFilter[8 + i].size.width = gSurfParameter.srcSize.width / 16;
        gSurfParameter.responseFilter[8 + i].size.height = gSurfParameter.srcSize.height / 16;
        gSurfParameter.responseFilter[8 + i].step = INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE;
    }
    for(i = 0; i < 2; i ++)
    {
        gSurfParameter.responseFilter[10 + i].size.width = gSurfParameter.srcSize.width / 32;
        gSurfParameter.responseFilter[10 + i].size.height = gSurfParameter.srcSize.height / 32;
        gSurfParameter.responseFilter[10 + i].step = INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE * INIT_SAMPLE;
    }
    
    gSurfParameter.filter_map[0][0] = 0;
    gSurfParameter.filter_map[0][1] = 1;
    gSurfParameter.filter_map[0][2] = 2;
    gSurfParameter.filter_map[0][3] = 3;
    
    gSurfParameter.filter_map[1][0] = 1;
    gSurfParameter.filter_map[1][1] = 3;
    gSurfParameter.filter_map[1][2] = 4;
    gSurfParameter.filter_map[1][3] = 5;

    gSurfParameter.filter_map[2][0] = 3;
    gSurfParameter.filter_map[2][1] = 5;
    gSurfParameter.filter_map[2][2] = 6;
    gSurfParameter.filter_map[2][3] = 7;

    gSurfParameter.filter_map[3][0] = 5;
    gSurfParameter.filter_map[3][1] = 7;
    gSurfParameter.filter_map[3][2] = 8;
    gSurfParameter.filter_map[3][3] = 9;

    gSurfParameter.filter_map[4][0] = 7;
    gSurfParameter.filter_map[4][1] = 9;
    gSurfParameter.filter_map[4][2] = 10;
    gSurfParameter.filter_map[4][3] = 11;
#endif
#if 1
    gSurfParameter.responseFilter[0].filter = 9;
    gSurfParameter.responseFilter[1].filter = 15;
    gSurfParameter.responseFilter[2].filter = 21;
    gSurfParameter.responseFilter[3].filter = 27;
    gSurfParameter.responseFilter[4].filter = 39;
    gSurfParameter.responseFilter[5].filter = 51;
    gSurfParameter.responseFilter[6].filter = 75;
    gSurfParameter.responseFilter[7].filter = 99;
    gSurfParameter.responseFilter[8].filter = 147;
    gSurfParameter.responseFilter[9].filter = 195;
    gSurfParameter.responseFilter[10].filter = 291;
    gSurfParameter.responseFilter[11].filter = 387;
#endif
    gSurfParameter.pyramids = OCTAVES * 2 + 2;


    return 0;
}

int unitSurfParameter()
{
    return 0;
}