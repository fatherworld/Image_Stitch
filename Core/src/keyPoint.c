#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "ISLog.h"
#include "keyPoint.h"

int initKeyPoints(keyPointInfo* mKeyPointInfo)
{
    if(mKeyPointInfo == NULL)
    {
        return -1;
    }
    mKeyPointInfo->count = 0;
    mKeyPointInfo->mkeyPoint = (keyPoint*)calloc(1, sizeof(keyPoint) * Alva_Max_KeyPoint);
	CHECK_ALLOC_P(mKeyPointInfo->mkeyPoint);
    return 0;
}
int unitKeyPoints(keyPointInfo* mKeyPointInfo)
{
    if(mKeyPointInfo == NULL)
    {
        return -1;
    }
    mKeyPointInfo->count = 0;
    if(mKeyPointInfo->mkeyPoint != NULL)
        free(mKeyPointInfo->mkeyPoint);
    return 0;
}
int clearKeyPoints(keyPointInfo* mKeyPointInfo)
{
    mKeyPointInfo->count = 0;
    memset(mKeyPointInfo->mkeyPoint, 0, sizeof(keyPoint) * Alva_Max_KeyPoint);
    return 0;
}
