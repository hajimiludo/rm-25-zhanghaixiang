#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

/////////////////////////����Ƶ��������ع�����ȵĴ��ڵĴ���
struct exposure_brightness {
    int exp;
    int bri;
}EB1;//���ڷ����ع�����ȵ�ֵ

int exposure = 100, brightness = 100;
void onTrackbar(int, void*) {
    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
}// �ڻص������и���ȫ�ֽṹ��

exposure_brightness Trac() {
    namedWindow("Trackbar", (640, 50));
    createTrackbar("�ع�", "Trackbar", &exposure, 200, onTrackbar);
    createTrackbar("����", "Trackbar", &brightness, 200, onTrackbar);

    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
    return EB1;
}//����������

int main() {
    VideoCapture cap(0);//����Ƶ
    Mat img;

    Trac();//��������������

    while (true) {
        cap.read(img);//������Ƶ֡
        img.convertTo(img, -1, 1.0 + EB1.exp / 100.0, EB1.bri);//�û������ı�֡�ع������
        imshow("Video", img);
        waitKey(20);
    }
    return 0;
}