#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include "ASiftParameter.h"
#include "CBuffer.h"
#include "keyPoint.h"
#include "Surf.h"
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
const float gauss25[7][7] = {
    0.02546481f,	0.02350698f,	0.01849125f,	0.01239505f,	0.00708017f,	0.00344629f,	0.00142946f,
    0.02350698f,	0.02169968f,	0.01706957f,	0.01144208f,	0.00653582f,	0.00318132f,	0.00131956f,
    0.01849125f,	0.01706957f,	0.01342740f,	0.00900066f,	0.00514126f,	0.00250252f,	0.00103800f,
    0.01239505f,	0.01144208f,	0.00900066f,	0.00603332f,	0.00344629f,	0.00167749f,	0.00069579f,
    0.00708017f,	0.00653582f,	0.00514126f,	0.00344629f,	0.00196855f,	0.00095820f,	0.00039744f,
    0.00344629f,	0.00318132f,	0.00250252f,	0.00167749f,	0.00095820f,	0.00046640f,	0.00019346f,
    0.00142946f,	0.00131956f,	0.00103800f,	0.00069579f,	0.00039744f,	0.00019346f,	0.00008024f
};
#define PI  3.1415926f

static int fRound(float value)
{
    return ((int)floor(value + 0.5f));
}

//求图像的积分图
static int ISIntegral(float* srcImage, float* integralImage, int width, int height)
{
    float rs = 0.0f;
    int x, y;

    for(x = 0; x < width; x ++)
    {
        rs += srcImage[x];
        integralImage[x] = rs;
    }

    for(y = 1; y < height; y ++)
    {
        rs = 0.0f;
        for(x = 0; x < width; x ++)
        {
            rs += srcImage[y * width + x];
            integralImage[y * width + x] = rs + integralImage[(y - 1) * width + x];
        }
    }
    return 0;
}

static float boxIntegral(float* data, int row, int col, int rows, int cols, int width, int height)
{

    // The subtraction by one for row/col is because row/col is inclusive.
    int r1 = row - 1;
    int c1 = col - 1;
    int r2 = row + rows - 1;
    int c2 = col + cols - 1;

    float A = 0.0f, B = 0.0f, C = 0.0f, D = 0.0f;
    if (r1 >= 0 && c1 >= 0 && r1 < height && c1 < width) A = data[r1 * width + c1];
    if (r1 >= 0 && c2 >= 0 && r1 < height && c2 < width) B = data[r1 * width + c2];
    if (r2 >= 0 && c1 >= 0 && r2 < height && c1 < width) C = data[r2 * width + c1];
    if (r2 >= 0 && c2 >= 0 && r2 < height && c2 < width) D = data[r2 * width + c2];

    return max(0.f, A - B - C + D);
}
static int buildOneFloor(float* srcImage, float* destImage, SurfParameter* mSurfParemter, int pyramid, int srcWidth, int srcHeight)
{
    int filter = mSurfParemter->responseFilter[pyramid].filter;

    int step = mSurfParemter->responseFilter[pyramid].step;
    int b = (filter - 1) / 2;
    int l = filter / 3;
    int w = filter;
    float inverseArea = 1.0f / (w * w);
    float Dxx, Dyy, Dxy;
    int destWidth, destHeight;
    int index = 0, srcX, srcY, x, y;
    destWidth = mSurfParemter->responseFilter[pyramid].size.width;
    destHeight = mSurfParemter->responseFilter[pyramid].size.height;
    for(y = 0; y < destHeight; y ++)
    {
        for(x = 0; x < destWidth; x ++, index ++)
        {
            srcY = y * step;
            srcX = x * step;
            Dxx =  boxIntegral(srcImage, srcY - l + 1, srcX - b,     2 * l - 1, w,         srcWidth, srcHeight)
                -  boxIntegral(srcImage, srcY - l + 1, srcX - l / 2, 2 * l - 1, l,         srcWidth, srcHeight) * 3;
            Dyy =  boxIntegral(srcImage, srcY - b,     srcX - l + 1, w,         2 * l - 1, srcWidth, srcHeight)
                -  boxIntegral(srcImage, srcY - l / 2, srcX - l + 1, l,         2 * l - 1, srcWidth, srcHeight) * 3;
            Dxy =  boxIntegral(srcImage, srcY - l,     srcX + 1,     l,         l,         srcWidth, srcHeight)
                +  boxIntegral(srcImage, srcY + 1,     srcX - l,     l,         l,         srcWidth, srcHeight)
                -  boxIntegral(srcImage, srcY - l,     srcX - l,     l,         l,         srcWidth, srcHeight)
                -  boxIntegral(srcImage, srcY + 1,     srcX + 1,     l,         l,         srcWidth, srcHeight);

            // Normalise the filter responses with respect to their size
            Dxx *= inverseArea;
            Dyy *= inverseArea;
            Dxy *= inverseArea;

            // Get the determinant of hessian response & laplacian sign
            destImage[index] = (Dxx * Dyy - 0.81f * Dxy * Dxy);
        }
    }   

    return 0;
}
static int ISBuildResponseLayer(float* srcImage, CBufferList pyramidImages, SurfParameter* mSurfPara)
{
    float** bufferList = NULL;
    int srcWidth = 0, srcHeight = 0;
    int i;
    srcWidth = mSurfPara->srcSize.width;
    srcHeight = mSurfPara->srcSize.height;
    bufferList = getBufferList();
    for(i = 0; i < mSurfPara->pyramids; i ++)
    {
        buildOneFloor(srcImage, bufferList[pyramidImages + i], mSurfPara, i, srcWidth, srcHeight);
    }
    return 0;
}

//static int isExtremum(float* t, float* m, float* b, 
//                      int twidth, int theight, int mwidth, int mheight, int bwidth, int bheight,
//                      SurfParameter* mSurfPara, int filter, int step, int x, int y)
static int isExtremum(float* t, float* m, float* b,
	int twidth, int theight, int mwidth, int bwidth,
	SurfParameter* mSurfPara, int filter, int step, int x, int y)
{
    int rr, cc;
    int layerBorder = (filter + 1) / (2 * step);
    int m2t_scale = mwidth / twidth;
    int b2t_scale = bwidth / twidth;
    float mcandidate = m[(m2t_scale * y) * mwidth + (m2t_scale * x)];

    if(y <= layerBorder || y >= theight - layerBorder || x <= layerBorder || x >= twidth - layerBorder)
        return 0;
    if(mcandidate < mSurfPara->thres)
        return 0;

    for(rr = -1; rr <= 1; rr ++)
    {
        for(cc = -1; cc <= 1; cc ++)
        {
            float tcandidate = t[(rr + y) * twidth + (cc + x)];
            float m2tcandidate = m[(m2t_scale * (rr + y)) * mwidth + m2t_scale * (cc + x)];
            float b2tcandidate = b[(b2t_scale * (rr + y)) * bwidth + b2t_scale * (cc + x)];
            if(tcandidate >= mcandidate || ((rr != 0 || cc != 0) && m2tcandidate >= mcandidate) || b2tcandidate >= mcandidate)
                return 0;
        }
    }
    return 1;
}

static void deriv3D(int x, int y, float* t, float* m, float* b, float dI[3], int mwidth, int twidth, int bwidth)
{
    float dx, dy, ds;
    int m2t_scale = mwidth / twidth;
    int b2t_scale = bwidth / twidth;
    dx = (m[(y * m2t_scale) * mwidth + (x + 1) * m2t_scale] - m[(y * m2t_scale) * mwidth + (x - 1) * m2t_scale]) / 2.0f;
    dy = (m[((y + 1) * m2t_scale) * mwidth + x * m2t_scale] - m[((y - 1) * m2t_scale) * mwidth + x * m2t_scale]) / 2.0f;
    ds = (t[y * twidth + x] - b[(y * b2t_scale) * bwidth + x * b2t_scale]) / 2.0f;
    dI[0] = dx;
    dI[1] = dy;
    dI[2] = ds;
}

static void hessian3D(int x, int y, float* t, float* m, float* b, float H[3][3], int twidth, int mwidth, int bwidth)
{
    float v, dxx, dyy, dss, dxy, dxs, dys;
    int m2t_scale = mwidth / twidth;
    int b2t_scale = bwidth / twidth;

    v = m[(y * m2t_scale) * mwidth + x * m2t_scale];
    dxx = m[(y * m2t_scale) * mwidth + (x + 1) * m2t_scale] + 
          m[(y * m2t_scale) * mwidth + (x - 1) * m2t_scale] - 2 * v;
    dyy = m[((y + 1) * m2t_scale) * mwidth + x * m2t_scale] + 
          m[((y - 1) * m2t_scale) * mwidth + x * m2t_scale] - 2 * v;
    dss = t[y * twidth + x] + 
          b[(y * b2t_scale) * bwidth + x * b2t_scale] - 2 * v;

    dxy = (m[((y + 1) * m2t_scale) * mwidth + (x + 1) * m2t_scale] - 
          m[((y + 1) * m2t_scale) * mwidth + (x - 1) * m2t_scale] - 
          m[((y - 1) * m2t_scale) * mwidth + (x + 1) * m2t_scale] + 
          m[((y - 1) * m2t_scale) * mwidth + (x - 1) * m2t_scale]) / 4.0f;

    dxs = (t[y * twidth + x + 1] - 
          t[y * twidth + x - 1] - 
          b[(y * b2t_scale) * bwidth + (x + 1) * b2t_scale] +
          b[(y * b2t_scale) * bwidth + (x - 1) * b2t_scale]) / 4.0f;

    dys = (t[(y + 1) * twidth + x] - 
          t[(y - 1) * twidth + x] - 
          b[((y + 1) * b2t_scale) * bwidth + x * b2t_scale] + 
          b[((y - 1) * b2t_scale) * bwidth + x * b2t_scale]) / 4.0f;

    H[0][0] = dxx;
    H[0][1] = dxy;
    H[0][2] = dxs;
    H[1][0] = dxy;
    H[1][1] = dyy;
    H[1][2] = dys;
    H[2][0] = dxs;
    H[2][1] = dys;
    H[2][2] = dss;
}


static void interpolateStep(int x, int y, float* t, float* m, float* b, int mwidth, int twidth, int bwidth, float* xi, float* xr, float* xc)
{
    float dD[3];
    float H[3][3];
    float H_inv[3][3];

    float _a, _b, _c, _d, _e, _f, _g, _h, _k;
    float det = 0.0f;
    float det_inv = 0.0f;
    deriv3D(x, y, t, m, b, dD, mwidth, twidth, bwidth);
    hessian3D(x, y, t, m, b, H, twidth, mwidth, bwidth);

    _a = H[0][0];
    _b = H[0][1];
    _c = H[0][2];
    _d = H[1][0];
    _e = H[1][1];
    _f = H[1][2];
    _g = H[2][0];
    _h = H[2][1];
    _k = H[2][2];
    det = _a*(_e*_k - _f*_h) + _b*(_f*_g - _k*_d) + _c*(_d*_h - _e*_g);
    det_inv = 1.0f / det;
    H_inv[0][0] = (_e*_k - _f*_h)*det_inv;
    H_inv[0][1] = (_c*_h - _b*_k)*det_inv;
    H_inv[0][2] = (_b*_f - _c*_e)*det_inv;

    H_inv[1][0] = (_f*_g - _d*_k)*det_inv;
    H_inv[1][1] = (_a*_k - _c*_g)*det_inv;
    H_inv[1][2] = (_c*_d - _a*_f)*det_inv;

    H_inv[2][0] = (_d*_h - _e*_g)*det_inv;
    H_inv[2][1] = (_g*_b - _a*_h)*det_inv;
    H_inv[2][2] = (_a*_e - _b*_d)*det_inv;

    *xc = (-1)*(H_inv[0][0] * dD[0] + H_inv[1][0] * dD[1] + H_inv[2][0] * dD[2]);
    *xr = (-1)*(H_inv[0][1] * dD[0] + H_inv[1][1] * dD[1] + H_inv[2][1] * dD[2]);
    *xi = (-1)*(H_inv[0][2] * dD[0] + H_inv[1][2] * dD[1] + H_inv[2][2] * dD[2]);
}


//static int interpolateExtremum(float* t, float* m, float* b, int mfilter, int bfilter, int tstep, int x, int y, SurfParameter* mSurfPara,
//                               int twidth, int theight, int mwidth, int mheight, int bwidth, int bheight, keyPointInfo* mkeyPoints)
static int interpolateExtremum(float* t, float* m, float* b, int mfilter, int bfilter, int tstep, int x, int y, SurfParameter* mSurfPara,
	int twidth, int mwidth, int bwidth, keyPointInfo* mkeyPoints)
{
    int filterStep = mfilter - bfilter;
    float xi = 0, xr = 0, xc = 0;
    interpolateStep(x, y, t, m, b, mwidth, twidth, bwidth, &xi, &xr, &xc);
    if(fabs(xi) < 0.5f && fabs(xr) < 0.5f && fabs(xc) < 0.5f)
    {
        int count = mkeyPoints->count;
        mkeyPoints->mkeyPoint[count].x = (x + xc) * tstep;
        mkeyPoints->mkeyPoint[count].y = (y + xr) * tstep;
        mkeyPoints->mkeyPoint[count].scale = 0.1333f * (mfilter + xi * filterStep);
        mkeyPoints->count ++;
    }

    return 0;
}


static int getKeysPoint(CBufferList pyramidImages, keyPointInfo* mkeyPoints)
{
    int o, i;
    int r, c;
    SurfParameter* mSurfParameter = NULL;
    float** bufferList = NULL;

    int bwidth, bheight;
    int mwidth, mheight;
    int twidth, theight;
    int tstep;
    int tfiter;
    int mfilter;
    int bfilter;
    float* t = NULL;
    float* m = NULL;
    float* b = NULL;
    mSurfParameter = getSurfarameter();
    bufferList = getBufferList();    

    ISBuildResponseLayer(bufferList[Buffer_integral_image], Buffer_pyramid_image, mSurfParameter);
    for(o = 0; o < mSurfParameter->octaves; o ++)
    {
        for(i = 0; i < 2; i ++)
        {
            b = bufferList[pyramidImages + mSurfParameter->filter_map[o][i]];
            m = bufferList[pyramidImages + mSurfParameter->filter_map[o][i + 1]];
            t = bufferList[pyramidImages + mSurfParameter->filter_map[o][i + 2]];

            bwidth = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i]].size.width;
            bheight = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i]].size.height;
            mwidth = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 1]].size.width;
            mheight = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 1]].size.height;
            twidth = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 2]].size.width;
            theight = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 2]].size.height;

            tstep = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 2]].step;
            tfiter = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 2]].filter;
            bfilter = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i]].filter;
            mfilter = mSurfParameter->responseFilter[mSurfParameter->filter_map[o][i + 1]].filter;

            for(r = 0; r < theight; r ++)
            {
                for(c = 0; c < twidth; c ++)
                {
                    if(isExtremum(t, m, b, twidth, theight, mwidth, bwidth, mSurfParameter, tfiter, tstep, c, r))
                    {
                        interpolateExtremum(t, m, b, mfilter, bfilter, tstep, c, r, mSurfParameter,
                            twidth, mwidth, bwidth, mkeyPoints);
                    }
                }
            }
        }
    }
    return 0;
}

static float haarX(float* image, int y, int x, int s, int width, int height)
{
    return (boxIntegral(image, y - s / 2, x, s, s / 2, width, height) - boxIntegral(image, y - s / 2, x - s / 2, s, s / 2, width, height));
}
static float haarY(float* image, int y, int x, int s, int width, int height)
{
    return (boxIntegral(image, y, x - s / 2, s / 2, s, width, height) - 
        boxIntegral(image, y - s / 2, x - s / 2, s / 2, s, width, height));
}
static float getAngle(float X, float Y)
{
    if (X > 0 && Y >= 0)
        return (float)atan(Y / X);

    if (X < 0 && Y >= 0)
        return PI - (float)atan(-Y / X);

    if (X < 0 && Y < 0)
        return PI + (float)atan(Y / X);

    if (X > 0 && Y < 0)
        return 2 * PI - (float)atan(-Y / X);

    return 0.0f;
}
static void getOrientation(keyPoint* oneKey, int width, int height, float* image)
{
    float gauss = 0.0f;
    int s = 0, r = 0, c = 0;
    float resX[109], resY[109], Ang[109];
    int id[13] = { 6,5,4,3,2,1,0,1,2,3,4,5,6 };
    int idx = 0;
    int i, j;
    int angSize;

    float sumX = 0.0f, sumY = 0.0f;
    float maxValue = 0.0f, orientation = 0.0f;
    float ang1 = 0.0f, ang2 = 0.0f;

    s = fRound(oneKey->scale);
    r = fRound(oneKey->y);
    c = fRound(oneKey->x);

    for(i = -6; i <= 6; i ++)
    {
        for(j = -6; j <= 6; j ++)
        {
            if(i * i + j * j < 36)
            {
                gauss = gauss25[id[i + 6]][id[j + 6]];
                resX[idx] = gauss * haarX(image, r + j * s, c + i * s, 4 * s, width, height);
                resY[idx] = gauss * haarY(image, r + j * s, c + i * s, 4 * s, width, height);
                Ang[idx] = getAngle(resX[idx], resY[idx]);
                idx ++;
            }
        }
    }
    angSize = idx;
    for(ang1 = 0; ang1 < 2 * PI; ang1 += 0.15f)
    {
        int k = 0;
        ang2 = ((ang1 + PI / 3.0f) > (2 * PI) ? (ang1 - 5.0f * PI / 3.0f) : (ang1 + PI / 3.0f));
        sumX = 0.0f;
        sumY = 0.0f;
        for(k = 0; k < angSize; k ++)
        {
            float ang = Ang[k];
            if(ang1 < ang2 && ang1 < ang && ang < ang2)
            {
                sumX += resX[k];
                sumY += resY[k];
            }else if(ang2 < ang1 && ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2 * PI)))
            {
                sumX += resX[k];
                sumY += resY[k];
            }
        }
        if((sumX * sumX + sumY * sumY) > maxValue)
        {
            maxValue = sumX * sumX + sumY * sumY;
            orientation = getAngle(sumX, sumY);
        }
    }
    oneKey->angle = orientation;
}
static float gaussian(float x, float y, float sig)
{
    return (float)(1.0f / (2.0f*PI*sig*sig)) * (float)exp(-(x*x + y*y) / (2.0f*sig*sig));
}
static void getDescriptor(keyPoint* oneKey, int width, int height, float* image)
{
    int y, x, sample_x, sample_y, count = 0;
    int i = 0, ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
    float scale, dx, dy, mdx, mdy, co, si;
    float gauss_s1 = 0.0f, gauss_s2 = 0.0f;
    float rx = 0.0f, ry = 0.0f, rrx = 0.0f, rry = 0.0f, len = 0.0f;
    float cx = -0.5f, cy = 0.0f;
    scale = oneKey->scale;
    x = (int)fRound(oneKey->x);
    y = (int)fRound(oneKey->y);

    co = (float)cos(oneKey->angle);
    si = (float)sin(oneKey->angle);
    i = -8;
    while(i < 12)
    {
        j = -8;
        i = i - 4;

        cx += 1.0f;
        cy = -0.5f;

        while(j < 12)
        {
            int k, l;
            float descriptVec[8] = {0.0f};
            dx = 0.0f;
            dy = 0.0f;
            mdx = 0.0f;
            mdy = 0.0f;
            cy += 1.0f;

            j = j - 4;
            jx = i + 5;
            jx = j + 5;
            xs = fRound(x + (-jx * scale * si + ix * scale * co));
            ys = fRound(y + (jx * scale * co + ix * scale * si));

            for(k = i; k < i + 9; k ++)
            {
                for(l = j; l < j + 9; l ++)
                {
                    sample_x = fRound(x + (-l * scale * si + k * scale * co));
                    sample_y = fRound(y + (l * scale * co + k * scale * si));
                    gauss_s1 = gaussian((float)(xs - sample_x), (float)(ys - sample_y), 2.5f*scale);
                    rx = haarX(image, sample_y, sample_x, 2 * fRound(scale), width, height);
                    ry = haarY(image, sample_y, sample_x, 2 * fRound(scale), width, height);

                    //Get the gaussian weighted x and y responses on rotated axis
                    rrx = gauss_s1*(-rx*si + ry*co);
                    rry = gauss_s1*(rx*co + ry*si);

                    //descriptVec
                    if(rry >= 0.0f)
                    {
                        descriptVec[0] += rrx;
                        descriptVec[1] += (float)fabs(rrx);
                    }else
                    {
                        descriptVec[2] += rrx;
                        descriptVec[3] += (float)fabs(rrx);
                    }
                    if(rrx >= 0.0f)
                    {
                        descriptVec[4] += rry;
                        descriptVec[5] += (float)fabs(rry);
                    }else
                    {
                        descriptVec[6] += rry;
                        descriptVec[7] += (float)fabs(rry);
                    }
                }
            }
            gauss_s2 = gaussian(cx - 2.0f, cy - 2.0f, 1.5f);
            oneKey->vec[count + 0] = descriptVec[0] * gauss_s2;
            oneKey->vec[count + 1] = descriptVec[1] * gauss_s2;
            oneKey->vec[count + 2] = descriptVec[2] * gauss_s2;
            oneKey->vec[count + 3] = descriptVec[3] * gauss_s2;
            oneKey->vec[count + 4] = descriptVec[4] * gauss_s2;
            oneKey->vec[count + 5] = descriptVec[5] * gauss_s2;
            oneKey->vec[count + 6] = descriptVec[6] * gauss_s2;
            oneKey->vec[count + 7] = descriptVec[7] * gauss_s2;
            count += 8;
            j += 9;
        }
        i += 9;
    }

    for(i = 0; i < Alva_VecLength; i ++)
    {
        len += (oneKey->vec[i] * oneKey->vec[i]);
    }
    len = sqrtf(len);
    for(i = 0; i < Alva_VecLength; i ++)
    {
        oneKey->vec[i] /= len;
    }
}
static void getDescriptors(keyPointInfo* mkeyPoints, float* srcImage, int srcWidth, int srcHeight)
{
    int i = 0;
    for(i = 0; i < mkeyPoints->count; i ++)
    {
        getOrientation(&(mkeyPoints->mkeyPoint[i]), srcWidth, srcHeight, srcImage);
        getDescriptor(&(mkeyPoints->mkeyPoint[i]), srcWidth, srcHeight, srcImage);
    }
}

int computeImageFeatures(float* src, int width, int height, keyPointInfo* mkeyPoints)
{
    float** bufferList = NULL;
    bufferList = getBufferList();
    memcpy(bufferList[Buffer_gray_src], src, sizeof(float) * width * height);

    ISIntegral(bufferList[Buffer_gray_src], bufferList[Buffer_integral_image], width, height);
    getKeysPoint(Buffer_pyramid_image, mkeyPoints);
    getDescriptors(mkeyPoints, bufferList[Buffer_integral_image], width, height);

    return 0;
}
