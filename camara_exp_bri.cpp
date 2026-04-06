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

// 全局变量，用于滑动条控制曝光和亮度
int exposure = 0;
int brightness = 120;

// 滑动条回调函数，当滑动条移动时自动调用
void onTrackbar(int, void*) {
    // 无需而外操作，cap.set()可以直接使用全局变量
    // 回调主要用于实时响应，实际设置在主循环中进行
}

// 创建滑动条控制窗口
void createTracbarWindow() {
    namedWindow("Trackbar", WINDOW_NORMAL);// 创建控制窗口
    resizeWindow("Trackbar", 640, 50);
    // 创建曝光滑动条
    createTrackbar("Exposure", "Trackbar", &exposure, 20, onTrackbar);
    // 创建亮度滑动条
    createTrackbar("Brightness", "Trackbar", &brightness, 255, onTrackbar);
}


int main() {
    // 打开摄像头
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Failed to open camera" << endl;
        return -1;
    }

    // 尝试获取摄像头原始参数
    double fps = cap.get(CAP_PROP_FPS);
    if (fps <= 0) {
        cout << "failed to get fps" << endl;
        fps = 30; // 若无法获取。默认设置为30fps
    }
    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    Size imgSize(width, height);

    // 保存视频
    VideoWriter writer("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imgSize);

    // 初始化滑动条控制窗口
    createTracbarWindow();

    Mat img;
    cout << "camera is on, enter 'q' to quit" << endl;
    
    while (true) {
        // 设置摄像头属性
        // 
        cap.set(CAP_PROP_EXPOSURE, exposure - 10);
        cap.set(CAP_PROP_BRIGHTNESS, brightness);

        // 读取一帧
        cap.read(img);
        if (img.empty()){
            cout << "ero1" << endl;
            break;
        }

        // 写入视频文件
        //if (writer.isOpened()) {
        //    writer.write(img);
        //}

        // 在画面上显示信息
        //double fps = cap.get(CAP_PROP_FPS);
        //string FPS = "FPS: " + to_string(fps);
        //Size imgSize = img.size();

        string FPS = "FPS:" + to_string(static_cast<int>(fps));
        string SIZE = "Size: " + to_string(width) + "x" + to_string(height);
        putText(img, FPS, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 200, 255), 2);
        putText(img, SIZE, Point(20, 50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 200, 255), 2);

        // 显示视频流
        imshow("Video", img);
        //
        int key = waitKey(30);// 等待30ms
        if (key == 'q' || key == 27) { // 27是esc
            break;
        }
    }

    // 释放资源
    cap.release();
    writer.release();
    destroyAllWindows();

    return 0;
}
