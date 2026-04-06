#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//题目（3）下面是一张可爱猫猫图片，你将运用鼠标事件知识，要求如下：
//- 用鼠标在图片中框选猫猫，然后保存框选的图片。
//- 鼠标拖动过程中要求显示框的线条以及鼠标当前像素点信息（RGB 值，坐标）
//- 拖动完成后单独显示框取的图像
//- 拖动完成后 cout 输出框中心像素点坐标

// 全局变量定义
Mat imgCat; // 原始猫猫图像
Mat tempImage; // 用于绘制矩形的临时图像
Point startPoint, endPoint; // 鼠标按下点和当前点
bool drawRect = false; // 是否正在拖动

/*
* 鼠标回调函数
* 左键按下，记录起点
* 鼠标移动，绘制矩形和显示RGB值和坐标
* 左键释放，保存矩形区域，显示结果，输出中心像素点坐标
*/
void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
    //鼠标按下时
    case EVENT_LBUTTONDOWN: 
    {
        // 确保点击在图像内
        if (x >= 0 && x < imgCat.cols && y >= 0 && y < imgCat.rows) 
        {
            startPoint = Point(x, y);
            endPoint = startPoint;
            drawRect = true;
            imgCat.copyTo(tempImage); // 复制原始图像到临时层，防止线条残留
        }
        break;
    }

    // 鼠标点击并移动时
    case EVENT_MOUSEMOVE: {
        if (drawRect) 
        {
            endPoint = Point(x, y);
            imgCat.copyTo(tempImage);

            // 从左上到右下绘制矩形
            rectangle(tempImage, startPoint, endPoint, Scalar(200, 0, 255), 2, LINE_AA);

            // 边界检查后再读取像素
            if (x >= 0 && x < imgCat.cols && y >= 0 && y < imgCat.rows)
            {
                // 获取像素值，OpenCV是BGR顺序
                Vec3b pixel = imgCat.at<Vec3b>(y, x);
                //  转换为RBG顺序
                string rgb = "RGB:(" +
                    to_string(pixel[2]) + "," +
                    to_string(pixel[1]) + "," +
                    to_string(pixel[0]) + ")";
                string currentPoint = "Current Point:(" +
                    to_string(x) + "," +
                    to_string(y) + ")";

                //将文本绘制在图像上
                //位置： 右下角
                putText(tempImage, rgb, Point(x -330, y - 50),
                    FONT_HERSHEY_SIMPLEX, 0.8, Scalar(100, 0, 255), 2);
                putText(tempImage, currentPoint, Point(x - 330, y - 20),
                    FONT_HERSHEY_SIMPLEX, 0.8, Scalar(100, 0, 255), 2);
                //位置：左上角（固定）
                //putText(tempImage, rgb, Point(startPoint.x + 10, startPoint.y + 30),
                //    FONT_HERSHEY_SIMPLEX, 0.8, Scalar(200, 0, 255), 2);
                //putText(tempImage, currentPoint, Point(startPoint.x + 10, startPoint.y + 60),
                //    FONT_HERSHEY_SIMPLEX, 0.8, Scalar(200, 0, 255), 2);
            }

            imshow("Cat", tempImage);
        }
        break;
    }

    // 鼠标释放
    case EVENT_LBUTTONUP: {
        drawRect = false;
        endPoint = Point(x, y);

        // 计算实际 ROI
        int x1 = min(startPoint.x, endPoint.x);
        int y1 = min(startPoint.y, endPoint.y);
        int x2 = max(startPoint.x, endPoint.x);
        int y2 = max(startPoint.y, endPoint.y);

        x1 = max(0, x1);
        y1 = max(0, y1);
        x2 = min(imgCat.cols - 1, x2);
        y2 = min(imgCat.rows - 1, y2);

        //
        int width = x2 - x1;
        int height = y2 - y1;
        //
        if (width <= 0 || height <= 0) return;

        Rect roi(x1, y1, width, height);
        Mat selectedRegion = imgCat(roi).clone();

        bool saved = imwrite("./processed_pictures/cat_selected.png", selectedRegion);
        if (saved) {
            cout << "saved successfully" << endl;
        }
        else {
            cout << "failed to saved" << endl;
        }

        // 显示选中区域
        imshow("Selected", selectedRegion);

        //计算并输出中心点
        Point center(
            (x1 + x2) / 2,
            (y1 + y2) / 2
        );
        cout << "center point: (" << center.x << "," << center.y << ")" << endl;

        imshow("Cat", imgCat);
        break;
    }
    }
}


int main(){
    // 加载图像
    imgCat = imread(("./pictures/cat.png"));
    // resize(imgCat, imgCat, Size(800, 600));
    if (imgCat.empty()) {
        cout << "Failed to load image: " << endl;
        return -1;
    }

    // 初始化临时图像
    imgCat.copyTo(tempImage);
    // 创建窗口并绑定鼠标回调
    namedWindow("Cat");
    setMouseCallback("Cat", mouseCallback, NULL);
    // 显示初始图像
    imshow("Cat", imgCat);
    waitKey(0);
    destroyAllWindows();
    return 0;
}