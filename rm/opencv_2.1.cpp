#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
/*题目*/
//（1）传统分割算法
//下面图像是一个红苹果，现在需要编写一个程序自动的提取苹果的轮廓，
//除了苹果的轮廓，其它（例如树叶、树枝) 的轮廓不要显示出来，
// 用一个矩形将苹果框出来。


//处理苹果图片
Mat Process(Mat img_apple) {
    Mat imghsv;
    cvtColor(img_apple, imghsv, COLOR_BGR2HSV);//转换为hsv颜色空间进行形态学处理

    //通过使用颜色检测函数（ColorTrackbar.cpp）来找到苹果红色的HSV范围
    //需要两个范围,HSV色环是一个圆形,红色正好位于起点和终点位置
    Scalar lower_red1(0, 160, 80);    // H(0-10), S(100-255), V(100-255)
    Scalar upper_red1(21, 245, 255);
    Scalar lower_red2(100, 60, 60);  // H(160-180), S(100-255), V(100-255)
    Scalar upper_red2(180, 255, 255);
    Mat mask1, mask2, mask;
    inRange(imghsv, lower_red1, upper_red1, mask1);
    inRange(imghsv, lower_red2, upper_red2, mask2);
    mask = mask1 | mask2;  // 合并两个红色范围,得到二值化的苹果图像

    //形态学处理图像
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    //开运算去除小的噪声点
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    dilate(mask, mask, kernel);
    //闭运算填补
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    return mask;
}

//绘制苹果轮廓以及矩形
Mat contours_rect(Mat mask,Mat img_apple){
    Mat img_contours = img_apple.clone();
    //找到轮廓和计算面积
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<double> areas;
    double maxarea = 0;
    //找到最大的面积，即苹果的面积
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        areas.push_back(area);
        cout << area << endl;
        if (maxarea < area) {
            maxarea = area;
        }
    }
    //绘制轮廓和矩形
    for (size_t i = 0; i < areas.size(); i++) {
        if (areas[i] >= maxarea) {
            //绘制苹果轮廓
            drawContours(img_contours, contours, i, Scalar(255, 0, 0), 2);
            //绘制一个矩形
            Rect rect = boundingRect(contours[i]);
            rectangle(img_contours, rect, Scalar(255, 0, 0), 2);
        }
    }
    //imshow("3", img_contours);

    return img_contours;
}

int main() {
    string path = "rm/rm_test/apple.png";
    Mat img_apple = imread(path);
    
    //形态学处理
    Mat mask = Process(img_apple);

    //找到并绘制轮廓
    Mat goal = contours_rect(mask, img_apple);
    
    imshow("img_apple", img_apple);
    imshow("mask", mask);
    imshow("goal", goal);
    waitKey(0);

    return 0;
}
