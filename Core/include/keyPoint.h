#ifndef _key_point_h_
#define _key_point_h_

#define Alva_OriSize 8
#define Alva_IndexSize 4
#define Alva_VecLength (Alva_IndexSize * Alva_IndexSize * Alva_OriSize)     //4 * 4 * 8 = 128
#define Alva_Max_KeyPoint 5000

typedef struct _key_point_float_
{
    float x;
    float y;
    float scale;
    float angle;
    float vec[Alva_VecLength];
}keyPoint;

typedef struct _key_Point_info_
{
    int count;
    keyPoint* mkeyPoint;
}keyPointInfo;


#ifdef __cplusplus
extern "C"
{
#endif

    int initKeyPoints(keyPointInfo* mKeyPointInfo);
    int unitKeyPoints(keyPointInfo* mKeyPointInfo);
    int clearKeyPoints(keyPointInfo* mKeyPointInfo);

#ifdef __cplusplus
};
#endif


#endif