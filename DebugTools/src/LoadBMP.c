#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "BMP.h"

#define BMP_SIZE_FILEHEADER 14
#define BMP_SIZE_INFOHEADER 40

#define BMP_COLOR_BITS_24 24
#define BMP_COLOR_BITS_32 32


static unsigned int uInt16Number(unsigned char buf[2]){
    return (buf[1] << 8) | buf[0];
}

static unsigned int uInt32Number(unsigned char buf[4]){
    unsigned numb = buf[3];
    numb = (numb << 8) | buf[2];
    numb = (numb << 8) | buf[1];
    numb = (numb << 8) | buf[0];
    return numb;
}

static void IntTo2Bytes(int val, unsigned char buf[2]){
    buf[0] = (unsigned char)val;
    buf[1] = (unsigned char)(val >> 8);
}

static void IntTo4Bytes(int val, unsigned char buf[4]){
    buf[0] = (unsigned char)val;
    buf[1] = (unsigned char)(val >> 8);
    buf[2] = (unsigned char)(val >> 16);
    buf[3] = (unsigned char)(val >> 24);
}


static int ReadFileHeader(FILE* f, int* bitmap_pos){

    unsigned char header[BMP_SIZE_FILEHEADER];
    BMPHEAD bmp_head;

    int numb = 0;
    int offset = 0;

    if (fseek(f, 0, SEEK_SET))
        return -1;
    //读取bmp head信息
    numb = fread(header, BMP_SIZE_FILEHEADER, 1, f);
    if (numb != 1)
        return -2;
    // 0 - 1
    if (header[0] != 'B' || header[1] != 'M')
        return -3;
    bmp_head.type[0] = header[0];
    bmp_head.type[1] = header[1];

    // 2 - 5
    bmp_head.size = uInt32Number(header + 2);
    // 6 - 7
    // 8 - 9

    // 10 - 13
    offset = uInt32Number(header + 10);
    if(offset != 54)
        return -4;    

    *bitmap_pos  = offset;
    return 0;
}

static int ReadFileInfoHead(FILE* f, IMAGE * img){

    unsigned char header[BMP_SIZE_INFOHEADER];
    BMPINFOHEAD bmp_info_head;
    
    int numb = 0;

    if (fseek(f, BMP_SIZE_FILEHEADER, SEEK_SET))
        return -1;
    //读取bmp info head信息
    numb = fread(header, BMP_SIZE_INFOHEADER, 1, f);
    if (numb != 1)
        return -2;
    
    // 14 - 17
    bmp_info_head.selfSize = uInt32Number(header);
    if(bmp_info_head.selfSize != 40)
        return -3;

    // 18 - 21
    bmp_info_head.bitWidth = (long)uInt32Number(header + 4);
    // 22 - 25
    bmp_info_head.bitHeight = (long)uInt32Number(header + 8);
    // 26 - 27
    bmp_info_head.bitPlanes = (unsigned short)uInt16Number(header + 12);
    // 28 - 29
    bmp_info_head.pixelBitCount = (unsigned short)uInt16Number(header + 14);
    // 30 - 33
    bmp_info_head.compression = uInt32Number(header + 16);
    // 34 - 37
    bmp_info_head.sizeImage = uInt32Number(header + 20);
    // 38 - 41
    bmp_info_head.pixelXPerMeter = (long)uInt32Number(header + 24);
    // 42 - 45
    bmp_info_head.pixelYPerMeter = (long)uInt32Number(header + 28);
    // 46 - 49
    bmp_info_head.colorUsed = uInt32Number(header + 32);
    // 50 - 53
    bmp_info_head.colorImportant = uInt32Number(header + 36);

    img->width = (int)abs(bmp_info_head.bitWidth);
    img->height = (int)abs(bmp_info_head.bitHeight);

    return (int)(bmp_info_head.pixelBitCount);
}
int ReadBMPHead(const char * file, IMAGE* out_img)
{
    FILE* f;
    int bitmap_pos;
    int n_bits;

    int flErr = 0;

    fopen_s(&f, file, "rb");

    if (!f)
        return -11;

    if (out_img == NULL)
        return -12;

    if (0 > (flErr = ReadFileHeader(f, &bitmap_pos))){
        fclose(f);
        return flErr;
    }

    n_bits = ReadFileInfoHead(f, out_img);
    fclose(f);
    return 0;
}
int LoadBMP(const char* file, IMAGE* out_img, BITDATATYPE bit_data_type){

    FILE* f;
    int bitmap_pos;
    int n_bits;

    int flErr = 0;

    fopen_s(&f, file, "rb");

    if (!f)
        return -11;

    if (out_img == NULL)
        return -12;

    if (0 > (flErr = ReadFileHeader(f, &bitmap_pos))){
        fclose(f);
        return flErr;
    }

    n_bits = ReadFileInfoHead(f, out_img);

    if(n_bits == BMP_COLOR_BITS_24){
        int rgb_size;
        int rgba_size;
        unsigned char* rgb=NULL;
        int y;
        unsigned char* line=NULL;
        int rest_4;

        if (fseek(f, bitmap_pos, SEEK_SET)){
            fclose(f);
            return -16;
        }

        rgb_size = 3 * out_img->width;
        rgba_size = 4 * out_img->width;

        rest_4 = rgb_size % 4;
        if (rest_4 > 0)
            rgb_size += 4 - rest_4;

        if(bit_data_type == 1)
        {
            out_img->channels = 4;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 4);

            if (out_img->data == NULL)
                return -17;

            rgb = (unsigned char*)malloc(rgb_size);

            if(NULL == rgb)
                return -18;

            for(y = out_img->height - 1; y >= 0; y --){
                int numb = 0;
                int x = 0;

                numb = fread(rgb, rgb_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgb);
                    return -19;
                }

                numb = 0;
                line = out_img->data + out_img->width * 4 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[3] = 255;
                    line[2] = rgb[numb++];
                    line[1] = rgb[numb++];
                    line[0]= rgb[numb++];
                    line += 4;
                }
            }
        }else if(bit_data_type == 2)
        {
            out_img->channels = 3;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 3);

            if (out_img->data == NULL)
                return -17;

            rgb = (unsigned char*)malloc(rgb_size);

            if(NULL == rgb)
                return -18;

            for(y = out_img->height - 1; y >= 0; y --){
                int numb = 0;
                int x = 0;

                numb = fread(rgb, rgb_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgb);
                    return -19;
                }

                numb = 0;
                line = out_img->data + out_img->width * 3 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgb[numb++];
                    line[1] = rgb[numb++];
                    line[0]= rgb[numb++];
                    line += 3;
                }
            }
        }
        fclose(f);
        free(rgb);
    }else if(n_bits == BMP_COLOR_BITS_32)
    {
        int rgba_size;
        unsigned char* rgba=NULL;
        int y;
        unsigned char* line;

        if (fseek(f, bitmap_pos, SEEK_SET)){
            fclose(f);
            return -16;
        }

        rgba_size = 4 * out_img->width;
        if(bit_data_type == 1)
        {
            out_img->channels = 4;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 4);

            if (out_img->data == NULL)
                return -17;

            rgba = (unsigned char*)malloc(rgba_size);

            if(NULL == rgba)
                return -18;

            for (y = out_img->height - 1; y >= 0; y --){
                int numb = 0;
                int x = 0;

                numb = fread(rgba, rgba_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgba);
                    return -19;
                }

                numb = 0;
                line = out_img->data + out_img->width * 4 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgba[numb++];//B
                    line[1] = rgba[numb++];//G
                    line[0] = rgba[numb++];//R
                    line[3]= rgba[numb++];//A
                    line += 4;
                }
            }
        }else if(bit_data_type == 2)
        {
            out_img->channels = 3;
            out_img->data = (unsigned char*)malloc(out_img->width * out_img->height * 3);

            if (out_img->data == NULL)
                return -17;

            rgba = (unsigned char*)malloc(rgba_size);

            if(NULL == rgba)
                return -18;

            for (y = out_img->height - 1; y >= 0; y --){
                int numb = 0;
                int x = 0;

                numb = fread(rgba, rgba_size, 1, f);
                if (numb != 1){
                    fclose(f);
                    free(rgba);
                    return -19;
                }

                numb = 0;
                line = out_img->data + out_img->width * 3 * y;
                for (x = 0; x < out_img->width ; x++){
                    line[2] = rgba[numb++];//B
                    line[1] = rgba[numb++];//G
                    line[0] = rgba[numb++];//R
                    line += 3;
                    numb ++;
                }
            }
        }
        fclose(f);
        free(rgba);
    }
    else{
        return -20;
    }
    return 0;
}

static int WriteFileHeader(FILE* f, IMAGE * in_img, BITDATATYPE bit_data_type)
{
    unsigned char header[BMP_SIZE_FILEHEADER];
    int imgWidth, imgHeight;
    int imageSize;

    int numb = 0;
    int offset = 0;

    if (fseek(f, 0, SEEK_SET))
        return -1;

    header[0] = 'B';
    header[1] = 'M';

    imgWidth = in_img->width;
    imgHeight = in_img->height;

    if(bit_data_type == 1)
    {
        imageSize = imgWidth * 4 * imgHeight + BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER;
    }else if(bit_data_type == 2)
    {
        int rgbSize;
        int rest_4;
        rgbSize = imgWidth * 3;
        rest_4 = rgbSize % 4;
        if (rest_4 > 0)
            rgbSize += 4 - rest_4;
        imageSize = rgbSize * imgHeight + BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER;
    }
    IntTo4Bytes(imageSize, header + 2);
    IntTo4Bytes(0, header + 6);
    IntTo4Bytes((BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), header + 10);

    //读取bmp head信息
    numb = fwrite(header, BMP_SIZE_FILEHEADER, 1, f);
    if (numb != 1)
        return -2;

    return 0;
}

static int WriteFileInfoHead(FILE* f, IMAGE * in_img, BITDATATYPE bit_data_type){

    unsigned char header[BMP_SIZE_INFOHEADER];
    int imageSize = 0;
    int numb = 0;

    if (fseek(f, BMP_SIZE_FILEHEADER, SEEK_SET))
        return -1;
    // 14- 17
    IntTo4Bytes(BMP_SIZE_INFOHEADER, header);
    // 18 - 21 
    IntTo4Bytes(in_img->width, header + 4);
    // 22 - 25
    IntTo4Bytes(in_img->height, header + 8);
    // 26 - 27
    IntTo2Bytes(1, header + 12);
    // 28 - 29
    if(bit_data_type == 1)
    {
        IntTo2Bytes(32, header + 14);
    }else if(bit_data_type == 2)
    {
        IntTo2Bytes(24, header + 14);
    }
    // 30 - 33
    IntTo4Bytes(0, header + 16);
    // 34 - 37
     if(bit_data_type == 1)
    {
        imageSize = in_img->width * 4 * in_img->height;
    }else if(bit_data_type == 2)
    {
        int rgbSize;
        int rest_4;
        rgbSize = in_img->width * 3;
        rest_4 = rgbSize % 4;
        if (rest_4 > 0)
            rgbSize += 4 - rest_4;
        imageSize = rgbSize * in_img->height;
    }
    IntTo4Bytes(imageSize, header + 20);
    // 38 - 41
    IntTo4Bytes(0, header + 24);
    // 42 - 45
    IntTo4Bytes(0, header + 28);
    // 46 - 49
    IntTo4Bytes(0, header + 32);
    // 50 - 53
    IntTo4Bytes(0, header + 36);



    numb = fwrite(header, BMP_SIZE_INFOHEADER, 1, f);
    if (numb != 1)
        return -2;

    return 0;
}
int WriteBMP(const char * file, IMAGE * in_img, BITDATATYPE bit_data_type){
    FILE* f;

    int flErr = 0;

    fopen_s(&f, file, "wb");

    if (!f)
        return -1;

    if (in_img == NULL)
        return -2;

    if (0 != WriteFileHeader(f, in_img, bit_data_type)){
        fclose(f);
        return -3;
    }

    if(0 != WriteFileInfoHead(f, in_img, bit_data_type)){
        fclose(f);
        return -4;
    }


    if(bit_data_type == 2){
        int rgb_size;
        unsigned char * rgb;
        int y;
        unsigned char* line;
        int rest_4;

        rgb_size = 3 * in_img->width;

        rest_4 = rgb_size % 4;
        if (rest_4 > 0)
            rgb_size += 4 - rest_4;


        rgb = (unsigned char *)malloc(rgb_size * in_img->height);

        if (rgb == NULL)
            return -6;

        if(in_img->channels == 4)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 4 * y;
                line = rgb + rgb_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line += 3;
                    numb ++;
                }
            }
        }else if(in_img->channels == 3)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 3 * y;
                line = rgb + rgb_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line += 3;
                }
            }
        }
        if (fseek(f, (BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), SEEK_SET)){
            fclose(f);
            return -5;
        }
        fwrite(rgb, rgb_size * in_img->height, 1, f); 
        fclose(f);
        free(rgb);
    }else if(bit_data_type == 1)
    {
        int rgba_size;
        unsigned char * rgba;
        int y;
        unsigned char* line;
        rgba_size = 4 * in_img->width;


        rgba = (unsigned char *)malloc(rgba_size * in_img->height);

        if (rgba == NULL)
            return -6;

        if(in_img->channels == 4)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 4 * y;
                line = rgba + rgba_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];//R
                    line[1] = in_img->data[numb++];//G
                    line[0]= in_img->data[numb++];//B
                    line[3]= in_img->data[numb++];//A
                    line += 4;
                }
            }
        }else if(in_img->channels == 3)
        {
            for (y = 0; y < in_img->height; y ++){
                int numb = 0;
                int x = 0;

                numb = in_img->width * 3 * y;
                line = rgba + rgba_size * (in_img->height - y - 1);
                for (x = 0; x < in_img->width; x++){
                    line[2] = in_img->data[numb++];
                    line[1] = in_img->data[numb++];
                    line[0]= in_img->data[numb++];
                    line[3]= 255;
                    line += 4;
                }
            }
        }
        if (fseek(f, (BMP_SIZE_FILEHEADER + BMP_SIZE_INFOHEADER), SEEK_SET)){
            fclose(f);
            return -5;
        }
        fwrite(rgba, rgba_size * in_img->height, 1, f); 
        fclose(f);
        free(rgba);
    }
    else{
        return -20;
    }
    return 0;
}
int freeImage(IMAGE * img){
    if(img == NULL)
    {
        return -1;
    }
    if(img->data != NULL)
    {
        free(img->data);
        img->data = NULL;
        img->width = 0;
        img->height = 0;
    }
    return 0;
}

