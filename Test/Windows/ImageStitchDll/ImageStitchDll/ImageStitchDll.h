#define IMAGESTITCHDLL_API __declspec(dllexport)

//初始化路径
IMAGESTITCHDLL_API int __stdcall DLL_IS_initPath(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath);

//图像预处理,获取到图像处理后的宽高
IMAGESTITCHDLL_API int __stdcall DLL_IS_imagePretreatment();
//计算图像特征点初始化
IMAGESTITCHDLL_API int __stdcall DLL_IS_computeImageFeaturesInit();
//计算图像特征点
IMAGESTITCHDLL_API int __stdcall DLL_IS_computeImageFeatures();
//计算图片特征点的匹配点对
IMAGESTITCHDLL_API int __stdcall DLL_IS_Matching();
//计算H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_ComputeHMatrix();

//手动拼接计算H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_HandComputeHMatrix(const char* image1_name, const char* image2_name, int* twoImagePoint);

//补全H矩阵
IMAGESTITCHDLL_API int __stdcall DLL_IS_ComplementH();
//计算旋转图像
IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageRotate();

//完成图像拼接
//IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageConformity(std::vector<std::string>& okImages, std::vector<std::string>& nOkImages, std::string& outImageName, std::string& baseImageName);
IMAGESTITCHDLL_API int __stdcall DLL_IS_ImageConformity(std::vector<std::string>** ppvFileName, int** ppnOK, char* outImageName, const char* baseImageName);
//释放资源
IMAGESTITCHDLL_API int __stdcall DLL_IS_Unit();
