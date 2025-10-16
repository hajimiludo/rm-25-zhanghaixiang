#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

//��Ŀ��2��
//������Ҫ��ͨ�����ô���򿪵�������ͷ���������õ�������ͷ����¼��һ����Ƶ��Ҫ�����£�
//- ������ͷ�����ô�����ʾ
//- ���������������ͷ��Ϣ��Ҫ������Ƶͼ���С��֡�ʣ�FPS��
//- ʹ�û�������������ͷ�ع�ʱ�䡢���ȣ�Ҫ���иߵ��ع⡢�������ȵ�չʾ����ʾ��createTrackbar�����ֵ�������ͷ�����޷������ع�ʱ�䣬�����������ع���һ�
//- ¼������Ƶ�󱣴��ڱ���

struct exposure_brightness {
    int exp;
    int bri;
}EB1;//���ڷ����ع�����ȵ�ֵ

int exposure = 100, brightness = 100;
void onTrackbar(int, void*) {
    EB1.exp = exposure - 100;
    EB1.bri = brightness - 100;
}// �ڻص������и���ȫ�ֽṹ��

void creatTrac() {
    namedWindow("Trackbar", WINDOW_NORMAL);
    resizeWindow("Trackbar", 640, 50);
    createTrackbar("�ع�", "Trackbar", &exposure, 200, onTrackbar);
    createTrackbar("����", "Trackbar", &brightness, 200, onTrackbar);
}//����������

int main() {
    // ������ͷ
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "�޷�������ͷ" << endl;
        return -1;
    }
    Mat img;

    creatTrac();//����������

    double fps = cap.get(CAP_PROP_FPS);
    if (fps <= 0) fps = 30; // �����ȡʧ�ܣ�ʹ��Ĭ��ֵ
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    Size imgSize(width, height);

    //������Ƶ
    VideoWriter writer("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imgSize);
    //ʹ�û�������������ͷ�ع�ʱ�䡢���ȣ�
    
//��ʾ��Ƶ֡
    while (true) {
        cap.read(img);
        if (img.empty()) break;
        writer.write(img);//������Ƶ

        // �ڶ�ȡ��֡�ϻ����ı�
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