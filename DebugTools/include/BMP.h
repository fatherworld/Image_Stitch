#ifndef _BMP_H_
#define _BMP_H_


#ifdef __cplusplus
extern "C" {
#endif

    typedef enum _BIT_DATA_TYPE_{
        BIT32 = 1,                      //读取或存储成32位
        BIT24 = 2,                      //读取或存储成24位
    }BITDATATYPE;

    typedef struct _BMPFILEHEAD_{
        unsigned char   type[2];        //存储 'B' 'M'                    2字节
        unsigned int    size;           //位图文件大小                    4字节
        unsigned short  reserved1;      //保留字                          2字节
        unsigned short  reserved2;      //保留字                          2字节
        unsigned int    offBits;        //位图数据起始位置                4字节
    }BMPHEAD;
    typedef struct _BMPFILEINFOHEAD_{
        unsigned int    selfSize;       //位图信息头的大小                 4字节
        long            bitWidth;       //位图的宽度,以像素为单位          4字节
        long            bitHeight;      //位图的高度,以像素为单位          4字节
        unsigned short  bitPlanes;      //目标设备的级别,必须为1           2字节
        unsigned short  pixelBitCount;  //每个像素所需的位数               2字节
        unsigned int    compression;    //位图压缩类型,0(不压缩)           4字节
        unsigned int    sizeImage;      //位图的大小,以字节为单位          4字节
        long            pixelXPerMeter; //位图的水平分辨率,每米像素数      4字节
        long            pixelYPerMeter; //位图的垂直分辨率,每米像素数      4字节
        unsigned int    colorUsed;      //位图实际使用的颜色表中的颜色数   4字节
        unsigned int    colorImportant; //位图显示过程中重要的颜色数       4字节
    }BMPINFOHEAD;
    typedef struct _IMAGE_{
        int width;
        int height;
        int channels;
        unsigned char * data;
    }IMAGE;

int LoadBMP(const char * file, IMAGE* out_img, BITDATATYPE bit_data_type);
int WriteBMP(const char * file, IMAGE * in_img, BITDATATYPE bit_data_type);

int ReadBMPHead(const char * file, IMAGE* out_img);

int freeImage(IMAGE * img);
#ifdef __cplusplus
}
#endif

#endif
