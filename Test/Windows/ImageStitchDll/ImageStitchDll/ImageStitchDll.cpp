// ImageStitchDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include<string>
#include <vector>
#include "ImageStitchDll.h"
#include "Algorithm.h"
using namespace std;
//初始化路径
IMAGESTITCHDLL_API int __stdcall DLL_IS_initPath(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath)
{
	IS_initPath(srcImagePath, dstImagePath, matchingPath, featurePath, imageRotationPath, imageISPath);
	return 0;
}

//图像预处理,获取到图像处理后的宽高
IMAGESTITCHDLL_API int __stdcall DLL_IS_imagePretreatment()
{
	IS_imagePretreatment();
	return 0;
}
//计算图像特征点初始化
IMAGESTITCHDLL_API int __stdcall DLL_IS_computeImageFeaturesInit()
{
	//计算图像特征点初始化
	IS_computeImageFeaturesInit();
	return 0;
}
//计算图像特征点
IMAGESTITCHDLL_API int __stdcall DLL_IS_computeImageFeatures()
{
	//计算图像特征点
	IS_computeImageFeatures();
	return 0;
}
//计算图片特征点的匹配点对
IMAGESTITCHDLL_API int __stdcall DLL_IS_Matching()
{
	//计算图片特征点的匹配点对
	IS_Matching();
	return 0;
}
//计算H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_ComputeHMatrix()
{
	//计算H矩阵
	IS_ComputeHMatrix();
	return 0;
}

//手动拼接计算H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_HandComputeHMatrix(const char* image1_name, const char* image2_name, int* twoImagePoint)
{
	if (image1_name == NULL || image2_name == NULL || *image1_name == '\0' || *image2_name == '\0')
	{
		return -1;
	}
	//手动拼接计算H矩阵
	IS_HandComputeHMatrix(image1_name, image2_name, twoImagePoint);
	return 0;
}

//补全H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_ComplementH()
{
	//补全H矩阵
	IS_ComplementH();
	return 0;
}
//计算旋转图像
IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageRotate()
{
	//计算旋转图像
	IS_ImageRotate();

	return 0;
}

//完成图像拼接
// IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageConformity(vector<string>& okImages, vector<string>& nOkImages, string& outImageName, string& baseImageName)
// {
// 	//完成图像拼接
// 	IS_ImageConformity(okImages, nOkImages, outImageName, baseImageName);
// 	return 0;
// }

IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageConformity(std::vector<std::string>** ppvFileName, int** ppnOK, char* outImageName, const char* baseImageName)
{
	IS_ImageConformity(ppvFileName, ppnOK, outImageName, baseImageName);
	return 0;
}

//释放资源
IMAGESTITCHDLL_API int __stdcall DLL_IS_Unit()
{
	//释放资源
	IS_Unit();
	return 0;
}

