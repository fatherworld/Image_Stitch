#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "ISStruct.h"
#include "keyPoint.h"
#include "ISLog.h"
#include "featMatching.h"

int ISInitMatchCoorList(MatchCoorList* mMatch)
{
    if(mMatch == NULL)
        return -1;
    mMatch->count = 0;
    mMatch->matchCoor = (MatchCoor*)calloc(1, sizeof(MatchCoor) * IS_MAX_MATCH_NUMBER);
	CHECK_ALLOC_P(mMatch->matchCoor);
    return 0;
}
int ISUnitMatchCoorList(MatchCoorList* mMatch)
{
    if(mMatch == NULL)
        return -1;
    mMatch->count = 0;
    free(mMatch->matchCoor);
    return 0;
}
int ISClearMatchCoorList(MatchCoorList* mMatch)
{
    if(mMatch == NULL)
        return -1;
    mMatch->count = 0;
    memset(mMatch->matchCoor, 0, sizeof(MatchCoor) * IS_MAX_MATCH_NUMBER);
    return 0;
}
static float getDescriptorsLen(float* descriptors1, float* descriptors2, int count)
{
    float sum = 0.0f;
    int i = 0;
    for(i = 0; i < count; i ++)
    {
        sum += ((descriptors1[i] - descriptors2[i]) * (descriptors1[i] - descriptors2[i]));
    }
    return (float)sqrtf(sum);
}


int ISFeatMatching(keyPointInfo* keys1, keyPointInfo* keys2, MatchCoorList* matches)
{
    float dist, d1, d2;
    int i, j;
    int t_j;
    for(i = 0; i < keys1->count; i ++)
    {
        d1 = 1000.0f;
        d2 = 1000.0f;
        for(j = 0; j < keys2->count; j ++)
        {
            dist = getDescriptorsLen(keys1->mkeyPoint[i].vec, keys2->mkeyPoint[j].vec, Alva_VecLength);
            if(dist < d1)
            {
                d2 = d1;
                d1 = dist;
                t_j = j;
            }else if(dist < d2)
            {
                d2 = dist;
            }
        }
        if(d1 / d2 < /*0.65f*/ 0.85/*0.75f 0.8f*/ )
        {
            int matchCount = matches->count;
            matches->matchCoor[matchCount].coor1.x = keys1->mkeyPoint[i].x;
            matches->matchCoor[matchCount].coor1.y = keys1->mkeyPoint[i].y;
            matches->matchCoor[matchCount].coor2.x = keys2->mkeyPoint[t_j].x;
            matches->matchCoor[matchCount].coor2.y = keys2->mkeyPoint[t_j].y;
            matches->count ++;
        }
    }
    return 0;
}

