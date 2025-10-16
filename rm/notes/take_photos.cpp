#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void store(Mat img, int photoCount) {
    string filename = "./test/标定" + to_string(photoCount + 1) + ".jpg";
    bool success = imwrite(filename, img);
    if (success) {
        cout << "图像保存成功！" << endl;
    }
    else {
        cout << "图像保存失败！" << endl;
    }
}

int main() {
    VideoCapture cap(0);//导入摄像头
    Mat img;    
    int photoCount = 0;

    cout << "摄像头标定照片拍摄程序" << endl;
    cout << "按 's' 保存照片" << endl;
    cout << "按 'q' 退出" << endl;

    while (true) {
        cap >> img;
        if (img.empty()) break;


        imshow("Video", img);
        char key = waitKey(1);
        if (key == 's') {
            store(img , photoCount);
            photoCount++;
        }
        else if (key == 'q') {
            break;
        }
    }
    cout << "拍摄完成! 共保存 " << photoCount << " 张照片" << endl;

    return 0;
}