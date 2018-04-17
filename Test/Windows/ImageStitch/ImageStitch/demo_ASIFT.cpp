#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;
#include "Algorithm.h"
int main(int argc, char **argv)
{
    int ret = 0;
    if(argc != 7)
    {
        std::cerr << "introduction path error." << std::endl;
        std::cerr << "1. src image path" << std::endl;
        std::cerr << "2. dst image path" << std::endl;
        std::cerr << "3. matching path" << std::endl;
        std::cerr << "4. feature path" << std::endl;
        std::cerr << "5. image Rotation path" << std::endl;
        std::cerr << "6. image IS path" << std::endl;
        return -1;
    }
    string outImageName;
    string baseImageName = "00002";
    //路径初始化
    IS_initPath(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    //图像预处理
    ret = IS_imagePretreatment();
    if (ret)
        return ret;
    //计算图像特征点初始化
    IS_computeImageFeaturesInit();
    //计算图像特征点
    IS_computeImageFeatures();
    //计算图像匹配点
    IS_Matching();
    //计算H矩阵
    IS_ComputeHMatrix();
    //补全H矩阵
    IS_ComplementH();
    //图像旋转
    IS_ImageRotate();
//    vector<string> okImages;
//    vector<string> nOkImages;
    int * nOkImages  ;
    std::vector<std::string>*okImages;
    //图像拼接
    IS_ImageConformity(&okImages, &nOkImages, (char*)outImageName.c_str(), baseImageName.c_str());
    //资源释放
    IS_Unit();
    system("pause");
    return 0;
}
