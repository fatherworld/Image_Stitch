#ifndef _C_Buffer_h_
#define _C_Buffer_h_

#include "ASiftParameter.h"

typedef enum _C_Buffer_List_
{
    Buffer_start                                                    = -1,
    Buffer_gray_src                                                     ,
    Buffer_integral_image                                               ,
    Buffer_pyramid_image                                                ,
    
    Buffer_Number                   = Buffer_pyramid_image + OCTAVES * 2 + 2,
}CBufferList;

#ifdef __cplusplus
extern "C"
{
#endif

    float** getBufferList();

    int initCBufferList();

    int unitCBufferList();

#ifdef __cplusplus
};
#endif

#endif
