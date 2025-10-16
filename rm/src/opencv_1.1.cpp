#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

//-ɫ�ʷָ��ͼƬ�к�ɫͨ��ɫ�ʷָ�ķ�����ֵ���ɺڰ�ͼ��Ŀ����ɫΪ�ף����౳��Ϊ�ڣ���
Mat imgBW(const Mat& img) {
    Mat imgHSV, mask, mask0;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    //red:140,179,215,255,231,255
    int hmin = 140, smin = 215, vmin = 231;
    int hmax = 179, smax = 255, vmax = 255;

    Scalar lower(hmin, smin, vmin);
    Scalar upper(hmax, smax, vmax);
    inRange(imgHSV, lower, upper, mask0);

    //�����㣬����ȥ��С���������
    Mat kernel = getStructuringElement(MORPH_RECT, Size(7, 7));
    morphologyEx(mask0, mask, MORPH_CLOSE, kernel);

    return mask;
}

//- ��ɫ�ʷָ���ͼ���б�Ե��ȡ����������Ե��ǳ��ɫ��������Ϊ��ɫ�� ����ʾ�� Canny �㷨��
Mat getcontours(const Mat& mask) {
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;
    Mat imgBlur, imgCanny;

    //ģ��
    GaussianBlur(mask, imgBlur, Size(3, 3), 3, 0);
    //��Ե���
    Canny(imgBlur, imgCanny, 25, 75);
    imshow("Image canny", imgCanny);

    //������Ե
    findContours(imgCanny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat imgB(mask.size(), CV_8UC3, Scalar(0, 0, 0));
    drawContours(imgB, contours, -1, Scalar(235, 206, 135), 4);

    imshow("Imagecontour", imgB);

    return imgB;
}

//- �ô�������ԭͼ���ڰ�ͼ�񡢱�Ե��ȡͼ��ͼ���СΪ 1280 * 720 ��

//- ����Եͼ�񱣴��ڱ��ء�
void store(const Mat& img) {
    bool success = imwrite("./rm/rm_test/imgEdge.png", img);

    if (success) {
        cout << "ͼ�񱣴�ɹ���" << endl;
    }
    else {
        cout << "ͼ�񱣴�ʧ�ܣ�" << endl;
    }
}

int main() {
    //��ȡͼƬ�����޸Ĵ�С
    string path = "rm/rm_test/0b136e342cc8ff725a47d9d81fd97e0.png";
    Mat imgori = imread(path);
    Mat img;
    resize(imgori, img, Size(1280, 720));

    //��ͼƬ�к�ɫͨ��ɫ�ʷָ�ķ�����ֵ���ɺڰ�ͼ��
    Mat imgbw = imgBW(img);
    Mat imgEdge = getcontours(imgbw);
    //����ͼƬ
    //store(imgEdge);

    imshow("Image", img);
    imshow("Image_Black_White", imgbw);
    waitKey(0);


    return 0;
}