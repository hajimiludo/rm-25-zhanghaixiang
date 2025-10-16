#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

//float w, h;
//Mat matrix, imgWarp;


//绘制边缘
void getcontours(Mat imgDil, Mat& mask) {
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(mask, contours, -1, Scalar(150, 0, 0), 2);
    //噪点判断
     //vector<vector<Point>>conPoly(contours.size());
     //for (int i = 0; i < contours.size(); i++) {
     //    int area = contourArea(contours[i]);
     //    cout << area << endl;
     //    if (area > 12000) {
     //        float peri = arcLength(contours[i], true);
     //        approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
     //        drawContours(mask, contours, i, Scalar(235, 206, 135), 4);
     //    }
     //}
    imshow("Image contours", mask);

    waitKey(0);
}

//形态学处理
void Process() {
//二值化
cvtColor(img, imgGray,COLOR_BGR2GRAY);
//模糊
GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
//边缘检测
Canny(imgBlur, imgCanny, 25, 75);
//膨胀和腐蚀(增加厚度和减少厚度
Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
dilate(imgCanny,imgDil,kernel);
erode(imgDil, imgErode, kernel);
//开运算去除小的噪声点
morphologyEx(mask, mask, MORPH_OPEN, kernel);
//闭运算填补
morphologyEx(mask, mask, MORPH_CLOSE, kernel);
}

void imgsize() {
    //调整大小
    //cout << img.size() << endl;
    resize(img, imgResize, Size(),0.5,0.5);
    //裁剪
    Rect roi(100, 100, 300, 250);
    imgCrop = img(roi);
}

void createimg_rect_line_text() {
    Mat img1(512, 512, CV_8UC3, Scalar(255, 255, 255));
    circle(img1, Point(256, 256), 255, Scalar(0, 69, 255),-1);//draw a circle
    rectangle(img1,Point(130, 226), Point(382, 286), Scalar(255, 255, 255), 7);//draw a rectangle
    line(img1, Point(130, 295), Point(321, 133), Scalar(255, 255, 0), 3);
    putText(img, );
}

void PUTTEXT() {
    //在图像上显示文本
    // 设置文本内容
    std::string text = "Hello OpenCV!";
    // 设置文本位置（左下角坐标）
    cv::Point textOrg(50, 150);
    // 设置字体类型
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    // 设置字体大小缩放因子
    double fontScale = 1.0;
    // 设置文本颜色（BGR格式：蓝色）
    cv::Scalar color(255, 0, 0);
    // 设置文本线宽
    int thickness = 1;
    // 使用putText函数在图像上绘制文本
    cv::putText(image, text, textOrg, fontFace, fontScale, color, thickness);
    // 显示图像
    cv::imshow("Basic PutText Example", image);
    // 等待按键
    cv::waitKey(0);
}

//鼠标事件回调函数
void mcb(int event, int x, int y, int flags, void* userdata) {
    Mat* image = static_cast<Mat*>(userdata);
}

void mousetype() {
    EVENT_MOUSEMOVE = 0, // 鼠标移动
    EVENT_LBUTTONDOWN = 1, // 左键按下
    EVENT_RBUTTONDOWN = 2, // 右键按下
    EVENT_MBUTTONDOWN = 3, // 中键按下
    EVENT_LBUTTONUP = 4, // 左键释放
    EVENT_RBUTTONUP = 5, // 右键释放
    EVENT_MBUTTONUP = 6, // 中键释放
    EVENT_LBUTTONDBLCLK = 7, // 左键双击
    EVENT_RBUTTONDBLCLK = 8, // 右键双击
    EVENT_MBUTTONDBLCLK = 9, // 中键双击
    EVENT_MOUSEWHEEL = 10, // 鼠标滚轮
    EVENT_MOUSEHWHEEL = 11  // 鼠标水平滚轮
    EVENT_FLAG_LBUTTON = 1,  // 左键按下
    EVENT_FLAG_RBUTTON = 2,  // 右键按下
    EVENT_FLAG_MBUTTON = 4,  // 中键按下
    EVENT_FLAG_CTRLKEY = 8,  // Ctrl键按下
    EVENT_FLAG_SHIFTKEY = 16, // Shift键按下
    EVENT_FLAG_ALTKEY = 32  // Alt键按下
}

int main() {

    //导入图片
    string path = "rm/rm_test/0b136e342cc8ff725a47d9d81fd97e0.png";
    Mat img = imread(path);
    if (img.empty()) {
        cout << "读取图像失败" << endl;
        return -1;
    }
    // 打开摄像头
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "无法打开摄像头" << endl;
        return -1;
    }
    //获取帧率和图片尺寸
    double fps = cap.get(CAP_PROP_FPS);//fps
    Size imgSize(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));

    VideoWriter writer("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imgSize);
    //cout << "FPS:" << fps << endl;
    //cout << "SIZE" << imgSize << endl;
    //显示视频帧
    while (true) {
        cap.read(img);


        imshow("Video", img);
        waitKey(20);
    }

    //单目投影，projectPoints()计算3D-->2D
    //单目位姿估计，solvePnP()计算旋转向量和平移向量

    ////扭曲图像
    //Point2f src[4]={}
    //Point2f dst[4] = {}

    //matrix = getPerspectiveTransform(src, dst);//透视变换
    //warpPerspective(img, imgWarp, matrix, Point(w, h));

    return 0;
}