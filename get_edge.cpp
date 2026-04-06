#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

//1.基础题
//题目（1）下面有一张图片，里面有不同形状、不同颜色的图案，要求用opencv实现下列功能：
//- 色彩分割：将图片中红色通过色彩分割的方法二值化成黑白图像（目标颜色为白，其余背景为黑）。
//- 将色彩分割后的图进行边缘提取，并画出边缘（浅蓝色），背景为黑色。 （提示： Canny 算法）
//- 用窗口输入原图、黑白图像、边缘提取图像，图像大小为 1280 * 720 。
//- 将边缘图像保存在本地。


// 定义颜色阈值参数结构体，便于管理和调整
/*
HSV色彩空间：
H (Hue)：色调，表示颜色的种类（0-180，OpenCV中范围是0-179）
S (Saturation)：饱和度，表示颜色的纯度（0-255）
V (Value)：亮度，表示颜色的明暗程度（0-255）*/
struct ColorThreshold 
{
    int h_min = 140;  // HSV色相最小值
    int s_min = 215;  // HSV饱和度最小值
    int v_min = 231;  // HSV亮度最小值
    int h_max = 179;  // HSV色相最大值
    int s_max = 255;  // HSV饱和度最大值
    int v_max = 255;  // HSV亮度最大值
};


Mat imgBW(const Mat& img, const ColorThreshold& threshold) 
{
    Mat imgHSV, mask;
    // 把BGR图像转换为HSV颜色空间
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    // 定义颜色范围上下限
    Scalar lower(threshold.h_min, threshold.s_min, threshold.v_min);
    Scalar upper(threshold.h_max, threshold.s_max, threshold.v_max);
    // 根据颜色范围创建掩码
    inRange(imgHSV, lower, upper, mask);

    // 形态学闭运算，填充小孔洞，链接相邻区域
    Mat kernel = getStructuringElement(MORPH_RECT, Size(9,9));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    return mask;
}

/// <summary>
/// 查找并绘制轮廓
/// </summary>
/// <param name="mask"></param>
/// <returns>边缘图片</returns>
Mat getcontours(const Mat& mask) 
{
    // 高斯模糊去噪，减少边缘检测的干扰
    Mat imgBlur;
    GaussianBlur(mask, imgBlur, Size(3, 3), 3, 0);

    // Canny 边缘检测，提取边缘
    Mat imgCanny;
    Canny(imgBlur, imgCanny, 25, 75);
    imshow("Image canny", imgCanny);

    // 查找轮廓
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;
    findContours(imgCanny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 创建黑色背景图像用于绘制轮廓
    Mat imgEdge(mask.size(), CV_8UC3, Scalar(0, 0, 0));
    // 绘制轮廓
    if(!contours.empty()){
        drawContours(imgEdge, contours, -1, Scalar(235, 206, 135), 4);
    }

    return imgEdge;
}

/// <summary>
/// 保存图像到processed_pictures文件夹中
/// </summary>
/// <param name="img"></param>
void store(const Mat& img)
{
    // 保存图像
    bool success = imwrite("./processed_pictures/edge.png", img);

    if (success) 
    {
        cout << "Image saved successfully" << endl;
    }else 
    {
        cout << "Failed to save image" << endl;
    }
}

int main() {
    // 图像路径
    string path = "pictures/edge.png";

    // 读取图像
    Mat imgori = imread(path);
    // 调整图像尺寸
    Mat img;
    resize(imgori, img, Size(1280, 720));

    // 设置颜色阈值参数
    ColorThreshold threshold;
    // 色彩分割，提取边缘
    Mat imgbw = imgBW(img, threshold);
    Mat imgEdge = getcontours(imgbw);

    // 保存图像
    store(imgEdge);

    // 显示图像
    imshow("Image", img);
    imshow("Image_Black_White", imgbw);
    imshow("Imagecontour", imgEdge);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
