#ifndef _Dump_h_
#define _Dump_h_

#ifdef __cplusplus
extern "C"
{
#endif

    int dump(char* fileName, char* buf, long long size);
    int dump_float(char* fileName, float* buf, int width, int height, int channels, int channel);
#ifdef __cplusplus
};
#endif

#endif