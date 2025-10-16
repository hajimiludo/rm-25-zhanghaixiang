#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

//-色彩分割：将图片中红色通过色彩分割的方法二值化成黑白图像（目标颜色为白，其余背景为黑）。
Mat imgBW(const Mat& img) {
    Mat imgHSV, mask, mask0;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    //red:140,179,215,255,231,255
    int hmin = 140, smin = 215, vmin = 231;
    int hmax = 179, smax = 255, vmax = 255;

    Scalar lower(hmin, smin, vmin);
    Scalar upper(hmax, smax, vmax);
    inRange(imgHSV, lower, upper, mask0);

    //开运算，用于去除小物体或噪声
    Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
    morphologyEx(mask0, mask, MORPH_CLOSE, kernel);

    return mask;
}

//- 将色彩分割后的图进行边缘提取，并画出边缘（浅蓝色），背景为黑色。 （提示： Canny 算法）
Mat getcontours(const Mat& mask) {
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;
    Mat imgBlur, imgCanny;

    //模糊
    GaussianBlur(mask, imgBlur, Size(3, 3), 3, 0);
    //边缘检测
    Canny(imgBlur, imgCanny, 25, 75);
    imshow("Image canny", imgCanny);

    //画出边缘
    findContours(imgCanny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat imgB(mask.size(), CV_8UC3, Scalar(0, 0, 0));
    drawContours(imgB, contours, -1, Scalar(235, 206, 135), 4);

    imshow("Imagecontour", imgB);

    return imgB;
}

//- 用窗口输入原图、黑白图像、边缘提取图像，图像大小为 1280 * 720 。

//- 将边缘图像保存在本地。
void store(const Mat& img) {
    bool success = imwrite("./rm/rm_test/imgEdge.png", img);

    if (success) {
        cout << "图像保存成功！" << endl;
    }
    else {
        cout << "图像保存失败！" << endl;
    }
}

int main() {
    //获取图片，并修改大小
    string path = "rm/rm_test/0b136e342cc8ff725a47d9d81fd97e0.png";
    Mat imgori = imread(path);
    Mat img;
    resize(imgori, img, Size(1280, 720));

    //将图片中红色通过色彩分割的方法二值化成黑白图像
    Mat imgbw = imgBW(img);
    Mat imgEdge = getcontours(imgbw);
    //保存图片
    //store(imgEdge);

    imshow("Image", img);
    imshow("Image_Black_White", imgbw);
    waitKey(0);


    return 0;
}