#ifndef _Cut_Image_h_
#define _Cut_Image_h_
#include <vector>
#include <string.h>
#include <iostream>
using namespace std;

int CutAllImages(vector<string>& outImagesPath, vector<string>& inImagesPath, int& width, int& height, vector<int>& allImageWidth, vector<int>& allImageHeight);

int BmpToGrayBit32(unsigned char* imageData, float** outImage, int width, int height);

#endif