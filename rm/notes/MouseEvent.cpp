#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat image;
Point previousPoint;
bool drawing = false;

// 鼠标回调函数
void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
        // 1. 鼠标移动
    case EVENT_MOUSEMOVE:
        if (drawing) {
            // 在移动时绘制线条
            line(image, previousPoint, Point(x, y), Scalar(0, 255, 0), 2);
            previousPoint = Point(x, y);
            imshow("Mouse Events", image);
        }
        break;

        // 2. 左键按下
    case EVENT_LBUTTONDOWN:
        cout << "左键按下 - 坐标: (" << x << ", " << y << ")" << endl;
        previousPoint = Point(x, y);
        drawing = true;
        // 在点击位置画圆
        circle(image, Point(x, y), 5, Scalar(255, 0, 0), -1);
        imshow("Mouse Events", image);
        break;

        // 3. 左键释放
    case EVENT_LBUTTONUP:
        cout << "左键释放 - 坐标: (" << x << ", " << y << ")" << endl;
        drawing = false;
        break;

        // 4. 右键按下
    case EVENT_RBUTTONDOWN:
        cout << "右键按下 - 坐标: (" << x << ", " << y << ")" << endl;
        // 右键清除图像
        image = Mat::zeros(500, 500, CV_8UC3);
        putText(image, "Click and drag to draw", Point(50, 50),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        imshow("Mouse Events", image);
        break;

        // 5. 左键双击
    case EVENT_LBUTTONDBLCLK:
        cout << "左键双击 - 坐标: (" << x << ", " << y << ")" << endl;
        // 双击位置画矩形
        rectangle(image, Point(x - 15, y - 15), Point(x + 15, y + 15),
            Scalar(0, 0, 255), 2);
        imshow("Mouse Events", image);
        break;

        // 6. 右键双击
    case EVENT_RBUTTONDBLCLK:
        cout << "右键双击 - 坐标: (" << x << ", " << y << ")" << endl;
        break;

        // 7. 中键按下
    case EVENT_MBUTTONDOWN:
        cout << "中键按下 - 坐标: (" << x << ", " << y << ")" << endl;
        break;
    }

    // 检查组合键状态
    if (flags & EVENT_FLAG_CTRLKEY) {
        cout << "Ctrl键被按下" << endl;
    }
    if (flags & EVENT_FLAG_SHIFTKEY) {
        cout << "Shift键被按下" << endl;
    }
    if (flags & EVENT_FLAG_LBUTTON) {
        // 左键当前被按下
    }
}

int main() {
    // 创建空白图像
    image = Mat::zeros(500, 500, CV_8UC3);
    putText(image, "Click and drag to draw", Point(50, 50),
        FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    // 创建窗口并设置鼠标回调
    namedWindow("Mouse Events");
    setMouseCallback("Mouse Events", mouseCallback, NULL);

    imshow("Mouse Events", image);

    cout << "使用说明:" << endl;
    cout << "- 左键点击: 画蓝点" << endl;
    cout << "- 左键拖拽: 画绿线" << endl;
    cout << "- 左键双击: 画红矩形" << endl;
    cout << "- 右键点击: 清除画布" << endl;
    cout << "- 按ESC退出" << endl;

    while (true) {
        int key = waitKey(1);
        if (key == 27) break; // ESC键退出
    }

    return 0;
}