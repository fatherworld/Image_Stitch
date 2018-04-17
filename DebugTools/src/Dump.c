#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

static char* dumpRoot = ".//testImage//";

static char file2open[1024];
int dump(char* fileName, char* buf, long long size)
{
    FILE* file = NULL;
    sprintf_s(file2open, 1024, "%s%s", dumpRoot, fileName);
    fopen_s(&file, file2open, "wb");
    if(NULL == file)
    {
        return -1;
    }
    fwrite(buf, 1, (size_t)size, file);
    fclose(file);
    return 0;
}
int dump_float(char* fileName, float* buf, int width, int height, int channels, int channel)
{
    FILE* file;
    int w, h;
    sprintf_s(file2open, 1024, "%s[%d, %d]%s", dumpRoot, channels, channel, fileName);
    fopen_s(&file, file2open, "w+");

    for(h = 0; h < height; h ++)
    {
        for(w = 0; w < width - 1; w ++)
        {
            fprintf(file, "%f,", buf[(h * width + w) * channels + channel]);
        }
        fprintf(file, "%f\n", buf[(h * width + w) * channels + channel]);
    }

    fclose(file);
    return 0;

}