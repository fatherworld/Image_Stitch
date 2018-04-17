#ifndef _Image_Path_Info_h_
#define _Image_Path_Info_h_

class ImagePathInfo{
public:
    ImagePathInfo();
    ImagePathInfo(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath);
    void init(char* srcImagePath, char* dstImagePath, char* matchingPath, char* featurePath, char* imageRotationPath, char* imageISPath);
    ~ImagePathInfo();
    void clearImagePathInfo();
    vector<string> fileNames;
    vector<string> filePaths;
    vector<string> outFilePaths;
    vector<string> names;
    vector<vector<string>> matchings;
    vector<string> featureNames;
    vector<vector<string>> imageRotatesPath;
    vector<string> ISImgs;


private:
    string fileType;
    string outFilesType;
    string matchFileType;
    string featureType;
};




#endif