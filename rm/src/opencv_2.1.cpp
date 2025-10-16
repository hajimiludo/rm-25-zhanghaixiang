#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
/*��Ŀ*/
//��1����ͳ�ָ��㷨
//����ͼ����һ����ƻ����������Ҫ��дһ�������Զ�����ȡƻ����������
//����ƻ����������������������Ҷ����֦) ��������Ҫ��ʾ������
// ��һ�����ν�ƻ���������


//����ƻ��ͼƬ
Mat Process(Mat img_apple) {
    Mat imghsv;
    cvtColor(img_apple, imghsv, COLOR_BGR2HSV);//ת��Ϊhsv��ɫ�ռ������̬ѧ����

    //ͨ��ʹ����ɫ��⺯����ColorTrackbar.cpp�����ҵ�ƻ����ɫ��HSV��Χ
    //��Ҫ������Χ,HSVɫ����һ��Բ��,��ɫ����λ�������յ�λ��
    Scalar lower_red1(0, 160, 80);    // H(0-10), S(100-255), V(100-255)
    Scalar upper_red1(21, 245, 255);
    Scalar lower_red2(100, 60, 60);  // H(160-180), S(100-255), V(100-255)
    Scalar upper_red2(180, 255, 255);
    Mat mask1, mask2, mask;
    inRange(imghsv, lower_red1, upper_red1, mask1);
    inRange(imghsv, lower_red2, upper_red2, mask2);
    mask = mask1 | mask2;  // �ϲ�������ɫ��Χ,�õ���ֵ����ƻ��ͼ��

    //��̬ѧ����ͼ��
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    //������ȥ��С��������
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    dilate(mask, mask, kernel);
    //�������
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    return mask;
}

//����ƻ�������Լ�����
Mat contours_rect(Mat mask,Mat img_apple){
    Mat img_contours = img_apple.clone();
    //�ҵ������ͼ������
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<double> areas;
    double maxarea = 0;
    //�ҵ������������ƻ�������
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        areas.push_back(area);
        cout << area << endl;
        if (maxarea < area) {
            maxarea = area;
        }
    }
    //���������;���
    for (size_t i = 0; i < areas.size(); i++) {
        if (areas[i] >= maxarea) {
            //����ƻ������
            drawContours(img_contours, contours, i, Scalar(255, 0, 0), 2);
            //����һ������
            Rect rect = boundingRect(contours[i]);
            rectangle(img_contours, rect, Scalar(255, 0, 0), 2);
        }
    }
    //imshow("3", img_contours);

    return img_contours;
}

int main() {
    string path = "rm/rm_test/apple.png";
    Mat img_apple = imread(path);
    
    //��̬ѧ����
    Mat mask = Process(img_apple);

    //�ҵ�����������
    Mat goal = contours_rect(mask, img_apple);
    
    imshow("img_apple", img_apple);
    imshow("mask", mask);
    imshow("goal", goal);
    waitKey(0);

    return 0;
}
