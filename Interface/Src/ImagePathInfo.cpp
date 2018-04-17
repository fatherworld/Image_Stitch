#include <vector>
#include <string.h>
#include <iostream>
using namespace std;
#include "ImagePathInfo.h"
#include "ReadDir.h"
ImagePathInfo::ImagePathInfo()
{

}
ImagePathInfo::ImagePathInfo(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath)
{    
    this->fileType = "*.bmp";
    this->outFilesType = ".bmp";
    this->matchFileType = ".txt";
    this->featureType = "feat.txt";
    //读取文件夹下的图片
    ReadDir(srcImagePath, this->fileType.c_str(), this->fileNames, this->filePaths);
    //得到图片的名称
    GetFileNames(this->fileType.c_str(), this->fileNames, this->names);
    //图片裁边后保存
    TidyBmpDir(dstImagePath, this->outFilesType.c_str(), this->names, this->outFilePaths);
    //两张图片特征点匹配后的点对存储的名称
    GetMatchingFileNames(matchingPath, this->matchFileType.c_str(), this->names, this->matchings);
    //每张图片的特征点的文件名
    GetFeatureFileNames(featurePath, this->featureType.c_str(), this->names, this->featureNames);
    //每张图片旋转后的文件名
    GetImageRotateFilePathNames(imageRotationPath, this->outFilesType.c_str(), this->names, this->imageRotatesPath);
    //保存最终拼接好的图像
    GetISPathNames(imageISPath, this->outFilesType.c_str(), this->names, this->ISImgs);
    
}
void ImagePathInfo::init(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath)
{

    this->fileType = "*.bmp";
    this->outFilesType = ".bmp";
    this->matchFileType = ".txt";
    this->featureType = "feat.txt";
    //读取文件夹下的图片
    ReadDir(srcImagePath, this->fileType.c_str(), this->fileNames, this->filePaths);
    //得到图片的名称
    GetFileNames(this->fileType.c_str(), this->fileNames, this->names);
    //图片裁边后保存
    TidyBmpDir(dstImagePath, this->outFilesType.c_str(), this->names, this->outFilePaths);
    //两张图片特征点匹配后的点对存储的名称
    GetMatchingFileNames(matchingPath, this->matchFileType.c_str(), this->names, this->matchings);
    //每张图片的特征点的文件名
    GetFeatureFileNames(featurePath, this->featureType.c_str(), this->names, this->featureNames);
    //每张图片旋转后的文件名
    GetImageRotateFilePathNames(imageRotationPath, this->outFilesType.c_str(), this->names, this->imageRotatesPath);
    //保存最终拼接好的图像
    GetISPathNames(imageISPath, this->outFilesType.c_str(), this->names, this->ISImgs);
}
void ImagePathInfo::clearImagePathInfo()
{
    unsigned int i = 0;
    this->fileNames.clear();
    this->filePaths.clear();
    this->outFilePaths.clear();
    this->names.clear();
    for (i = 0; i < this->matchings.size(); i++)
    {
        this->matchings[i].clear();
    }
    this->matchings.clear();

    this->featureNames.clear();
    for (i = 0; i < this->imageRotatesPath.size(); i++)
    {
        this->imageRotatesPath[i].clear();
    }
    this->imageRotatesPath.clear();
    this->ISImgs.clear();
}
ImagePathInfo::~ImagePathInfo()
{

}