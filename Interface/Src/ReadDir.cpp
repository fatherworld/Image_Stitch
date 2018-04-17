
#include <direct.h>
#include <io.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
using namespace std;
#include "ComputeHMatrix.h"
#include "ReadDir.h"
#include "featMatching.h"

int ReadDir(const char* DirPath,const char* fileType, vector<string>& fileNames, vector<string>& filePaths)
{
    long file = 0;
    struct _finddata_t fileInfo;
    string p = DirPath;
    string t = DirPath;
    p.append(fileType);
    if((file = _findfirst(p.c_str(), &fileInfo)) == -1)
        return -1;
    else
    {
        do{
            string filename = fileInfo.name;
            fileNames.push_back(filename);
            string filePath;
            filePath = t;
            filePath.append(filename);
            filePaths.push_back(filePath);
        }while(_findnext(file, &fileInfo) == 0);
    }
    _findclose(file);
    return 0;
}

int TidyBmpDir(const char* DirPath, const char* fileType, vector<string>& names, vector<string>& filePaths)
{
    string p = DirPath;
    for(unsigned int i = 0; i < names.size(); i ++)
    {
        string filePath = p;
        filePath.append(names[i]);
        filePath.append(fileType);
        filePaths.push_back(filePath);
    }
    return 0;
}



int GetFileNames(const char* fileType, vector<string>& fileNames, vector<string>& names)
{
    string p = fileType;
    int typeLen = p.size() - 1;
    for(unsigned int i = 0; i < fileNames.size(); i ++)
    {
        names.push_back(fileNames[i].substr(0, fileNames[i].size() - typeLen));
    }

    return 0;
}

int GetMatchingFileNames(const char * outPath, const char* outFileType, vector<string>& names, vector<vector<string>>& matchingName)
{
    string str = outFileType;
    string p = outPath;
    matchingName.resize(names.size());
    for(unsigned int i = 0; i < names.size() - 1; i ++)
    {
        for(unsigned int j = i + 1; j < names.size(); j ++)
        {
            string match;
            match = p;
            match.append(names[i]);
            match.append("_And_");
            match.append(names[j]);
            match.append(outFileType);
            matchingName[i].push_back(match);
        }
    }

    return 0;
}
int GetFeatureFileNames(const char * featurePath, const char * featureType, vector<string>& names, vector<string>& featureNames)
{
    string str = featureType;
    string featP = featurePath;
    for(unsigned int i = 0; i < names.size(); i ++)
    {
        string featureName;
        featureName = featurePath;
        featureName.append(names[i]);

        featureName.append(str);
        featureNames.push_back(featureName);
    }
    return 0;
}
int GetImageRotateFilePathNames(const char * rotatePath, const char * imageType, std::vector<std::string>& names, std::vector<std::vector<std::string>>& imageRotatesPath)
{
    string imgType = imageType;
    string rPath = rotatePath;
    imageRotatesPath.resize(names.size());
    for(unsigned int i = 0; i < names.size(); i ++)
    {        
        for(unsigned int j = 0; j < names.size(); j ++)
        {
            string name;
            name = rPath;
            name.append(names[i]);
            name.append("_");
            name.append(names[j]);
            name.append("r");
            name.append(imgType);
            imageRotatesPath[i].push_back(name);
        }
    }
    return 0;
}
int GetISPathNames(const char* ISPath, const char* imageType, std::vector<std::string>& names, std::vector<std::string>& ISPaths)
{
    string imgType = imageType;
    string rPath = ISPath;
    for(unsigned int i = 0; i < names.size(); i ++)
    {    
        string name;
        name = rPath;
        name.append(names[i]);
        name.append("IS");
        name.append(imgType);
        ISPaths.push_back(name);
    }
    return 0;
}
void fprintfFeature(char * filename, keyPointInfo * mKeyPointInfo)
{
    string filePath = filename;
    ofstream fout(filePath);

    int keys = mKeyPointInfo->count;

    fout << "keys=" << keys << endl;

    for(int z = 0; z < keys; z ++)
    {
        fout << mKeyPointInfo->mkeyPoint[z].x << "," 
            << mKeyPointInfo->mkeyPoint[z].y << endl;
        fout << mKeyPointInfo->mkeyPoint[z].angle << ","
            << mKeyPointInfo->mkeyPoint[z].scale << endl;
        for(int w = 0; w < 127; w ++)
        {
            fout << mKeyPointInfo->mkeyPoint[z].vec[w] << ",";
        }
        fout << mKeyPointInfo->mkeyPoint[z].vec[127] << endl;
    }

    fout.close();
}
static char * xmlStr = NULL;
static int LoadXML(char * xmlPath)
{
    FILE * file = NULL;
    size_t length = 0;
    fopen_s(&file, xmlPath, "rb");
    if(NULL == file)
    {
        return -2;
    }
    fseek(file, 0, SEEK_END);

    length = ftell(file);

    fseek(file, 0, SEEK_SET);

    if(xmlStr == NULL)
    {
        xmlStr = (char*)malloc(length);
    }else{
        free(xmlStr);
        xmlStr = NULL;
        xmlStr = (char*)malloc(length);
    }

    fread(xmlStr, 1, length, file);
    fclose(file);
    return 0;
}
void fscanfFeature(char * filename, keyPointInfo * mKeyPointInfo)
{
    char* ptr = 0, *nextPtr = 0;
    LoadXML(filename);
    ptr = xmlStr;
    string str = "keys=";
    int len = str.size();

    int keys;
    ptr = strstr(ptr, "keys=");
    ptr += len;
    nextPtr = strstr(ptr, "\n");
    *nextPtr = '\0';
    sscanf_s(ptr, "%d", &keys);
    mKeyPointInfo->count = keys;
    for(int z = 0; z < keys; z ++)
    {
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, ",");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].x));

        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, "\n");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].y));

        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, ",");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].angle));

        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, "\n");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].scale));
        for(int w = 0; w < 127; w ++)
        {
            ptr = nextPtr + 1;
            nextPtr = strstr(ptr, ",");
            *nextPtr = '\0';
            sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].vec[w]));
        }
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, "\n");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(mKeyPointInfo->mkeyPoint[z].vec[127]));
    }
}
void fpritfMatching(char * matchingName, MatchCoorList * matchCoorList)
{
    FILE* fp = NULL;
    fopen_s(&fp, matchingName, "w+");
    int matchingNumber = matchCoorList->count;
    fprintf(fp, "%d\n", matchingNumber);
    for(int i = 0; i < matchingNumber; i ++)
    {
        fprintf(fp, "%f,%f,%f,%f\n", matchCoorList->matchCoor[i].coor1.x, matchCoorList->matchCoor[i].coor1.y,
            matchCoorList->matchCoor[i].coor2.x, matchCoorList->matchCoor[i].coor2.y);
    }
    fclose(fp);
}

void fscanfMatching(char* matchingName, MatchCoorList* matchCoorList)
{
    char* ptr = 0, *nextPtr = 0;
    int matchingNumber = 0;
    LoadXML(matchingName);
    ptr = xmlStr;

    nextPtr = strstr(ptr, "\n");
    *nextPtr = '\0';
    sscanf_s(ptr, "%d", &matchingNumber);

    matchCoorList->count = matchingNumber;

    for(int i = 0; i < matchingNumber; i ++)
    {
        //由于是以第一张图像为基准因此换一下坐标
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, ",");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(matchCoorList->matchCoor[i].coor2.x));
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, ",");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(matchCoorList->matchCoor[i].coor2.y));
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, ",");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(matchCoorList->matchCoor[i].coor1.x));
        ptr = nextPtr + 1;
        nextPtr = strstr(ptr, "\n");
        *nextPtr = '\0';
        sscanf_s(ptr, "%f", &(matchCoorList->matchCoor[i].coor1.y));
    }
}