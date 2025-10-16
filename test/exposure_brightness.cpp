#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

/////////////////////////给视频创造调节曝光和亮度的窗口的代码
struct exposure_brightness {
    int exp;
    int bri;
}EB1;//便于返回曝光和亮度的值

int exposure = 100, brightness = 100;
void onTrackbar(int, void*) {
    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
}// 在回调函数中更新全局结构体

exposure_brightness Trac() {
    namedWindow("Trackbar", (640, 50));
    createTrackbar("曝光", "Trackbar", &exposure, 200, onTrackbar);
    createTrackbar("亮度", "Trackbar", &brightness, 200, onTrackbar);

    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
    return EB1;
}//创建滑动条

int main() {
    VideoCapture cap(0);//打开视频
    Mat img;

    Trac();//创建滑动条窗口

    while (true) {
        cap.read(img);//导入视频帧
        img.convertTo(img, -1, 1.0 + EB1.exp / 100.0, EB1.bri);//用滑动条改变帧曝光和亮度
        imshow("Video", img);
        waitKey(20);
    }
    return 0;
}