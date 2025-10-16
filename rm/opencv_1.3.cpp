#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//题目（3）下面是一张可爱猫猫图片，你将运用鼠标事件知识，要求如下：
//- 用鼠标在图片中框选猫猫，然后保存框选的图片。
//- 鼠标拖动过程中要求显示框的线条以及鼠标当前像素点信息（RGB 值，坐标）
//- 拖动完成后单独显示框取的图像
//- 拖动完成后 cout 输出框中心像素点坐标

Mat imgcat;
Mat tempimage;
Point Point1;
Point Point2;
bool drawRect = false;

//鼠标事件
//使用说明：按下左键开始绘制，放开结束绘制
void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
    case EVENT_LBUTTONDOWN:
        // 鼠标左键按下，开始绘制
        Point1 = Point(x, y);
        Point2 = Point1;
        drawRect = true;

        // 复制原图到临时图像
        imgcat.copyTo(tempimage);
        break;

        //鼠标移动后绘制下一个矩形并删除上一个矩形,获取并显示RGB 值和坐标
    case EVENT_MOUSEMOVE:
        if (drawRect) {
            Point2 = Point(x, y);
            // 复制原图到临时图像（清除上一个临时矩形）
            imgcat.copyTo(tempimage);

            // 在临时图像上绘制当前矩形
            rectangle(tempimage, Point1, Point2, Scalar(200, 0, 255), 3);

            //获取并显示RGB 值和坐标
            Vec3b pixel = imgcat.at<Vec3b>(y, x);
            //Vec3b表示包含3个unsigned char的向量; .at<Vec3b>(y,x)模板方法，用于访问指定位置的像素（行，列）

            string rgbText = "RGB: (" + to_string(pixel[2]) + ", " + to_string(pixel[1]) + ", " + to_string(pixel[0]) + ")";
            string coordText = "(" + to_string(Point2.x) + ", " + to_string(Point2.y) + ")";
            putText(tempimage, coordText, Point(Point2.x + 10, Point2.y - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200, 0, 255), 2);
            putText(tempimage, rgbText, Point(Point2.x + 20, Point2.y + 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200, 0, 255), 2);

            imshow("Cat", tempimage);
        }
        break;

        //拖动完成后
    case EVENT_LBUTTONUP:
        if (drawRect) {
            drawRect = false;
            Point2 = Point(x, y);
            //cout 输出框中心像素点坐标
            Point center;
            center.x = (Point2.x - Point1.x) / 2;
            center.y = (Point2.y - Point1.y) / 2;

            cout << "框中心坐标:(" << center.x << ", " << center.y << ")" << endl;
        }
        //单独显示框取的图像
        if (Point1 != Point2) {
            Rect roiRect;
            roiRect.x = min(Point1.x, Point2.x);
            roiRect.y = min(Point1.y, Point2.y);
            roiRect.width = abs(Point2.x - Point1.x);
            roiRect.height = abs(Point2.y - Point1.y);

            Mat rect = imgcat(roiRect);
            imwrite("./rm_test/cat_rect.png", rect);
            imshow("Selected Region", rect);
        }
    }
}

int main() {
    String path = ("rm/rm_test/cat.png");
    imgcat = imread(path);

    //检查是否成功导入图片
    if (imgcat.empty()) {
        cout << "导入图片失败： " << path << endl;
        return -1;
    }

    cout << "使用说明: 用鼠标框选猫猫区域，松开后会自动保存并显示框选部分" << endl;

    //鼠标事件，用鼠标绘制矩形
    namedWindow("Cat");
    setMouseCallback("Cat", mouseCallback, NULL);

    imshow("Cat", imgcat);
    waitKey(0);
    return 0;
}