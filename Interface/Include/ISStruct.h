#ifndef _IS_Struct_h_
#define _IS_Struct_h_

typedef struct _Coor_
{
    float x;
    float y;
}Coor;

typedef struct _Img_Size_
{
    int width;
    int height;
}ImgSize;


typedef struct _Match_Coor_
{
    Coor coor1;
    Coor coor2;
}MatchCoor;

typedef struct _Match_Coor_List_
{
    int count;
    MatchCoor* matchCoor;
}MatchCoorList;




typedef struct _H_Matrix_
{
    int flag;
    float* H;
}HMatrix;

typedef struct _H_Matrix_List_
{
    int pairCount;
    HMatrix* hMatrix;
}HMatrixList;

typedef struct _H_Matrix_Info_
{
    int count;
    HMatrixList* hMatrixList;
}HMatrixInfo;



#endif