#ifndef _Algorithm_h_
#define _Algorithm_h_
#include <vector>
#include <string>
//初始化路径
int IS_initPath(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath);
//图像预处理,获取到图像处理后的宽高
int IS_imagePretreatment();
//计算图像特征点初始化
int IS_computeImageFeaturesInit();
//计算图像特征点
int IS_computeImageFeatures();
//计算图片特征点的匹配点对
int IS_Matching();
//计算H矩阵
int IS_ComputeHMatrix();

//手动拼接计算H矩阵
int IS_HandComputeHMatrix(const char* image1_name, const char* image2_name, int* twoImagePoint);

//补全H矩阵
int IS_ComplementH();
//计算旋转图像
int IS_ImageRotate();

//完成图像拼接
//int IS_ImageConformity(std::vector<std::string>& okImages, std::vector<std::string>& nOkImages, std::string& outImageName, std::string& baseImageName);
int IS_ImageConformity(std::vector<std::string>** ppvFileName, int** ppnOK, char* outImageName, const char* baseImageName);
//释放资源
int IS_Unit();
#endif