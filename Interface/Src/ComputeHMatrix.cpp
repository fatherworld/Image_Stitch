#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "ISLog.h"
using namespace std;
#ifdef __cplusplus
extern "C"
{
#endif

#include "ASiftParameter.h"
#include "featMatching.h"
#ifdef __cplusplus
};
#endif

#include "ComputeHMatrix.h"
#include "ReadDir.h"


#define MAX_MATCHED_FEATURE 300
#define MAX_Point_Num 2000
#define TM_NUM MAX_MATCHED_FEATURE

static int _consensus1[MAX_MATCHED_FEATURE] = {0};
static int _consensus2[MAX_MATCHED_FEATURE] = {0};
static int sample[4] = {0};
static int sampled[MAX_Point_Num] = {0};

static float TM_XY[TM_NUM * 2 * 1];
static float TM_H_xy[6 * 1];

static float TM_H[TM_NUM * 2 * 6];
static float TM_H_tra[TM_NUM * 6 * 2];
static float TM_H_mul[6 * 6];

static Coor TM_Coor0[TM_NUM];
static Coor TM_Coor1[TM_NUM];


//H 矩阵求逆 3*3矩阵求逆
static void MatrixInverse(float* srcMatrix, float* InverseMatrix)
{
    float MatrixDieLength = 0;
    MatrixDieLength = (srcMatrix[0] * srcMatrix[4]) - 
        (srcMatrix[1] * srcMatrix[3]);

    InverseMatrix[0] = srcMatrix[4] / MatrixDieLength;
    InverseMatrix[3] = -srcMatrix[3] / MatrixDieLength;
    InverseMatrix[6] = 0.0f;
    InverseMatrix[1] = -srcMatrix[1] / MatrixDieLength;
    InverseMatrix[4] = srcMatrix[0] / MatrixDieLength;
    InverseMatrix[7] = 0.0f;
    InverseMatrix[2] = (srcMatrix[1] * srcMatrix[5] - srcMatrix[2] * srcMatrix[4]) / MatrixDieLength;
    InverseMatrix[5] = -1.0f * (srcMatrix[0] * srcMatrix[5] - srcMatrix[2] * srcMatrix[3]) / MatrixDieLength;
    InverseMatrix[8] = 1.0f;
}
//a矩阵乘于b矩阵  
//a矩阵 m行n列   b矩阵n行l列
//生成的c矩阵为  m行l列
static void fMatmul(float* a, float* b, float* c, int m, int n, int l)
{
    int i, j, k;
    for (i = 0; i < m; i++)
        for (j = 0; j < l; j++)
        {
            c[i*l + j] = 0.0;
            for (k = 0; k < n; k++)
                c[i*l + j] += a[i*n + k] * b[k*l + j];
        }
}
//转置矩阵  m行n列   转置n行m列
static void MatrixTranspose(float* H, float* H_tra, int m, int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
        {
            H_tra[i*m + j] = H[j*n + i];
        }
}

//齐次坐标方程
static void Homogeneous(Coor match0[], Coor match1[], float coefficient[], int num)
{
    int i;

    for (i = 0; i < num; i++)
    {
        //x坐标
        coefficient[i * 6 + 0] = match0[i].x;
        coefficient[i * 6 + 1] = match0[i].y;
        coefficient[i * 6 + 2] = 1.0f;
        coefficient[i * 6 + 3] = 0.0f;
        coefficient[i * 6 + 4] = 0.0f;
        coefficient[i * 6 + 5] = 0.0f;
        //coefficient[i * 8 + 6] = -match0[i].x * match1[i].x;
        //coefficient[i * 8 + 7] = -match0[i].y * match1[i].x;
        //coefficient[i * 8 + 6] = 0.0f;
        //coefficient[i * 8 + 7] = 0.0f;

        //y坐标
        coefficient[(i + num) * 6 + 0] = 0.0f;
        coefficient[(i + num) * 6 + 1] = 0.0f;
        coefficient[(i + num) * 6 + 2] = 0.0f;
        coefficient[(i + num) * 6 + 3] = match0[i].x;
        coefficient[(i + num) * 6 + 4] = match0[i].y;
        coefficient[(i + num) * 6 + 5] = 1.0f;
        //coefficient[(i + num) * 8 + 6] = -match0[i].x * match1[i].y;
        //coefficient[(i + num) * 8 + 7] = -match0[i].y * match1[i].y;
        //coefficient[(i + num) * 8 + 6] = 0.0f;
        //coefficient[(i + num) * 8 + 7] = 0.0f;
    }
}

//随机从匹配点中抽取3个点  记得释放内存
static int* draw_ransac_sample(int n)
{
    int i, x;

    for (i = 0; i < 3; i++)//从n特征点中取出m个用于模型建立
    {
        do
        {
            x = rand() % n;//0-(n-1) 之间的整数
        } while (1 == sampled[x]);//若抽取的特征点的sampled值为1，继续选取（防止已经取到的点被重复拾取）；否则停止，将其作为样本中的一个点 

        sample[i] = x;
        sampled[x] = 1;//该点的feature_data成员的sampled域值设为1
    }
    for (i = 0; i < 3; i++) {
        sampled[sample[i]] = 0;
    }
    return sample;
}
//高斯消元法解方程组
//TM_H_mul  6   6
//TM_H_xy   6   1
//H_gauss 
//N         6
static int gaussj(float A[], float b[], float X[], const int N)
{
    int i, j, k, index;
    float atemp, btemp, t;

    for (i = 0; i < N - 1; i++)
    {
        index = i;
        for (j = i + 1; j < N; j++)
        {
            if (fabs(A[j*N + i]) > fabs(A[i*N + i]))
                index = j;
        }

        if (i != index)
        {
            for (k = i; k < N; k++)
            {
                atemp = A[index*N + k];
                A[index*N + k] = A[i*N + k];
                A[i*N + k] = atemp;
            }

            btemp = b[index];
            b[index] = b[i];
            b[i] = btemp;
        }

        if (fabs(A[i*N + i]) < 1.0e-6) {
            return 1;
        }

        for (j = i + 1; j < N; j++)
        {
            t = A[j*N + i] / A[i*N + i];
            for (k = i; k < N; k++)
            {
                A[j*N + k] = A[j*N + k] - A[i*N + k] * t;
            }

            b[j] = b[j] - b[i] * t;
        }
    }

    for (i = N - 1; i > 0; i--)
    {
        for (j = i - 1; j >= 0; j--)
        {
            b[j] = b[j] - b[i] * A[j*N + i] / A[i*N + i];
        }
    }
    for (i = 0; i < N; i++)
    {
        X[i] = b[i] / A[i*N + i];
    }

    return 0;
}
static void transformationMatrix(MatchCoorList* mMatchCoorList, int* loc, float H_gauss[], int num)
{
    int i, k;
    for(i = 0; i < num; i ++)
    {
        k = loc[i];
        TM_Coor0[i].x = mMatchCoorList->matchCoor[k].coor1.x;
        TM_Coor0[i].y = mMatchCoorList->matchCoor[k].coor1.y;
        TM_Coor1[i].x = mMatchCoorList->matchCoor[k].coor2.x;
        TM_Coor1[i].y = mMatchCoorList->matchCoor[k].coor2.y;
        //XY
        TM_XY[i] = TM_Coor1[i].x;
        TM_XY[i + num] = TM_Coor1[i].y;
    }
    //将匹配坐标转换为齐次坐标   TM_H为初始H矩阵
    //TM_H  n*2  6
    Homogeneous(TM_Coor0, TM_Coor1, TM_H, num);
    //求矩阵的转置矩阵
    //TM_H          n*2  6
    //TM_H_tra      6  n*2
    MatrixTranspose(TM_H, TM_H_tra, num * 2, 6);
    //矩阵相乘  
    //TM_H_tra  6  n*2
    //TM_H      n*2 6
    //TM_H_mul  6  6
    fMatmul(TM_H_tra, TM_H, TM_H_mul, 6, num * 2, 6);
    //TM_H_tra  6   n*2
    //TM_XY     n*2   1
    //TM_H_xy   6   1
    fMatmul(TM_H_tra, TM_XY, TM_H_xy, 6, num * 2, 1);

    //TM_H_mul  6   6
    //TM_H_xy   6   1
    //H_gauss   
    gaussj(TM_H_mul, TM_H_xy, H_gauss, 6);

    H_gauss[6] = 0.0f;
    H_gauss[7] = 0.0f;
    H_gauss[8] = 1.0f;
}


static int find_consensus(int* _consensus, MatchCoorList* mMatchCoorList, float H_transform[9], int num)
{

    float err;
    float err_tol = /*2.f*/3.0f;
    int i, in = 0;
    float x0, y0, x1, y1, x2, y2;

    for (i = 0; i < num; i++)
    {

        x0 = mMatchCoorList->matchCoor[i].coor1.x;
        y0 = mMatchCoorList->matchCoor[i].coor1.y;

        x1 = mMatchCoorList->matchCoor[i].coor2.x;
        y1 = mMatchCoorList->matchCoor[i].coor2.y;

        x2 = (H_transform[0] * x0 + H_transform[1] * y0 + H_transform[2]);
        y2 = (H_transform[3] * x0 + H_transform[4] * y0 + H_transform[5]);

        err = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
        //printf("%f  \n", err);
        if (sqrt(err) <= err_tol)//若变换误差小于容错度，将其加入一致集
            _consensus[in++] = i;
    }

    return 	in;
}

static int alva_ComputeHMatrix(MatchCoorList* mMatchCoorList, float* H)
{
    int m = 0;
    int* sample = NULL;
    int in = 0;
    int in_max = 0;
    int* _consensus = _consensus1;
    int* best_consensus = NULL;
	//deleteRepeatedCoor(mMatchCoorList);
    if(mMatchCoorList->count < 7/*20*//*50*//*27*/)
    {
        H[0] = 1.0f;
        H[1] = 0.0f;
        H[2] = 0.0f;
        H[3] = 0.0f;
        H[4] = 1.0f;
        H[5] = 0.0f;
        H[6] = 0.0f;
        H[7] = 0.0f;
        H[8] = 1.0f;
        return -1;
    }else if(mMatchCoorList->count > MAX_MATCHED_FEATURE)
    {
        mMatchCoorList->count = MAX_MATCHED_FEATURE;
    }

    srand((unsigned int)time(NULL));
    for(m = 0; m < 500; m ++)
    {
        //初始化3个内点
        sample = draw_ransac_sample(mMatchCoorList->count);

        //计算H
        transformationMatrix(mMatchCoorList, sample, H, 3);
        in = find_consensus(_consensus, mMatchCoorList, H, mMatchCoorList->count);
        if(in > in_max)
        {
            best_consensus = _consensus;
            in_max = in;
            if(_consensus == _consensus1)
            {
                _consensus = _consensus2;
            }else
            {
                _consensus = _consensus1;
            }
        }
    }
    if(in_max >= 6)
    {
        transformationMatrix(mMatchCoorList, best_consensus, H, in_max);
    }
    return 0;
}

static int alva_TwoImage_ComputeHMatrix(MatchCoorList* mMatchCoorList, float* H)
{
    int* sample = NULL;
    int in = 0;
    int in_max = 0;
    int* _consensus = _consensus1;
    int* best_consensus = NULL;

    srand((unsigned int)time(NULL));

    //初始化3个内点
    sample = draw_ransac_sample(mMatchCoorList->count);

    //计算H
    transformationMatrix(mMatchCoorList, sample, H, 3);
    in = find_consensus(_consensus, mMatchCoorList, H, mMatchCoorList->count);
    if(in > in_max)
    {
        best_consensus = _consensus;
        in_max = in;
        if(_consensus == _consensus1)
        {
            _consensus = _consensus2;
        }else
        {
            _consensus = _consensus1;
        }
    }
    if(in_max >= 3)
    {
        transformationMatrix(mMatchCoorList, best_consensus, H, in_max);
	}
	else
	{
		return -1;
	}
    return 0;
}

//检验H矩阵是否合格
//1  合格
//-1  不合格
static int isHQualified(float* H)
{
    float A[4] = {0.0f};
    float A_tra[4] = {0.0f};
    float A_mul[4] = {0.0f};
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    float lamd1 = 0.0f, lamd2 = 0.0f;


    //return 1;

    A[0] = H[0];
    A[1] = H[1];
    A[2] = H[3];
    A[3] = H[4];
    //A矩阵转置矩阵A_tra
    MatrixTranspose(A, A_tra, 2, 2);
    //A_tra 矩阵乘A矩阵
    fMatmul(A, A_tra, A_mul, 2, 2, 2);

    a = 1.0f;
    b = -(A_mul[0] + A_mul[3]);
    c = A_mul[0] * A_mul[3] - A_mul[1] * A_mul[2];

    if((b * b - 4 * a * c < 0.0f))
    {
        return -1;
    }
    lamd1 = (-b + sqrtf((b * b - 4 * a * c))) / (2.0f * a);
    lamd2 = (-b - sqrtf((b * b - 4 * a * c))) / (2.0f * a);

    if((abs(lamd1) / abs(lamd2)) > 5.0f)
    {
        return -1;
    }else
    {
        return 1;
    }
    return -1;
}



void initHMatrixInfo(HMatrixInfo* mHMatrixInfo, int count)
{
    int i = 0;
    int j = 0;
    mHMatrixInfo->count = count;
    mHMatrixInfo->hMatrixList = (HMatrixList*)calloc(1, sizeof(HMatrixList) * (count));
	CHECK_IS_VOID(mHMatrixInfo->hMatrixList);
    for(i = 0; i < count; i ++)
    {
        mHMatrixInfo->hMatrixList[i].pairCount = 0;
        mHMatrixInfo->hMatrixList[i].hMatrix = (HMatrix*)calloc(1, sizeof(HMatrix) * (count));
		CHECK_IS_VOID(mHMatrixInfo->hMatrixList[i].hMatrix);
        for(j = 0; j < count; j ++)
        {
            mHMatrixInfo->hMatrixList[i].hMatrix[j].H = (float*)calloc(1, sizeof(float) * 3 * 3);
			CHECK_IS_VOID(mHMatrixInfo->hMatrixList[i].hMatrix[j].H);
        }
    }
}


void unitHMatrixInfo(HMatrixInfo* mHMatrixInfo, int count)
{
    int i = 0;
    int j = 0;
    for(i = 0; i < count; i ++)
    {
        for(j = 0; j < count; j ++)
        {
            free(mHMatrixInfo->hMatrixList[i].hMatrix[j].H);
        }
        mHMatrixInfo->hMatrixList[i].pairCount = 0;
        free(mHMatrixInfo->hMatrixList[i].hMatrix);
    }
    mHMatrixInfo->count = 0;
    free(mHMatrixInfo->hMatrixList);
}

void initHMatrixList(HMatrixList* mHMatrixList, int count)
{
    int i = 0;
    mHMatrixList->pairCount = 0;
    mHMatrixList->hMatrix = (HMatrix*)calloc(1, sizeof(HMatrix) * (count));
	CHECK_IS_VOID(mHMatrixList->hMatrix);
    for(i = 0; i < count; i ++)
    {
        mHMatrixList->hMatrix[i].H = (float*)calloc(1, sizeof(float) * 3 * 3);
		CHECK_IS_VOID(mHMatrixList->hMatrix[i].H);
    }
}
void unitHMatrixList(HMatrixList* mHMatrixList, int count)
{
    int i = 0;
    
    for(i = 0; i < count; i ++)
    {
        free(mHMatrixList->hMatrix[i].H);
    }
    free(mHMatrixList->hMatrix);
}

int Alva_HMatrixes(vector<vector<string>>& matchingNames, HMatrixInfo* mHMatrixInfo)
{
    unsigned int i = 0;
    unsigned int j = 0;
    //特征点对
    MatchCoorList mMatchCoorList;
    ISInitMatchCoorList(&mMatchCoorList);
    
    for(i = 0; i < matchingNames.size(); i++)
    {
        for(j = 0; j < matchingNames[i].size(); j ++)
        {
            int ret = 0;
            //读取特征点对
            fscanfMatching((char*)(matchingNames[i][j].c_str()), &mMatchCoorList);

            //计算H矩阵
            ret = alva_ComputeHMatrix(&mMatchCoorList, mHMatrixInfo->hMatrixList[i].hMatrix[j + i + 1].H);
            if(ret == -1)
            {
                mHMatrixInfo->hMatrixList[i].hMatrix[j + i + 1].flag = ret;
                mHMatrixInfo->hMatrixList[j + i + 1].hMatrix[i].flag = ret;
                continue;
            }
            //检测H矩阵是否合格  1合格  -1不合格
            ret = isHQualified(mHMatrixInfo->hMatrixList[i].hMatrix[j + i + 1].H);

            mHMatrixInfo->hMatrixList[i].hMatrix[j + i + 1].flag = ret;
            if(ret == 1)
                mHMatrixInfo->hMatrixList[i].pairCount ++;
            MatrixInverse(mHMatrixInfo->hMatrixList[i].hMatrix[j + i + 1].H, mHMatrixInfo->hMatrixList[j + i + 1].hMatrix[i].H);
            mHMatrixInfo->hMatrixList[j + i + 1].hMatrix[i].flag = ret;
            if(ret == 1)
                mHMatrixInfo->hMatrixList[j + i + 1].pairCount ++;

            ISClearMatchCoorList(&mMatchCoorList);
        }
    }    
    ISUnitMatchCoorList(&mMatchCoorList);
    return 0;
}
//image1_name  基准图
//image2_name  匹配图
//twoImagePoint 两张图的手动点对
//H矩阵信息
//names 文件名
int Alva_TwoImage_HMatrixes(const char* image1_name, const char* image2_name, int* twoImagePoint, HMatrixInfo* mHMatrixInfo, vector<string> names)
{
    int ret = 0;
    int i = 0;
    int image1_index = -1;
    int image2_index = -1;
    MatchCoorList mMatchCoorList;
    ISInitMatchCoorList(&mMatchCoorList);
    mMatchCoorList.count = 4;
    for(i = 0; i < 4; i ++)
    {
        mMatchCoorList.matchCoor[i].coor1.x = (float)twoImagePoint[i * 4 + 0];
        mMatchCoorList.matchCoor[i].coor1.y = (float)twoImagePoint[i * 4 + 1];
        mMatchCoorList.matchCoor[i].coor2.x = (float)twoImagePoint[i * 4 + 2];
        mMatchCoorList.matchCoor[i].coor2.y = (float)twoImagePoint[i * 4 + 3];
        //mMatchCoorList.matchCoor[i].coor1.x = (float)twoImagePoint[i * 4 + 2];
        //mMatchCoorList.matchCoor[i].coor1.y = (float)twoImagePoint[i * 4 + 3];
        //mMatchCoorList.matchCoor[i].coor2.x = (float)twoImagePoint[i * 4 + 0];
        //mMatchCoorList.matchCoor[i].coor2.y = (float)twoImagePoint[i * 4 + 1];
    }
    
    for(i = 0; i < (int)(names.size()); i ++)
    {
        if(names[i] == image1_name)
        {
            image1_index = i;
        }
        if(names[i] == image2_name)
        {
            image2_index = i;
        }
    }
    if (image1_index == -1 || image2_index == -1)
        return 0;
    ret = alva_TwoImage_ComputeHMatrix(&mMatchCoorList, mHMatrixInfo->hMatrixList[image1_index].hMatrix[image2_index].H);
    if(ret == -1)
    {
        mHMatrixInfo->hMatrixList[image1_index].hMatrix[image2_index].flag = ret;
        mHMatrixInfo->hMatrixList[image2_index].hMatrix[image1_index].flag = ret;
        return -1;
    }
    //检测H矩阵是否合格  1合格  -1不合格
    ret = isHQualified(mHMatrixInfo->hMatrixList[image1_index].hMatrix[image2_index].H);

    mHMatrixInfo->hMatrixList[image1_index].hMatrix[image2_index].flag = ret;
    if(ret == 1)
        mHMatrixInfo->hMatrixList[image1_index].pairCount ++;
    MatrixInverse(mHMatrixInfo->hMatrixList[image1_index].hMatrix[image2_index].H, mHMatrixInfo->hMatrixList[image2_index].hMatrix[image1_index].H);
    mHMatrixInfo->hMatrixList[image2_index].hMatrix[image1_index].flag = ret;
    if(ret == 1)
        mHMatrixInfo->hMatrixList[image2_index].pairCount ++;
    
    ISClearMatchCoorList(&mMatchCoorList);

    return 0;
}
void Alva_HMixH(float* H1, float* H2, float* outH)
{
    outH[0] = H1[0] * H2[0] + H1[1] * H2[3];
    outH[1] = H1[0] * H2[1] + H1[1] * H2[4];
    outH[2] = H1[0] * H2[2] + H1[1] * H2[5] + H1[2];

    outH[3] = H1[3] * H2[0] + H1[4] * H2[3];
    outH[4] = H1[3] * H2[1] + H1[4] * H2[4];
    outH[5] = H1[3] * H2[2] + H1[4] * H2[5] + H1[5];

    outH[6] = 0.0f;
    outH[7] = 0.0f;
    outH[8] = 1.0f;
}