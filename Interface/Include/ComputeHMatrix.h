#ifndef _Compute_H_Matrix_h_
#define _Compute_H_Matrix_h_
#include "ISStruct.h"
void initHMatrixInfo(HMatrixInfo* mHMatrixInfo, int count);
void unitHMatrixInfo(HMatrixInfo* mHMatrixInfo, int count);
void initHMatrixList(HMatrixList* mHMatrixList, int count);
void unitHMatrixList(HMatrixList* mHMatrixList, int count);

//计算全部的H矩阵
int Alva_HMatrixes(std::vector<std::vector<std::string> >& matchingNames, HMatrixInfo* mHMatrixInfo);

int Alva_TwoImage_HMatrixes(const char* image1_name, const char* image2_name, int* twoImagePoint, HMatrixInfo* mHMatrixInfo, std::vector<std::string> names);

void Alva_HMixH(float* H1, float* H2, float* outH);

    
    

#endif