#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void store(Mat img, int photoCount) {
    string filename = "./test/�궨" + to_string(photoCount + 1) + ".jpg";
    bool success = imwrite(filename, img);
    if (success) {
        cout << "ͼ�񱣴�ɹ���" << endl;
    }
    else {
        cout << "ͼ�񱣴�ʧ�ܣ�" << endl;
    }
}

int main() {
    VideoCapture cap(0);//��������ͷ
    Mat img;    
    int photoCount = 0;

    cout << "����ͷ�궨��Ƭ�������" << endl;
    cout << "�� 's' ������Ƭ" << endl;
    cout << "�� 'q' �˳�" << endl;

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
    cout << "�������! ������ " << photoCount << " ����Ƭ" << endl;

    return 0;
}