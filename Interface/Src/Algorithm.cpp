#include <vector>
#include <string>
using namespace std;
#include "Algorithm.h"
#include "ImagePathInfo.h"
#include "CutImage.h"
#include "IS.h"
#include "ISStruct.h"
#include "ComputeHMatrix.h"
#include "Flow.h"
#include "ImageIS.h"
static ImagePathInfo gImagePathInfo;
int IS_Image_Width;
int IS_Image_Height;
HMatrixInfo mHMatrixInfo;
HMatrixInfo mComplementHMatrixInfo;
ISImgInfo mISImgInfo;
//路径参数初始化
int IS_initPath(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath)
{
    gImagePathInfo.clearImagePathInfo();
    gImagePathInfo.init(srcImagePath, dstImagePath, matchingPath, featurePath, imageRotationPath, imageISPath);
    return 0;
}

//图像预处理,获取到图像处理后的宽高
int IS_imagePretreatment()
{
    int ret = 0;
    static vector<int> allImageWidth;
    static vector<int> allImageHeight;
    ret = CutAllImages(gImagePathInfo.outFilePaths, gImagePathInfo.filePaths, IS_Image_Width, IS_Image_Height,allImageWidth,allImageHeight);
    if (ret)
        return ret;
    int count = allImageWidth.size();
    for (int i = 0; i < count; i++)
    {
        if (allImageHeight[i] != IS_Image_Height || allImageWidth[i] != IS_Image_Width)
        {
            return -1;
        }
    }
    initHMatrixInfo(&mHMatrixInfo, gImagePathInfo.names.size());
    initHMatrixInfo(&mComplementHMatrixInfo, gImagePathInfo.names.size());
    ISImgInfoInit(gImagePathInfo.names.size(), &mISImgInfo);
    return ret;
}
//计算图像特征点初始化
int IS_computeImageFeaturesInit()
{
    ISInit(IS_Image_Width, IS_Image_Height);
    return 0;
}
//计算图像特征点
int IS_computeImageFeatures()
{
    ISFeature(gImagePathInfo.outFilePaths, gImagePathInfo.featureNames);
    return 0;
}
//计算图片特征点的匹配点对
int IS_Matching()
{
    ISMatching(gImagePathInfo.featureNames, gImagePathInfo.matchings);
    return 0;
}
//计算H矩阵
int IS_ComputeHMatrix()
{
    Alva_HMatrixes(gImagePathInfo.matchings, &mHMatrixInfo);

    return 0;
}

//手动拼接计算H矩阵
int IS_HandComputeHMatrix(const char* image1_name, const char* image2_name, int* twoImagePoint)
{
    Alva_TwoImage_HMatrixes(image1_name, image2_name, twoImagePoint, &mHMatrixInfo, gImagePathInfo.names);
    return 0;
}

//补全H矩阵
int IS_ComplementH()
{
    ISComplementH(gImagePathInfo.names.size(), &mHMatrixInfo, &mComplementHMatrixInfo);
    return 0;
}
//计算旋转图像
int IS_ImageRotate()
{    
    ISComputeISImgMulHInfo(gImagePathInfo.outFilePaths, gImagePathInfo.imageRotatesPath, &mISImgInfo, &mComplementHMatrixInfo);
    return 0;
}

//完成图像拼接
// int IS_ImageConformity(vector<string>& okImages, vector<string>& nOkImages, string& outImageName, string& baseImageName)
// {
//     ImageConformity(gImagePathInfo.imageRotatesPath, gImagePathInfo.outFilePaths, &mISImgInfo, baseImageName, 
//         gImagePathInfo.ISImgs, gImagePathInfo.names, okImages, nOkImages, outImageName);
//     return 0;
// }

int IS_ImageConformity(std::vector<std::string>** ppvFileName, int** ppnOK, char* outImageName, const char* baseImageName)
{
	*ppvFileName = &gImagePathInfo.names;
	*ppnOK = (int*)calloc(gImagePathInfo.names.size(), sizeof(int));
	ImageConformity(gImagePathInfo.imageRotatesPath, gImagePathInfo.outFilePaths, &mISImgInfo, baseImageName, 
	    gImagePathInfo.ISImgs, gImagePathInfo.names, *ppnOK, outImageName);
	return 0;
}

//释放资源
int IS_Unit()
{
    ISImgInfoUnit(&mISImgInfo);
    //释放H矩阵相关资源
    unitHMatrixInfo(&mHMatrixInfo, gImagePathInfo.names.size());
    unitHMatrixInfo(&mComplementHMatrixInfo, gImagePathInfo.names.size());

    //释放ASift资源
    ISUnit();
    return 0;
}