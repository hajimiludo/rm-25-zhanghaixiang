#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

//题目（2）
//下面需要你通过运用代码打开电脑摄像头（或其他用到的摄像头），录制一段视频，要求如下：
//- 打开摄像头，并用窗口显示
//- 画面里输出的摄像头信息主要包括视频图像大小、帧率（FPS）
//- 使用滑动条调节摄像头曝光时间、亮度，要求有高低曝光、明暗亮度的展示（提示：createTrackbar，部分电脑摄像头可能无法调节曝光时间，可跳过调节曝光这一项）
//- 录制完视频后保存在本地

struct exposure_brightness {
    int exp;
    int bri;
}EB1;//便于返回曝光和亮度的值

int exposure = 100, brightness = 100;
void onTrackbar(int, void*) {
    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
}// 在回调函数中更新全局结构体

void creatTrac() {
    namedWindow("Trackbar", WINDOW_NORMAL);
    resizeWindow("Trackbar", 640, 50);
    createTrackbar("曝光", "Trackbar", &exposure, 200, onTrackbar);
    createTrackbar("亮度", "Trackbar", &brightness, 200, onTrackbar);
}//创建滑动条

int main() {
    // 打开摄像头
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "无法打开摄像头" << endl;
        return -1;
    }
    Mat img;

    creatTrac();//创建滑动条

    double fps = cap.get(CAP_PROP_FPS);
    if (fps <= 0) fps = 30; // 如果获取失败，使用默认值
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    Size imgSize(width, height);

    //储存视频
    VideoWriter writer("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imgSize);
    //使用滑动条调节摄像头曝光时间、亮度，
    
//显示视频帧
    while (true) {
        cap.read(img);
        if (img.empty()) break;
        writer.write(img);//储存视频

        // 在读取的帧上绘制文本
        double fps = cap.get(CAP_PROP_FPS);
        string FPS = "FPS: " + to_string(fps);
        Size imgSize = img.size();
        string SIZE = "Size: " + to_string(width) + "x" + to_string(height);
        putText(img, FPS, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 200, 255), 2);
        putText(img, SIZE, Point(20, 50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 200, 255), 2);

        cap.set(CAP_PROP_EXPOSURE, EB1.exp);
        cap.set(CAP_PROP_BRIGHTNESS, EB1.bri);

        imshow("Video", img);
        waitKey(20);
    }

}