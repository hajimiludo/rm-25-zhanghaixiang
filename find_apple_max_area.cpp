#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//下面图像是一个红苹果，现在需要编写一个程序
// 自动的提取苹果的轮廓，并用一个矩形将苹果框出来。

/// <summary>
/// 使用HSV颜色空间提取红色，结合形态学处理去除噪点
/// </summary>
/// <param name="img_apple">目标提取图像</param>
/// <returns>返回二值mask，苹果为白色，其余部分为黑色</returns>
Mat Process(Mat img_apple) 
{
    Mat imghsv;
    // 转换颜色空间  BGR--HSV
    cvtColor(img_apple, imghsv, COLOR_BGR2HSV);


    // 定义红色的HSV阈值范围
    // 范围1：低Hue值部分
    Scalar lower_red1(0, 160, 80);  // H:0-21， S:160-255 ，V：80-255
    Scalar upper_red1(21, 245, 255);
    // 范围2: 高Hue值部分
    Scalar lower_red2(160, 60, 60);
    Scalar upper_red2(180, 255, 255);

    Mat mask1, mask2, mask;

    // 分别提取两个范围的mask，然后合并
    inRange(imghsv, lower_red1, upper_red1, mask1);
    inRange(imghsv, lower_red2, upper_red2, mask2);
    mask = mask1 | mask2;

    // 形态学处理
    // 创建椭圆结构元素，
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
    // 开运算：先腐蚀后膨胀，去除小噪点
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    // 闭运算：先膨胀后腐蚀，链接临近区域
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    // 膨胀：填补内部空洞
    dilate(mask, mask, kernel);

    return mask;
}

/// <summary>
/// 找到苹果轮廓并绘制矩形
/// </summary>
/// <param name="mask">二值化后的mask，寻找苹果轮廓</param>
/// <param name="img_apple">原始BGR图像，绘制矩形</param>
/// <returns>绘制了矩形的BGR图像</returns>
Mat contours_rect(Mat mask, Mat img_apple) 
{
    // 深拷贝，用于绘制结果
    Mat result = img_apple.clone();

    // 存储检测到的轮廓和层级信息
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // 寻找轮廓
    // 
    findContours(mask, contours, hierarchy, 
        RETR_EXTERNAL, 
        CHAIN_APPROX_SIMPLE);
    
    int maxAreaIdx = -1;
    double maxArea = 0;
    for (size_t i = 0; i < contours.size(); i++) 
    {
        double area = contourArea(contours[i]);
        // 如果当前面积大于记录的最大面积则更新
        if (area > maxArea)
        {
            maxArea = area;
            maxAreaIdx = i;
        }
    }


    // 绘制矩形
    // 
    if (maxAreaIdx != -1)
    {
        // 绘制矩形
        drawContours(result, contours, maxAreaIdx, Scalar(255, 100, 100), 2);

        //计算外接矩形
        Rect rect = boundingRect(contours[maxAreaIdx]);
        //绘制矩形
        rectangle(result, rect, Scalar(255, 100, 100), 2);
    }
    else cout << "no find any apple" << endl;

    return result;
}

int main() {
    // 加载图像
    Mat apple = imread("./pictures/apple.png");
    // 提取mask
    Mat mask = Process(apple);
    // 绘制矩形
    Mat result = contours_rect(mask, apple);
    // 显示结果
    imshow("apple", apple);
    imshow("mask", mask);
    imshow("result", result);
    waitKey(0);

    return 0;
}