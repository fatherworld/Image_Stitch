#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "BMP.h"
#include "ComputeHMatrix.h"
#include "Dump.h"
using namespace std;
#include "ImageIS.h"
#include "ISLog.h"

static int ImageRotate(float* srcImage, float* dstImage, int srcWidth, int srcHeight, int dstWidth, int dstHeight, float* H, int offsetX, int offsetY)
{

    int x, y;

    //  H'
    float H_inver[9] = {0.0f};
    //  |A|
    float H_A = 0.0f;

    H_A = H[0]*H[4] - H[1]*H[3];

    //      | H[4]   -H[1]     H[1]H[5]-H[2]H[4] |
    //      |-H[3]    H[0]   -(H[0]H[5]-H[2]H[3])|
    //      |  0       0       H[0]H[4]-H[1]H[3] |

    H_inver[0] = H[4] / H_A;
    H_inver[1] = -H[1] / H_A;
    H_inver[2] = (H[1]*H[5]-H[2]*H[4]) / H_A;
    H_inver[3] = -H[3] / H_A;
    H_inver[4] = H[0] / H_A;
    H_inver[5] = -(H[0]*H[5]-H[2]*H[3]) / H_A;
    H_inver[6] = 0.0f;
    H_inver[7] = 0.0f;
    H_inver[8] = 1.0f;

    int ceilSrcY, floorSrcY;
    int ceilSrcX, floorSrcX;
    float srcX, srcY;
    float weightY1, weightY2;
    float weightX1, weightX2;

    for(y = 0; y < dstHeight; y ++)
    {
        for(x = 0; x < dstWidth; x ++)
        {
            int x_t = x + offsetX;
            int y_t = y + offsetY;

            srcX = H_inver[0] * x_t + H_inver[1] * y_t + H_inver[2];
            srcY = H_inver[3] * x_t + H_inver[4] * y_t + H_inver[5];


            if(srcX < 0.0f || srcY < 0.0f || ceilf(srcX) >= srcWidth || ceilf(srcY) >= srcHeight)
            {
                (dstImage)[(y * dstWidth + x) * 3 + 0] = 0.0f;
                (dstImage)[(y * dstWidth + x) * 3 + 1] = 0.0f;
                (dstImage)[(y * dstWidth + x) * 3 + 2] = 0.0f;
                continue;
            }

            ceilSrcX = (int)ceilf(srcX);
            ceilSrcY = (int)ceilf(srcY);

            ceilSrcX = AlvaMin((srcWidth - 1), ceilSrcX);
            ceilSrcY = AlvaMin((srcHeight - 1), ceilSrcY);

            floorSrcX = AlvaMax((ceilSrcX - 1), 0);
            floorSrcY = AlvaMax((ceilSrcY - 1), 0);

            weightX1 = (float)ceilSrcX - srcX;
            weightX2 = 1.0f - weightX1;

            weightY1 = (float)ceilSrcY - srcY;
            weightY2 = 1.0f - weightY1;

            (dstImage)[(y * dstWidth + x) * 3 + 0] =   weightY1 * (weightX1 * srcImage[(floorSrcY * srcWidth + floorSrcX) * 3 + 0] + weightX2 * srcImage[(floorSrcY * srcWidth + ceilSrcX) * 3 + 0]) + 
                                                       weightY2 * (weightX1 * srcImage[( ceilSrcY * srcWidth + floorSrcX) * 3 + 0] + weightX2 * srcImage[( ceilSrcY * srcWidth + ceilSrcX) * 3 + 0]);
            
            (dstImage)[(y * dstWidth + x) * 3 + 1] =   weightY1 * (weightX1 * srcImage[(floorSrcY * srcWidth + floorSrcX) * 3 + 1] + weightX2 * srcImage[(floorSrcY * srcWidth + ceilSrcX) * 3 + 1]) + 
                                                       weightY2 * (weightX1 * srcImage[( ceilSrcY * srcWidth + floorSrcX) * 3 + 1] + weightX2 * srcImage[( ceilSrcY * srcWidth + ceilSrcX) * 3 + 1]);

            (dstImage)[(y * dstWidth + x) * 3 + 2] =   weightY1 * (weightX1 * srcImage[(floorSrcY * srcWidth + floorSrcX) * 3 + 2] + weightX2 * srcImage[(floorSrcY * srcWidth + ceilSrcX) * 3 + 2]) + 
                                                        weightY2 * (weightX1 * srcImage[( ceilSrcY * srcWidth + floorSrcX) * 3 + 2] + weightX2 * srcImage[( ceilSrcY * srcWidth + ceilSrcX) * 3 + 2]);

        }
    }
    return 0;
}


//*dstImage 用完必须释放
static int TwoImageComputeCoor(float* srcImage, float** dstImage, int srcWidth, int srcHeight, int* dstWidth, int* dstHeight, 
                               int* offsetX, int* offsetY, float* H, int baseWidth, int baseHeight)
{
    int x1, y1, x2, y2, x3, y3, x4, y4;
    int xMax, xMin, yMax, yMin;
    int x, y;

    x = 0;
    y = 0;
    x1 = (int)ceilf(x * H[0] + y * H[1] + H[2]);
    y1 = (int)ceilf(x * H[3] + y * H[4] + H[5]);

    x = 0;
    y = srcHeight - 1;
    x2 = (int)ceilf(x * H[0] + y * H[1] + H[2]);
    y2 = (int)ceilf(x * H[3] + y * H[4] + H[5]);

    x = srcWidth - 1;
    y = srcHeight - 1;
    x3 = (int)ceilf(x * H[0] + y * H[1] + H[2]);
    y3 = (int)ceilf(x * H[3] + y * H[4] + H[5]);

    x = srcWidth - 1;
    y = 0;
    x4 = (int)ceilf(x * H[0] + y * H[1] + H[2]);
    y4 = (int)ceilf(x * H[3] + y * H[4] + H[5]);

    xMax = x1;
    xMin = x1;
    yMax = y1;
    yMin = y1;

    if(xMax < x2){ xMax = x2;}
    if(xMax < x3){ xMax = x3;}
    if(xMax < x4){ xMax = x4;}

    if(xMin > x2){ xMin = x2;}
    if(xMin > x3){ xMin = x3;}
    if(xMin > x4){ xMin = x4;}

    if(yMax < y2){ yMax = y2;}
    if(yMax < y3){ yMax = y3;}
    if(yMax < y4){ yMax = y4;}
       
    if(yMin > y2){ yMin = y2;}
    if(yMin > y3){ yMin = y3;}
    if(yMin > y4){ yMin = y4;}

    //计算出图像旋转后的尺寸
    *dstWidth = (xMax - xMin) + 1;
    *dstHeight = (yMax - yMin) + 1;

    //图像相对于baseImage的偏移量
    *offsetX = xMin;
    *offsetY = yMin;

    (*dstImage) = (float*)calloc(1, sizeof(float) * (*dstWidth) * (*dstHeight) * 3);
	CHECK_ALLOC_P((*dstImage));
    ImageRotate(srcImage, *dstImage, srcWidth, srcHeight, *dstWidth, *dstHeight, H, *offsetX, *offsetY);

    return 0;
}


static int ucharToFloat(unsigned char* src, float* dst, int width, int height)
{
    int x, y;
    for(y = 0; y < height; y ++)
    {
        for(x = 0; x < width; x ++)
        {
            int r = (int)src[(y * width + x) * 3 + 0];
            int g = (int)src[(y * width + x) * 3 + 1];
            int b = (int)src[(y * width + x) * 3 + 2];

            dst[(y * width + x) * 3 + 0] = (float)r;
            dst[(y * width + x) * 3 + 1] = (float)g;
            dst[(y * width + x) * 3 + 2] = (float)b;
        }
    }
    return 0;
}
static int floatToUchar(float* src, unsigned char* dst, int width, int height)
{
    int x, y;
    float maxValue = -10.0f, minValue = 1000.0f;
    for(y = 0; y < height; y ++)
    {
        for(x = 0; x < width; x ++)
        {
            float r = src[(y * width + x) * 3 + 0];
            float g = src[(y * width + x) * 3 + 1];
            float b = src[(y * width + x) * 3 + 2];
            maxValue = AlvaMax(maxValue, r);
            maxValue = AlvaMax(maxValue, g);
            maxValue = AlvaMax(maxValue, b);
            minValue = AlvaMin(minValue, r);
            minValue = AlvaMin(minValue, g);
            minValue = AlvaMin(minValue, b);
        }
    }
    for(y = 0; y < height; y ++)
    {
        for(x = 0; x < width; x ++)
        {
            float r = src[(y * width + x) * 3 + 0];
            float g = src[(y * width + x) * 3 + 1];
            float b = src[(y * width + x) * 3 + 2];
            int ir = (int)(((r - minValue) / (maxValue - minValue)) * 255.0f);
            int ig = (int)(((g - minValue) / (maxValue - minValue)) * 255.0f);
            int ib = (int)(((b - minValue) / (maxValue - minValue)) * 255.0f);

            dst[(y * width + x) * 3 + 0] = (unsigned char)ir;
            dst[(y * width + x) * 3 + 1] = (unsigned char)ig;
            dst[(y * width + x) * 3 + 2] = (unsigned char)ib;
        }
    }
    return 0;
}
//基准图和a图像移动
static int base_a_Move(float* a, float* b, float* c, int aWidth, int aHeight, int bWidth, int bHeight, int cWidth, int cHeight,
                       int aOffsetX, int aOffsetY, int baseOffsetX, int baseOffsetY)
{
    int x, y;
    for(y = 0; y < cHeight; y ++)
    {
        for(x = 0; x < cWidth; x ++)
        {
            int cIndex = y * cWidth + x;
            int isFillPixelBase = 0;
            int isFillPixelA = 0;
            if(y >= baseOffsetY && x >= baseOffsetX && y < (baseOffsetY + bHeight) && x < (baseOffsetX + bWidth))
            {
                isFillPixelBase = 1;
            }

            if(y >= aOffsetY && x >= aOffsetX && y < (aOffsetY + aHeight) && x < (aOffsetX + aWidth))
            {
                isFillPixelA = 1;
            }

            if(isFillPixelBase == 1 && isFillPixelA == 1)
            {
                int baseX = x - baseOffsetX;
                int baseY = y - baseOffsetY;
                int aX = x - aOffsetX;
                int aY = y - aOffsetY;

                if(b[(baseY * bWidth + baseX) * 3 + 0] > 0.0f && a[(aY * aWidth + aX) * 3 + 0] > 0.0f)
                {
                    c[cIndex * 3 + 0] = (b[(baseY * bWidth + baseX) * 3 + 0] + a[(aY * aWidth + aX) * 3 + 0]) / 2.0f;
					
                }else if(fabs(b[(baseY * bWidth + baseX) * 3 + 0]) < 0.000001f)
                {
                    c[cIndex * 3 + 0] = a[(aY * aWidth + aX) * 3 + 0];
                }else if(fabs(a[(aY * aWidth + aX) * 3 + 0]) < 0.000001f)
                {
                    c[cIndex * 3 + 0] = b[(baseY * bWidth + baseX) * 3 + 0];
                }


                if(b[(baseY * bWidth + baseX) * 3 + 1] > 0.0f && a[(aY * aWidth + aX) * 3 + 1] > 0.0f)
                {
                    c[cIndex * 3 + 1] = (b[(baseY * bWidth + baseX) * 3 + 1] + a[(aY * aWidth + aX) * 3 + 1]) / 2.0f;
                }else if(fabs(b[(baseY * bWidth + baseX) * 3 + 1]) < 0.000001f)
                {
                    c[cIndex * 3 + 1] = a[(aY * aWidth + aX) * 3 + 1];
                }else if(fabs(a[(aY * aWidth + aX) * 3 + 1]) < 0.000001f)
                {
                    c[cIndex * 3 + 1] = b[(baseY * bWidth + baseX) * 3 + 1];
                }


                if(b[(baseY * bWidth + baseX) * 3 + 2] > 0.0f && a[(aY * aWidth + aX) * 3 + 2] > 0.0f)
                {
                    c[cIndex * 3 + 2] = (b[(baseY * bWidth + baseX) * 3 + 2] + a[(aY * aWidth + aX) * 3 + 2]) / 2.0f;
                }else if(fabs(b[(baseY * bWidth + baseX) * 3 + 2]) < 0.000001f)
                {
                    c[cIndex * 3 + 2] = a[(aY * aWidth + aX) * 3 + 2];
                }else if(fabs(a[(aY * aWidth + aX) * 3 + 2]) < 0.000001f)
                {
                    c[cIndex * 3 + 2] = b[(baseY * bWidth + baseX) * 3 + 2];
                }


            }else if(isFillPixelBase == 1 && isFillPixelA == 0)
            {
                int baseX = x - baseOffsetX;
                int baseY = y - baseOffsetY;
                c[cIndex * 3 + 0] = b[(baseY * bWidth + baseX) * 3 + 0];
                c[cIndex * 3 + 1] = b[(baseY * bWidth + baseX) * 3 + 1];
                c[cIndex * 3 + 2] = b[(baseY * bWidth + baseX) * 3 + 2];
            }else if(isFillPixelBase == 0 && isFillPixelA == 1)
            {
                int aX = x - aOffsetX;
                int aY = y - aOffsetY;
                c[cIndex * 3 + 0] = a[(aY * aWidth + aX) * 3 + 0];
                c[cIndex * 3 + 1] = a[(aY * aWidth + aX) * 3 + 1];
                c[cIndex * 3 + 2] = a[(aY * aWidth + aX) * 3 + 2];
            }

        }
    }
    return 0;
}

//a 图像  b基准图  c融合后图像
static int Image_Move(float* a, float* b, float** c, int aWidth, int aHeight, int bWidth, int bHeight,
                      int cWidth, int cHeight, int offsetX, int offsetY)
{
    *c = (float*)calloc(1, sizeof(float) * cWidth * cHeight * 3);
	CHECK_ALLOC_P(*c);
    int baseOffsetX, baseOffsetY, aOffsetX, aOffsetY;
    //由于x,y方向偏移量都为负,表示基准图b需要y方向偏移|offsetY|,x方向偏移|offsetX|
    // flag = 1;
    if(offsetX < 0 && offsetY < 0)
    {
        baseOffsetX = -offsetX;
        baseOffsetY = -offsetY;
        aOffsetX = 0;
        aOffsetY = 0;
    }
    //由于x方向偏移量为负,y方向偏移量为正,表示基准图b向x方向偏移|offsetX|  a图像向y方向偏移|offsetY|
    if(offsetX < 0 && offsetY >= 0)
    {
        baseOffsetX = -offsetX;
        baseOffsetY = 0;
        aOffsetX = 0;
        aOffsetY = offsetY;
    }
    //由于x方向偏移量为正,y方向偏移量为负,表示基准图b需要y方向偏移|offsetY|,a图像需要x方向偏移|offsetX|
    if(offsetX >= 0 && offsetY < 0)
    {
        baseOffsetX = 0;
        baseOffsetY = -offsetY;
        aOffsetX = offsetX;
        aOffsetY = 0;
    }
    //由于x方向偏移量为正,y方向偏移量为正,表示基准图b不动,a图像需要x方向偏移|offsetX|y方向偏移|offsetY|
    if(offsetX >= 0 && offsetY >= 0)
    {
        baseOffsetX = 0;
        baseOffsetY = 0;
        aOffsetX = offsetX;
        aOffsetY = offsetY;
    }

    base_a_Move(a, b, *c, aWidth, aHeight, bWidth, bHeight, cWidth, cHeight,
        aOffsetX, aOffsetY, baseOffsetX, baseOffsetY);

    return 0;
}


int ImageConformity(vector<vector<string>>& rotateImages, vector<string>& cutImages, ISImgInfo* mISImgInfo, 
    string& baseImageName, vector<string>& ISImgsPath, vector<string>& names, vector<string>& okImages, vector<string>& nOkImages, string& outImageName)
{
    int leftUpX = 10000, leftUpY = 10000;
    int rightDownX, rightDownY;
    int baseImageWidth, baseImageHeight;
    int baseImageIndex = 0;
    int OutImageWidth, OutImageHeight;

    for(unsigned int i = 0; i < names.size(); i ++)
    {
        if(baseImageName == names[i])
        {
            baseImageIndex = i;
            break;
        }
    }
    vector<int> okImagesIndex;
    vector<int> nOKImagesIndex;

    baseImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].baseImgSize.width;
    baseImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].baseImgSize.height;

    rightDownX = baseImageWidth;
    rightDownY = baseImageHeight;

    for(int i = 0; i < mISImgInfo->baseImgInfoList[baseImageIndex].listCount; i ++)
    {
        int offsetX, offsetY;
        int thisImageWidth, thisImageHeight;
        offsetX = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetX;
        offsetY = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetY;
        thisImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.width;
        thisImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.height;
        leftUpX = AlvaMin(leftUpX, offsetX);
        leftUpY = AlvaMin(leftUpY, offsetY);
        rightDownX = AlvaMax(rightDownX, thisImageWidth + offsetX);
        rightDownY = AlvaMax(rightDownY, thisImageHeight + offsetY);
    }

    leftUpX = AlvaMin(leftUpX, 0);
    leftUpY = AlvaMin(leftUpY, 0);

    //计算出输出图像的尺寸
    OutImageWidth = rightDownX - leftUpX;
    OutImageHeight = rightDownY - leftUpY;

    IMAGE OutImage;
    IMAGE BaseImage;
    IMAGE img;
    OutImage.width = OutImageWidth;
    OutImage.height = OutImageHeight;
    OutImage.channels = 3;
    OutImage.data = (unsigned char *)calloc(1, sizeof(unsigned char) * OutImageWidth * OutImageHeight * 3);
	CHECK_ALLOC_P(OutImage.data);
    LoadBMP(cutImages[baseImageIndex].c_str(), &BaseImage, BIT24);
    int baseWidth, baseHeight;
    int baseTrueOffsetX, baseTrueOffsetY;
    baseWidth = BaseImage.width;
    baseHeight = BaseImage.height;
    baseTrueOffsetX = -leftUpX;
    baseTrueOffsetY = -leftUpY;
    
    for(int y = 0; y < OutImageHeight; y ++)
    {
        for(int x = 0; x < OutImageWidth; x ++)
        {
            int imgx = x - baseTrueOffsetX;
            int imgy = y - baseTrueOffsetY;
            if(y >= baseTrueOffsetY && x >= baseTrueOffsetX && y < baseTrueOffsetY + baseHeight && x < baseTrueOffsetX + baseWidth)
            {
                if(BaseImage.data[(imgy * baseWidth + imgx) * 3 + 0] != 0)
                {
                    OutImage.data[(y * OutImageWidth + x) * 3 + 0] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 0];
                }
                if(BaseImage.data[(imgy * baseWidth + imgx) * 3 + 1] != 0)
                {
                    OutImage.data[(y * OutImageWidth + x) * 3 + 1] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 1];
                }
                if(BaseImage.data[(imgy * baseWidth + imgx) * 3 + 2] != 0)
                {
                    OutImage.data[(y * OutImageWidth + x) * 3 + 2] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 2];
                }
            }
        }
    }


    //WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
    okImagesIndex.push_back(baseImageIndex);
    for(int i = 0; i < mISImgInfo->baseImgInfoList[baseImageIndex].listCount; i ++)
    {
        if(i == baseImageIndex)
            continue;
        if(mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].isHOK == -1)
        {
            nOKImagesIndex.push_back(i);
            continue;
        }

        okImagesIndex.push_back(i);

        int offsetX, offsetY;
        int thisImageWidth, thisImageHeight;
        int trueOffsetX, trueOffsetY;
        offsetX = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetX;
        offsetY = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetY;
        thisImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.width;
        thisImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.height;

        trueOffsetX = offsetX - leftUpX;
        trueOffsetY = offsetY - leftUpY;
        LoadBMP(rotateImages[baseImageIndex][i].c_str(), &img, BIT24);

        for(int y = 0; y < OutImageHeight; y ++)
        {
            for(int x = 0; x < OutImageWidth; x ++)
            {
                int imgx = x - trueOffsetX;
                int imgy = y - trueOffsetY;
                if(y >= trueOffsetY && x >= trueOffsetX && y < trueOffsetY + thisImageHeight && x < trueOffsetX + thisImageWidth)
                {
                    if(img.data[(imgy * img.width + imgx) * 3 + 0] != 0)
                    {
                        OutImage.data[(y * OutImageWidth + x) * 3 + 0] = img.data[(imgy * img.width + imgx) * 3 + 0];
                    }
                    if(img.data[(imgy * img.width + imgx) * 3 + 1] != 0)
                    {
                        OutImage.data[(y * OutImageWidth + x) * 3 + 1] = img.data[(imgy * img.width + imgx) * 3 + 1];
                    }
                    if(img.data[(imgy * img.width + imgx) * 3 + 2] != 0)
                    {
                        OutImage.data[(y * OutImageWidth + x) * 3 + 2] = img.data[(imgy * img.width + imgx) * 3 + 2];
                    }
                }
            }
        }
        //WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
        freeImage(&img);
    }

    for(unsigned int i = 0; i < okImagesIndex.size(); i ++)
    {
        okImages.push_back(names[okImagesIndex[i]]);
        cout << okImages[i] << endl;
    }
    for(unsigned int i = 0; i < nOKImagesIndex.size(); i ++)
    {
        nOkImages.push_back(names[nOKImagesIndex[i]]);
    }


    WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
    outImageName = ISImgsPath[baseImageIndex];
    freeImage(&BaseImage);

    return 0;
}


/* ImageConformity(gImagePathInfo.imageRotatesPath, gImagePathInfo.outFilePaths, &mISImgInfo, baseImageName, 
    gImagePathInfo.ISImgs, gImagePathInfo.names, *ppnOK, outImageName); */

int ImageConformity(vector<vector<string>>& rotateImages, vector<string>& cutImages, ISImgInfo* mISImgInfo, const char* baseImageName, vector<string>& ISImgsPath, vector<string>& names, int* pnOK, char* outImageName)
{
	int leftUpX = 10000, leftUpY = 10000;
	int rightDownX, rightDownY;
	int baseImageWidth, baseImageHeight;
	int baseImageIndex = 0;
	int OutImageWidth, OutImageHeight;

	for (unsigned int i = 0; i < names.size(); i++)
	{
		if (baseImageName == names[i])
		{
			baseImageIndex = i;
			break;
		}
	}
	vector<int> okImagesIndex;
	vector<int> nOKImagesIndex;

	baseImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].baseImgSize.width;
	baseImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].baseImgSize.height;

	rightDownX = baseImageWidth;
	rightDownY = baseImageHeight;

	for (int i = 0; i < mISImgInfo->baseImgInfoList[baseImageIndex].listCount; i++)
	{
		int offsetX, offsetY;
		int thisImageWidth, thisImageHeight;
		offsetX = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetX;
		offsetY = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetY;
		thisImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.width;
		thisImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.height;
		leftUpX = AlvaMin(leftUpX, offsetX);
		leftUpY = AlvaMin(leftUpY, offsetY);
		rightDownX = AlvaMax(rightDownX, thisImageWidth + offsetX);
		rightDownY = AlvaMax(rightDownY, thisImageHeight + offsetY);
	}

	leftUpX = AlvaMin(leftUpX, 0);
	leftUpY = AlvaMin(leftUpY, 0);

	//计算出输出图像的尺寸
	OutImageWidth = rightDownX - leftUpX;
	OutImageHeight = rightDownY - leftUpY;

	IMAGE OutImage;
	IMAGE BaseImage;
	IMAGE img;
	OutImage.width = OutImageWidth;
	OutImage.height = OutImageHeight;
	OutImage.channels = 3;
	OutImage.data = (unsigned char *)calloc(1, sizeof(unsigned char) * OutImageWidth * OutImageHeight * 3);
	CHECK_ALLOC_P(OutImage.data);
	LoadBMP(cutImages[baseImageIndex].c_str(), &BaseImage, BIT24);
	int baseWidth, baseHeight;
	int baseTrueOffsetX, baseTrueOffsetY;
	baseWidth = BaseImage.width;
	baseHeight = BaseImage.height;
	baseTrueOffsetX = -leftUpX;
	baseTrueOffsetY = -leftUpY;

	for (int y = 0; y < OutImageHeight; y++)
	{
		for (int x = 0; x < OutImageWidth; x++)
		{
			int imgx = x - baseTrueOffsetX;
			int imgy = y - baseTrueOffsetY;
			if (y >= baseTrueOffsetY && x >= baseTrueOffsetX && y < baseTrueOffsetY + baseHeight && x < baseTrueOffsetX + baseWidth)
			{
				if (BaseImage.data[(imgy * baseWidth + imgx) * 3 + 0] != 0)
				{
					OutImage.data[(y * OutImageWidth + x) * 3 + 0] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 0];
				}
				if (BaseImage.data[(imgy * baseWidth + imgx) * 3 + 1] != 0)
				{
					OutImage.data[(y * OutImageWidth + x) * 3 + 1] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 1];
				}
				if (BaseImage.data[(imgy * baseWidth + imgx) * 3 + 2] != 0)
				{
					OutImage.data[(y * OutImageWidth + x) * 3 + 2] = BaseImage.data[(imgy * baseWidth + imgx) * 3 + 2];
				}
			}
		}
	}


	//WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
	okImagesIndex.push_back(baseImageIndex);
	for (int i = 0; i < mISImgInfo->baseImgInfoList[baseImageIndex].listCount; i++)
	{
		if (i == baseImageIndex)
			continue;
		if (mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].isHOK == -1)
		{
			nOKImagesIndex.push_back(i);
			continue;
		}

		okImagesIndex.push_back(i);

		int offsetX, offsetY;
		int thisImageWidth, thisImageHeight;
		int trueOffsetX, trueOffsetY;
		offsetX = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetX;
		offsetY = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].offsetY;
		thisImageWidth = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.width;
		thisImageHeight = mISImgInfo->baseImgInfoList[baseImageIndex].ImgToBaseList[i].thisImgRotateSize.height;

		trueOffsetX = offsetX - leftUpX;
		trueOffsetY = offsetY - leftUpY;
		LoadBMP(rotateImages[baseImageIndex][i].c_str(), &img, BIT24);

		for (int y = 0; y < OutImageHeight; y++)
		{
			for (int x = 0; x < OutImageWidth; x++)
			{
				int imgx = x - trueOffsetX;
				int imgy = y - trueOffsetY;
				if (y >= trueOffsetY && x >= trueOffsetX && y < trueOffsetY + thisImageHeight && x < trueOffsetX + thisImageWidth)
				{
					if (img.data[(imgy * img.width + imgx) * 3 + 0] != 0)
					{
						OutImage.data[(y * OutImageWidth + x) * 3 + 0] = img.data[(imgy * img.width + imgx) * 3 + 0];
					}
					if (img.data[(imgy * img.width + imgx) * 3 + 1] != 0)
					{
						OutImage.data[(y * OutImageWidth + x) * 3 + 1] = img.data[(imgy * img.width + imgx) * 3 + 1];
					}
					if (img.data[(imgy * img.width + imgx) * 3 + 2] != 0)
					{
						OutImage.data[(y * OutImageWidth + x) * 3 + 2] = img.data[(imgy * img.width + imgx) * 3 + 2];
					}
				}
			}
		}
		//WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
		freeImage(&img);
	}

	for (unsigned int i = 0; i < okImagesIndex.size(); i++)
	{
		//okImages.push_back(names[okImagesIndex[i]]);
		//cout << okImages[i] << endl;
		pnOK[okImagesIndex[i]] = 1;
	}
	for (unsigned int i = 0; i < nOKImagesIndex.size(); i++)
	{
		//nOkImages.push_back(names[nOKImagesIndex[i]]);
		pnOK[nOKImagesIndex[i]] = 0;
	}


	WriteBMP(ISImgsPath[baseImageIndex].c_str(), &OutImage, BIT24);
	//outImageName = ISImgsPath[baseImageIndex];
	strcpy(outImageName, ISImgsPath[baseImageIndex].c_str());
	freeImage(&BaseImage);

	return 0;
}

int ISImgInfoInit(int baseCount, ISImgInfo* mISImgInfo)
{
    int i;
    //baseCount  个基准图
    mISImgInfo->baseCount = baseCount;
    mISImgInfo->baseImgInfoList = (BaseImgInfo*)calloc(1, sizeof(BaseImgInfo) * baseCount);
	CHECK_ALLOC_P(mISImgInfo->baseImgInfoList);
    for(i = 0; i < baseCount; i ++)
    {
        //一张baseImage对应baseCount个图
        mISImgInfo->baseImgInfoList[i].listCount = baseCount;
        mISImgInfo->baseImgInfoList[i].ImgToBaseList = (ImgRotateInfo*)calloc(1, sizeof(ImgRotateInfo) * baseCount);
		CHECK_ALLOC_P(mISImgInfo->baseImgInfoList[i].ImgToBaseList);
    }
    return 0;
}
int ISImgInfoUnit(ISImgInfo* mISImgInfo)
{
    int i;
    for(i = 0; i < mISImgInfo->baseCount; i ++)
    {
        //一张baseImage对应baseCount个图
        mISImgInfo->baseImgInfoList[i].listCount = 0;
        free(mISImgInfo->baseImgInfoList[i].ImgToBaseList);
    }

    //baseCount  个基准图
    mISImgInfo->baseCount = 0;
    free(mISImgInfo->baseImgInfoList);

    return 0;
}

int ISComputeISImgMulHInfo(vector<string>& cutImagesPaths, vector<vector<string>>& RotateImagesPaths, ISImgInfo* mISImgInfo, HMatrixInfo* mHMatrixInfo)
{
    unsigned int i, j;
    IMAGE thisImg;
    IMAGE baseImg;
    IMAGE thisRotateImg;
    float* thisImgData = NULL;
    float* thisImgRotateData = NULL;

    int srcWidth, srcHeight, dstWidth, dstHeight, offsetX, offsetY, baseWidth, baseHeight;
    for(i = 0; i < cutImagesPaths.size(); i ++)
    {
        ReadBMPHead(cutImagesPaths[i].c_str(), &baseImg);
        baseWidth = baseImg.width;
        baseHeight = baseImg.height;
        mISImgInfo->baseImgInfoList[i].baseImgSize.width = baseWidth;
        mISImgInfo->baseImgInfoList[i].baseImgSize.height = baseHeight;

        for(j = 0; j < cutImagesPaths.size(); j ++)
        {
            if(j == i)
                continue;
            LoadBMP(cutImagesPaths[j].c_str(), &thisImg, BIT24);
            srcWidth = thisImg.width;
            srcHeight = thisImg.height;

            thisImgData = (float*)calloc(1, sizeof(float) * srcWidth * srcHeight * 3);
			CHECK_ALLOC_P(thisImgData);
            ucharToFloat(thisImg.data, thisImgData, thisImg.width, thisImg.height);

            if(mHMatrixInfo->hMatrixList[i].hMatrix[j].flag == 1)
            {
                TwoImageComputeCoor(thisImgData, &thisImgRotateData, srcWidth, srcHeight, &dstWidth, &dstHeight, 
                    &offsetX, &offsetY, mHMatrixInfo->hMatrixList[i].hMatrix[j].H, baseWidth, baseHeight);
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].isHOK = 1;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].offsetX = offsetX;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].offsetY = offsetY;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].thisImgSize.width = srcWidth;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].thisImgSize.height = srcHeight;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].thisImgRotateSize.width = dstWidth;
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].thisImgRotateSize.height = dstHeight;

                thisRotateImg.channels = 3;
                thisRotateImg.width = dstWidth;
                thisRotateImg.height = dstHeight;
                thisRotateImg.data = (unsigned char*)calloc(1, sizeof(unsigned char) * dstWidth * dstHeight * 3);
				CHECK_ALLOC_P(thisRotateImg.data);
                floatToUchar(thisImgRotateData, thisRotateImg.data, dstWidth, dstHeight);
                WriteBMP(RotateImagesPaths[i][j].c_str(), &thisRotateImg, BIT24);
                freeImage(&thisRotateImg);
                free(thisImgRotateData);
            }else
            {
                mISImgInfo->baseImgInfoList[i].ImgToBaseList[j].isHOK = -1;
            }

            free(thisImgData);
            thisImgData = NULL;
            freeImage(&thisImg);
        }
    }
    return 0;
}
