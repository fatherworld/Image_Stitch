#ifndef _IS_h_
#define _IS_h_
//初始化
int ISInit(int& width, int& height);
//释放资源
int ISUnit();

//计算图像特征点
int ISFeature(vector<string>& imagesPath, vector<string>& featurePath);

//计算两张图像的匹配点
int ISMatching(vector<string>& featuresPath, vector<vector<string>>& matchings);


#endif