#ifndef _feat_Matching_h_
#define _feat_Matching_h_

#define IS_MAX_MATCH_NUMBER 2000
#include "ISStruct.h"
#include "keyPoint.h"
#ifdef __cplusplus
extern "C"
{
#endif

    int ISInitMatchCoorList(MatchCoorList* mMatch);
    int ISUnitMatchCoorList(MatchCoorList* mMatch);
    int ISClearMatchCoorList(MatchCoorList* mMatch);
    int ISFeatMatching(keyPointInfo* keys1, keyPointInfo* keys2, MatchCoorList* matches);
#ifdef __cplusplus
};
#endif


#endif