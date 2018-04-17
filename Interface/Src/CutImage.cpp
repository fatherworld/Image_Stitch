#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <memory.h>
#include "matrix.h"
#include "Dump.h"
using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif

    #include "BMP.h"
	#include "ISLog.h"
#ifdef __cplusplus
};
#endif
#include "CutImage.h"

enum orientationEdge
{
    LeftCut=1,
    RightCut =2,
};



//#define UpCutLen 25
//#define DownCutLen 30
//#define LeftCutLen 5
//#define RightCutLen 5
#define UpCutLen    0
#define DownCutLen  0
#define LeftCutLen  13
#define RightCutLen 13
#define MIDFILTER_R 3
#define GAUSSFILTER_R 3
#define EdgesThresholdValue 27


static int cutEdgeValue(unsigned char * inputImg, unsigned int width,unsigned int height,int& leftCutBegin,int& rightCutBegin)
{

//    float thresholdValue = 20.0;
    int i = 0;
    int k = 0;
    int j = 0;
    float LeftpreCountAveValue = 0.0;
    float LeftnxtCountAveValue = 0.0f;
    float RightpreCountAveValue = 0.0f;
    float RightnxtCountAveValue = 0.0f;
    float LeftcountAveValue = 0.0f;
    float RightcountAveValue = 0.0f;
    


    if (inputImg == NULL)
    {
        return -1;
    }

    for (i = 0 ,k = width-1; i < LeftCutLen && k > width-RightCutLen ; i++ ,k--)
    {  
        float LeftcountValue = 0.0f;
        
        float RightcountValue = 0.0f;

        for (j = 0; j < height; j++)
        { 
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 0];
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 1];
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 2];

           

            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 0];
            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 1];
            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 2];
            
        }  

        LeftcountAveValue = LeftcountValue / height /3;
        RightcountAveValue = RightcountValue / height /3 ;
        if (i == 0 && k == width - 1 )
        {
            LeftpreCountAveValue = LeftcountAveValue;
            RightpreCountAveValue = RightcountAveValue;
        }
        else
        {
            LeftnxtCountAveValue = LeftcountAveValue;
            RightnxtCountAveValue = RightcountAveValue;
            if (abs(LeftnxtCountAveValue - LeftpreCountAveValue) < EdgesThresholdValue && abs(RightnxtCountAveValue - RightpreCountAveValue) < EdgesThresholdValue)
            {
                LeftpreCountAveValue = LeftnxtCountAveValue;
                RightpreCountAveValue = RightnxtCountAveValue;
            }
            else
            {
                leftCutBegin  = i + 1;
                rightCutBegin = width - k -1;
                return 0;
            }           
        }
    }
    return 0;
}


static int cutEdgeValue1(unsigned char * inputImg, unsigned int width, unsigned int height, int& leftCutBegin, int& rightCutBegin)
{

    //    float thresholdValue = 20.0;
    int i = 0;
    int k = 0;
    int j = 0;
    float LeftpreCountAveValue = 0.0;
    float LeftnxtCountAveValue = 0.0f;
    float RightpreCountAveValue = 0.0f;
    float RightnxtCountAveValue = 0.0f;
    float LeftcountAveValue = 0.0f;
    float RightcountAveValue = 0.0f;
    bool  LeftStop = false;
    bool  RightStop = false;


    if (inputImg == NULL)
    {
        return -1;
    }

    for (i = 0, k = width - 1; i < LeftCutLen && k > width - RightCutLen; i++, k--)
    {
        float LeftcountValue = 0.0f;

        float RightcountValue = 0.0f;

        for (j = 0; j < height; j++)
        {
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 0];
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 1];
            LeftcountValue += (float)inputImg[j*width * 3 + i * 3 + 2];



            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 0];
            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 1];
            RightcountValue += (float)inputImg[j*width * 3 + k * 3 + 2];

        }

        LeftcountAveValue = LeftcountValue / height / 3;
        RightcountAveValue = RightcountValue / height / 3;
        if (i == 0  || k == width - 1)
        {
            LeftpreCountAveValue = LeftcountAveValue;
            RightpreCountAveValue = RightcountAveValue;
        }
        
        else
        {
            LeftnxtCountAveValue = LeftcountAveValue;
            RightnxtCountAveValue = RightcountAveValue;
            if (abs(LeftnxtCountAveValue - LeftpreCountAveValue) < EdgesThresholdValue)
            {
                LeftpreCountAveValue = LeftnxtCountAveValue;
            }
            else
            {
                leftCutBegin = i + 1;
                LeftStop = true;
            }
            if (abs(RightnxtCountAveValue - RightpreCountAveValue) < EdgesThresholdValue)
            {
                RightpreCountAveValue = RightnxtCountAveValue;
            }
            else
            {
                rightCutBegin = width - k - 1;
                RightStop = true;
            }
            if (RightStop && LeftStop)
                return 0;
        }
    }
    return 0;
}

//static int isBmpFile(string inImage)

static int cutImage(string inImagePath, string outImagePath, int& width, int& height)
{
    IMAGE inImage;
    LoadBMP(inImagePath.c_str(), &inImage, BIT24);
    IMAGE outImage;
    int leftCutBegin = 0;
    int rightCutBegin = 0;
    unsigned int srcy, srcx, dsty, dstx;
    unsigned int dstWidth, dstHeight;
    unsigned int srcWidth, srcHeight;

    srcWidth = inImage.width;
    srcHeight = inImage.height;


    cutEdgeValue1(inImage.data,srcWidth,srcHeight,leftCutBegin, rightCutBegin);

    dstWidth = srcWidth - leftCutBegin - rightCutBegin;
    dstHeight = srcHeight - UpCutLen - DownCutLen;

    width = dstWidth;
    height = dstHeight;

    outImage.width = dstWidth;
    outImage.height = dstHeight;
    outImage.channels = inImage.channels;
    outImage.data = (unsigned char *)calloc(1, sizeof(unsigned char) * dstWidth * dstHeight * inImage.channels);
	CHECK_ALLOC_P(outImage.data);


    

    for(srcy = UpCutLen, dsty = 0; srcy < srcHeight - DownCutLen; srcy ++, dsty ++)
    {
        for(srcx = leftCutBegin, dstx = 0; srcx < srcWidth - rightCutBegin; srcx ++, dstx ++)
        {
            outImage.data[(dsty * dstWidth + dstx) * outImage.channels + 0] = inImage.data[(srcy * srcWidth + srcx) * inImage.channels + 0];
            outImage.data[(dsty * dstWidth + dstx) * outImage.channels + 1] = inImage.data[(srcy * srcWidth + srcx) * inImage.channels + 1];
            outImage.data[(dsty * dstWidth + dstx) * outImage.channels + 2] = inImage.data[(srcy * srcWidth + srcx) * inImage.channels + 2];
        }
    }


    WriteBMP(outImagePath.c_str(), &outImage, BIT24);

    freeImage(&inImage);
    freeImage(&outImage);
    return 0;
}

static int fileIsBmp(string inImagePath)
{
   int ret = 0;
   const char* begin = inImagePath.c_str();
   const char* dot = strstr(begin,"bmp");
   if (!dot)
       return -1;

   unsigned char header[14];
   FILE* f;

   fopen_s(&f,begin, "rb");
   int numb = 0;
   
   if (fseek(f, 0, SEEK_SET))
       return -2;
   //读取bmp head信息
   numb = fread(header, 14, 1, f);
   if (numb != 1)
       return -3;

   if (header[0] != 'B' || header[1] != 'M')
       return -4;

   if (f)
       fclose(f);
   return 0;
}

int CutAllImages(vector<string>& outImagesPath, vector<string>& inImagesPath, int& width, int& height,vector<int>& allImageWidth,vector<int>& allImageHeight)
{
    int ret = 0;
    for(unsigned int i = 0; i < inImagesPath.size(); i ++)
    {
        ret = fileIsBmp(inImagesPath[i]);
        if (ret)
            return ret;
        cutImage(inImagesPath[i], outImagesPath[i], width, height);

        allImageHeight.push_back(height);
        allImageWidth.push_back(width);
    }
    return ret;
}


//中值去噪
static float getMidValue(int x, int y, float* src, int width)
{
	int i, j, n = 0;
	float v[9] = { 0 };
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			v[n] = src[(y + i) * width + x + j];
			n++;
		}
	}

	for (i = 0; i < 8; i++)
	{
		for (j = i + 1; j < 9; j++)
		{
			if (v[n] > v[j])
			{
				float t = v[j];
				v[j] = v[n];
				v[n] = t;
			}
		}
	}
	return v[4];
}
static int midValueRemoveNoise(float* src, float* dst, int width, int height)
{
	int i, j;
	memcpy_s(dst, sizeof(float) * width * height, src, sizeof(float) * width * height);
	for (i = 1; i < height - 1; i++)
	{
		for (j = 1; j < width - 1; j++)
		{
			dst[i * width + j] = getMidValue(j, i, src, width);
		}
	}
	return 0;
}

//高斯平滑
//g(x,y) = 1 / (2*pi*det*det) * e(-(x*x+y*y)/(2*det*det))
static float gaussFilter[3][3] = {
	{ 0.0625f, 0.125f, 0.0625f },
	{ 0.125f , 0.25f , 0.125f },
	{ 0.0625f, 0.125f, 0.0625f }
};
static float comGaussSmooth(float lu, float u, float ru, float l, float v, float r, float ld, float d, float rd)
{
	return (gaussFilter[0][0] * lu + gaussFilter[0][1] * u + gaussFilter[0][2] * ru +
		gaussFilter[1][0] * l + gaussFilter[1][1] * v + gaussFilter[1][2] * r +
		gaussFilter[2][0] * ld + gaussFilter[2][1] * d + gaussFilter[2][2] * rd);
}
static int gaussSmoothness(float* src, float* dst, int width, int height)
{
	int i, j;
	memcpy_s(dst, sizeof(float) * width * height, src, sizeof(float) * width * height);
	for (i = 1; i < height - 1; i++)
	{
		for (j = 1; j < width - 1; j++)
		{
			dst[i * width + j] = comGaussSmooth(src[(i - 1) * width + j - 1], src[(i - 1) * width + j], src[(i - 1) * width + j + 1],
				src[(i)* width + j - 1], src[(i)* width + j], src[(i)* width + j + 1],
				src[(i + 1) * width + j - 1], src[(i + 1) * width + j], src[(i + 1) * width + j + 1]);
		}
	}
	return 0;
}
//图像增强
static int gaussEnhance(float* src, float* dst, int width, int height)
{
	float maxValue = -100000.0f, minValue = 1000000.0f;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (src[y * width + x] > maxValue)
			{
				maxValue = src[y * width + x];
			}
			if (src[y * width + x] < minValue)
			{
				minValue = src[y * width + x];
			}
		}
	}
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			dst[y * width + x] = (src[y * width + x] - minValue) / (maxValue - minValue);
		}
	}
	return 0;
}

static float getValue(float* src, int r, int c,int rMax,int  cMax)
{
    if ((r >= rMax) || (r < 0) || (c >= cMax) || (c < 0))
        return 0;
    else
    {
        return src[r*cMax + c];
    }
}



//畸变矫正
static int calibreation(float* src, float* dst, int width, int height)
{
  //  float A[9] = { 4.9418398977142772e+002,0 ,width/2.0,0,4.941839897714e+002,height/2.0,0 ,0 ,1 };
    float A[9] = { 5.9418398977142772e+002,0 ,3.1950000000000000e+002,0,5.941839897714e+002,2.3950000000000000e+002,0 ,0 ,1 };
    float D[5] = { 6.7442695842244271e-002, 2.4180872220967919e-001, 0, 0 ,- 3.3970575589699975e-001};
    float k[9] = { 5.9418398977142772e+002,0 ,width / 2.0,0,5.941839897714e+002,height / 2.0,0 ,0 ,1 };
    float fx = A[0];
    float fy = A[4];
    float cx = A[2];
    float cy = A[5];
    float k1 = D[0];
    float k2 = D[1];
    float k3 = D[4];
    float p1 = D[2];
    float p2=  D[3];
    MARTIX pixA, converse_A;

    MARTIX cameraA;
    cameraA.cols = 1;
    cameraA.rows = 3;
    cameraA.martix = (float*)malloc(sizeof(float) * 3);
    converse_A.cols = pixA.cols = 3;
    converse_A.rows = pixA.rows = 3;
    converse_A.martix = (float*)malloc(sizeof(float) * 9);
    pixA.martix = A;
    converse_martix(pixA, &converse_A);

    MARTIX tempMatrix;
    tempMatrix.cols = 1;
    tempMatrix.rows = 3;
    tempMatrix.martix = (float*)malloc(sizeof(float) * 3);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
        
            tempMatrix.martix[0] = j;
            tempMatrix.martix[1] = i;
            tempMatrix.martix[2] = 1;
            
            mul_maritx(converse_A, tempMatrix, &cameraA);

            float r2 = cameraA.martix[0] * cameraA.martix[0] + cameraA.martix[1] * cameraA.martix[1];

            float x1 = cameraA.martix[0] * (1 + k1* r2 + k2*r2*r2) + 2 * p1*cameraA.martix[0] * cameraA.martix[1] + p2*(r2 + 2 * cameraA.martix[0] * cameraA.martix[0]);
            float y1 = cameraA.martix[1] * (1 + k1* r2 + k2*r2*r2) + p1*(r2 + 2 * cameraA.martix[1] * cameraA.martix[1]) + 2 * p2 *cameraA.martix[0] * cameraA.martix[1];
                       
            
            float u_d = fx*x1 + cx;
            float v_d = fy*y1 + cy;
            int floor_ud = int(u_d);
            int floor_vd = int(v_d);

            float du = u_d - floor_ud;
            float dv = v_d - floor_vd;
            
            float middle_src = getValue(src, floor_vd, floor_ud, height, width);
            float right_src = getValue(src, floor_vd, floor_ud + 1, height, width);
            float bottom_src = getValue(src, floor_vd + 1, floor_ud, height, width);
            float bottom_right_src = getValue(src, floor_vd + 1, floor_ud + 1, height, width);

            dst[i *width + j] = (1 - du)*(1 - dv)*middle_src + (1 - dv)*du*right_src + (1 - du)*dv*bottom_src + du*dv*bottom_right_src;       
        }
    }

    if (cameraA.martix)
    {
        free(cameraA.martix);
        cameraA.martix = NULL;
    }
    if (converse_A.martix)
    {
        free(converse_A.martix);
        converse_A.martix = NULL;
    }
    if (tempMatrix.martix)
    {
        free(tempMatrix.martix);
        tempMatrix.martix = NULL;
    }
    return 0;
}


int BmpToGrayBit32(unsigned char* imageData, float** outImage, int width, int height)
{
    float maxValue = -10000.0f, minValue = 1000.0f;
    (*outImage) = (float*)calloc(1, sizeof(float) * width * height);
	CHECK_ALLOC_P((*outImage));
    for(int y = 0; y < height; y ++)
    {
        for(int x = 0; x < width; x ++)
        {
            int R = (int)imageData[(y * width + x) * 3 + 0];
            int G = (int)imageData[(y * width + x) * 3 + 1];
            int B = (int)imageData[(y * width + x) * 3 + 2];
            (*outImage)[y * width + x] = (float)R * 0.299f + (float)G * 0.587f + (float)B * 0.114f;
        }
    }
    float* t0= (float*)calloc(1, sizeof(float) * width * height);
	float* t1 = (float*)calloc(1, sizeof(float) * width * height);
	float* t2 = (float*)calloc(1, sizeof(float) * width * height);
	CHECK_ALLOC_P(t1);
	CHECK_ALLOC_P(t2);
    
    
    //畸变矫正
//    calibreation((*outImage), t0, width, height);

	//中值滤波  去除噪声
	midValueRemoveNoise(t0, t1, width, height);
	//gauss平滑
	gaussSmoothness(t1, t2, width, height);
	//图像增强
	gaussEnhance(t2, (*outImage), width, height);


    free(t0);
	free(t1);
	free(t2);
   
    return 0;
}
