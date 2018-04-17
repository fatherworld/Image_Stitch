#ifndef _Read_Dir_h_
#define _Read_Dir_h_

#include "ISStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "keyPoint.h"


#ifdef __cplusplus
};
#endif
//读取png文件的路径     读取文件的类型.png    文件的名字(包含后缀名)    最终包含路径的整个文件名
int ReadDir(const char* DirPath,const char* fileType, std::vector<std::string>& fileNames, std::vector<std::string>& filePaths);
//输出BMP文件的路径     .bmp      文件夹下所有文件的名字(不加后缀名)     存储输出BMP的整个文件名
int TidyBmpDir(const char* DirPath, const char* fileType, std::vector<std::string>& names, std::vector<std::string>& filePaths);
//获取文件名(不包含后缀名)
int GetFileNames(const char* fileType, std::vector<std::string>& fileNames, std::vector<std::string>& names);
//
int GetMatchingFileNames(const char * outPath, const char* outFileType, std::vector<std::string>& names, std::vector<std::vector<std::string>>& matchingName);
int GetFeatureFileNames(const char * featurePath, const char * featureType, std::vector<std::string>& names, std::vector<std::string>& featureNames);

int GetImageRotateFilePathNames(const char * rotatePath, const char * imageType, std::vector<std::string>& names, std::vector<std::vector<std::string>>& imageRotatesPath);
int GetISPathNames(const char* ISPath, const char* imageType, std::vector<std::string>& names, std::vector<std::string>& ISPaths);
void fprintfFeature(char * filename, keyPointInfo * mKeyPointInfo);
void fscanfFeature(char * filename, keyPointInfo * mKeyPointInfo);
void fpritfMatching(char * matchingName, MatchCoorList * matchCoorList);
void fscanfMatching(char* matchingName, MatchCoorList* matchCoorList);
#endif