#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void findcolor(Mat img) {
    Mat imgHSV, mask;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    int hmin = 100, smin = 43, vmin = 46;
    int hmax = 124, smax = 255, vmax = 255;

    namedWindow("Trackbar", (640, 200));
    createTrackbar("Hue Min", "Trackbar", &hmin, 179);
    createTrackbar("Hue Max", "Trackbar", &hmax, 179);
    createTrackbar("Sat Min", "Trackbar", &smin, 255);
    createTrackbar("Sat Max", "Trackbar", &smax, 255);
    createTrackbar("Val Min", "Trackbar", &vmin, 255);
    createTrackbar("Val Max", "Trackbar", &vmax, 255);
    while (true) {
        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);
        inRange(imgHSV, lower, upper, mask);

        imshow("Image", img);
        imshow("Image HSV", imgHSV);
        imshow("Image Mask", mask);
        waitKey(1);
    }
}

void CreateRedMask(Mat img_apple) {
    Mat imghsv;
    cvtColor(img_apple, imghsv, COLOR_BGR2HSV);
    //// 定义红色的HSV范围
    //Scalar lower_red1(0, 160, 80);    // H(0-10), S(100-255), V(100-255)
    //Scalar upper_red1(21, 245, 255);
    //Scalar lower_red2(100, 60, 60);  // H(160-180), S(100-255), V(100-255)
    //Scalar upper_red2(180, 255, 255);

    int hmin1 = 0, smin1 = 100, vmin1 = 100;
    int hmax1 = 10, smax1 = 255, vmax1 = 255;

    namedWindow("Trackbars1", (640, 200));
    createTrackbar("Hue Min", "Trackbars1", &hmin1, 179);
    createTrackbar("Hue Max", "Trackbars1", &hmax1, 179);
    createTrackbar("Sat Min", "Trackbars1", &smin1, 255);
    createTrackbar("Sat Max", "Trackbars1", &smax1, 255);
    createTrackbar("Val Min", "Trackbars1", &vmin1, 250);
    createTrackbar("Val Max", "Trackbars1", &vmax1, 255);

    int hmin2 = 160, smin2 = 100, vmin2 = 100;
    int hmax2 = 179, smax2 = 255, vmax2 = 255;

    namedWindow("Trackbars2", (640, 200));
    createTrackbar("Hue Min", "Trackbars2", &hmin2, 179);
    createTrackbar("Hue Max", "Trackbars2", &hmax2, 179);
    createTrackbar("Sat Min", "Trackbars2", &smin2, 255);
    createTrackbar("Sat Max", "Trackbars2", &smax2, 255);
    createTrackbar("Val Min", "Trackbars2", &vmin2, 255);
    createTrackbar("Val Max", "Trackbars2", &vmax2, 255);
    //定义红色的HSV范围来提取苹果
    //需要两个范围,HSV色环是一个圆形,红色正好位于起点和终点位置

    Mat mask1, mask2, red_mask;
    while (true) {
        Scalar lower_red1(hmin1, smin1, vmin1);
        Scalar upper_red1(hmax1, smax1, vmax1);
        Scalar lower_red2(hmin2, smin2, vmin2);
        Scalar upper_red2(hmax2, smax2, vmax2);

        inRange(imghsv, lower_red1, upper_red1, mask1);
        inRange(imghsv, lower_red2, upper_red2, mask2);
        red_mask = mask1 | mask2;  // 合并两个红色范围

        imshow("apple", img_apple);
        imshow("imghsv", imghsv);
        imshow("mask", red_mask);
        waitKey(1);
    }
}

Mat ProcessMask(Mat mask) {
    Mat processed_mask = mask.clone();
    // 形态学操作去除噪声和填充空洞
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    // 开运算去除小的噪声点
    morphologyEx(processed_mask, processed_mask, MORPH_OPEN, kernel);
    // 闭运算填充内部小空洞
    morphologyEx(processed_mask, processed_mask, MORPH_CLOSE, kernel);
    imshow("processed_mask",processed_mask);
    waitKey(0);

    return processed_mask;
}

//装甲板蓝：0,255,0,255,200,255
int main() {
    string path = "./rm/rm_test/蓝1.jpg";
    Mat img = imread(path);
    if (img.empty()) {
        cout << "读取图像失败" << endl;
        return -1;
    }
    resize(img, img, Size(), 0.5, 0.5, INTER_AREA);
    //CreateRedMask(img);
    //ProcessMask(img);
    findcolor(img);

    //imshow("apple", img);
    //imshow("mask", mask);
    //waitKey(0);
    return 0;
}