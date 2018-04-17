#include <iostream>
#include <vector>
#include <string>
#ifdef __cplusplus
extern "C"
{
#endif

#include "ASiftParameter.h"
#include "CBuffer.h"
#include "keyPoint.h"
#include "BMP.h"
#include "featMatching.h"
#include "Surf.h"

#ifdef __cplusplus
};
#endif
#include "ComputeHMatrix.h"
#include "CutImage.h"
#include "ReadDir.h"
using namespace std;
#include "IS.h"

//初始化
int ISInit(int& width, int& height)
{
    int ret = 0;
    ret = initSurfParameter(width, height);
    if(ret != 0)
    {
        printf("initASiftParameter error.\n");
        return -1;
    }
    ret = initCBufferList();
    if(ret != 0)
    {
        printf("initCBufferList error.\n");
        return -2;
    }
    return 0;
}
//释放资源
int ISUnit()
{
    int ret = 0;
    ret = unitSurfParameter();
    if(ret != 0)
    {
        printf("unitASiftParameter error.\n");
        return -1;
    }
    ret = unitCBufferList();
    if(ret != 0)
    {
        printf("unitCBufferList error.\n");
        return -2;
    }
    return 0;
}

//计算图像特征点
int ISFeature(vector<string>& imagesPath, vector<string>& featurePath)
{
    int w, h;
    float* iarr = NULL;
    keyPointInfo mKeyPointInfo;
    initKeyPoints(&mKeyPointInfo);

    for(unsigned int i = 0; i < imagesPath.size(); i ++)
    {
        IMAGE mImg;
        LoadBMP(imagesPath[i].c_str(), &mImg, BIT24);
        w = mImg.width;
        h = mImg.height;
        BmpToGrayBit32(mImg.data, &iarr, w, h);
        freeImage(&mImg);
       
        computeImageFeatures(iarr, w, h, &mKeyPointInfo);

        fprintfFeature((char*)(featurePath[i].c_str()), &mKeyPointInfo);
        clearKeyPoints(&mKeyPointInfo);
        free(iarr);
    }
    unitKeyPoints(&mKeyPointInfo);   

    return 0;
}
static int deleteRepeatedCoor(MatchCoorList* mMatchCoorList) {
	int coorNum = mMatchCoorList->count;
	int *label1 = (int*)calloc(1, sizeof(int) * coorNum);
	int *label2 = (int*)calloc(1, sizeof(int) * coorNum);

	for (int i = 0; i < coorNum; i++) {
		label1[i] = 0;
		label2[i] = 0;
	}
	int i_x1, i_y1, i_x2, i_y2;
	int x1, y1, x2, y2;
	int realMatch = 0;
	for (int i = 0; i < coorNum; i++) {
		if (!label1[i]) {
			x1 = (int)mMatchCoorList->matchCoor[i].coor1.x;
			y1 = (int)mMatchCoorList->matchCoor[i].coor1.y;
			label1[i] = 1;
		}
		if (!label2[i]) {
			x2 = (int)mMatchCoorList->matchCoor[i].coor2.x;
			y2 = (int)mMatchCoorList->matchCoor[i].coor2.y;
			label2[i] = 1;
		}
		if (label1[i] == 1 && label2[i] == 1) {
			mMatchCoorList->matchCoor[realMatch].coor1.x = mMatchCoorList->matchCoor[i].coor1.x;
			mMatchCoorList->matchCoor[realMatch].coor1.y = mMatchCoorList->matchCoor[i].coor1.y;
			mMatchCoorList->matchCoor[realMatch].coor2.x = mMatchCoorList->matchCoor[i].coor2.x;
			mMatchCoorList->matchCoor[realMatch].coor2.y = mMatchCoorList->matchCoor[i].coor2.y;
			realMatch++;
			for (int j = i + 1; j < coorNum; j++) {
				i_x1 = (int)mMatchCoorList->matchCoor[j].coor1.x;
				i_y1 = (int)mMatchCoorList->matchCoor[j].coor1.y;
				i_x2 = (int)mMatchCoorList->matchCoor[j].coor2.x;
				i_y2 = (int)mMatchCoorList->matchCoor[j].coor2.y;
				if ((x1 == i_x1 || y1 == i_y1) && !label1[j]) {
					label1[j] = 2;
				}
				if ((x2 == i_x2 || y2 == i_y2) && !label2[j]) {
					label2[j] = 2;
				}
			}
		}
	}

	free(label1);
	free(label2);
	mMatchCoorList->count = realMatch;
	return realMatch;
}

//计算两张图像的匹配点
int ISMatching(vector<string>& featuresPath, vector<vector<string>>& matchings)
{
    MatchCoorList mMatchCoorList;
    keyPointInfo mKeyPointInfo1;
    keyPointInfo mKeyPointInfo2;
    initKeyPoints(&mKeyPointInfo1);
    initKeyPoints(&mKeyPointInfo2);

    ISInitMatchCoorList(&mMatchCoorList);

    for(unsigned int i = 0; i < featuresPath.size() - 1; i ++)
    {
        fscanfFeature((char*)(featuresPath[i].c_str()), &mKeyPointInfo1);
        for(unsigned int j = i + 1; j < featuresPath.size(); j ++)
        {
            
            ISClearMatchCoorList(&mMatchCoorList);

            fscanfFeature((char*)(featuresPath[j].c_str()), &mKeyPointInfo2);

            ISFeatMatching(&mKeyPointInfo1, &mKeyPointInfo2, &mMatchCoorList);
			deleteRepeatedCoor(&mMatchCoorList);
            fpritfMatching((char*)(matchings[i][j - i - 1].c_str()), &mMatchCoorList);

            
            clearKeyPoints(&mKeyPointInfo2);
        }
        clearKeyPoints(&mKeyPointInfo1);
    }
    unitKeyPoints(&mKeyPointInfo1);
    unitKeyPoints(&mKeyPointInfo2);
    ISUnitMatchCoorList(&mMatchCoorList);
    return 0;
}
