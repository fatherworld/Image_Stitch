#ifndef _Image_IS_h_
#define _Image_IS_h_
#include "ISStruct.h"
#define AlvaRound(x) (x+0.5f);
#define AlvaMax(a,b) ((a) > (b) ? (a) : (b))
#define AlvaMin(a,b) ((a) < (b) ? (a) : (b))
#pragma warning(disable:4996)
typedef struct _IS_Coor_
{
    int x;
    int y;
}ISCoor;

typedef struct _IS_Img_To_BImg_Info_
{
    int isHOK;
    ImgSize thisImgSize;
    ImgSize thisImgRotateSize;
    int offsetX;
    int offsetY;
}ImgRotateInfo;

typedef struct _IS_Base_Img_Info_
{
    int listCount;
    ImgSize baseImgSize;
    ImgRotateInfo* ImgToBaseList;
}BaseImgInfo;

typedef struct _IS_Img_Info_
{
    int baseCount;
    BaseImgInfo* baseImgInfoList;
}ISImgInfo;

int ISImgInfoInit(int baseCount, ISImgInfo* mISImgInfo);
int ISImgInfoUnit(ISImgInfo* mISImgInfo);

int ISComputeISImgMulHInfo(vector<string>& cutImagesPaths, vector<vector<string>>& RotateImagesPaths, ISImgInfo* mISImgInfo, HMatrixInfo* mHMatrixInfo);
int ImageConformity(vector<vector<string>>& rotateImages, vector<string>& cutImages, ISImgInfo* mISImgInfo, 
   const char* baseImageName, vector<string>& ISImgsPath, vector<string>& names, int* pnOK, char* outImageName);

#endif