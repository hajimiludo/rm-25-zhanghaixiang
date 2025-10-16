#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

//float w, h;
//Mat matrix, imgWarp;


//���Ʊ�Ե
void getcontours(Mat imgDil, Mat& mask) {
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(mask, contours, -1, Scalar(150, 0, 0), 2);
    //����ж�
     //vector<vector<Point>>conPoly(contours.size());
     //for (int i = 0; i < contours.size(); i++) {
     //    int area = contourArea(contours[i]);
     //    cout << area << endl;
     //    if (area > 12000) {
     //        float peri = arcLength(contours[i], true);
     //        approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
     //        drawContours(mask, contours, i, Scalar(235, 206, 135), 4);
     //    }
     //}
    imshow("Image contours", mask);

    waitKey(0);
}

//��̬ѧ����
void Process() {
//��ֵ��
cvtColor(img, imgGray,COLOR_BGR2GRAY);
//ģ��
GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
//��Ե���
Canny(imgBlur, imgCanny, 25, 75);
//���ͺ͸�ʴ(���Ӻ�Ⱥͼ��ٺ��
Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
dilate(imgCanny,imgDil,kernel);
erode(imgDil, imgErode, kernel);
//������ȥ��С��������
morphologyEx(mask, mask, MORPH_OPEN, kernel);
//�������
morphologyEx(mask, mask, MORPH_CLOSE, kernel);
}

void imgsize() {
    //������С
    //cout << img.size() << endl;
    resize(img, imgResize, Size(),0.5,0.5);
    //�ü�
    Rect roi(100, 100, 300, 250);
    imgCrop = img(roi);
}

void createimg_rect_line_text() {
    Mat img1(512, 512, CV_8UC3, Scalar(255, 255, 255));
    circle(img1, Point(256, 256), 255, Scalar(0, 69, 255),-1);//draw a circle
    rectangle(img1,Point(130, 226), Point(382, 286), Scalar(255, 255, 255), 7);//draw a rectangle
    line(img1, Point(130, 295), Point(321, 133), Scalar(255, 255, 0), 3);
    putText(img, );
}

void PUTTEXT() {
    //��ͼ������ʾ�ı�
    // �����ı�����
    std::string text = "Hello OpenCV!";
    // �����ı�λ�ã����½����꣩
    cv::Point textOrg(50, 150);
    // ������������
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    // ���������С��������
    double fontScale = 1.0;
    // �����ı���ɫ��BGR��ʽ����ɫ��
    cv::Scalar color(255, 0, 0);
    // �����ı��߿�
    int thickness = 1;
    // ʹ��putText������ͼ���ϻ����ı�
    cv::putText(image, text, textOrg, fontFace, fontScale, color, thickness);
    // ��ʾͼ��
    cv::imshow("Basic PutText Example", image);
    // �ȴ�����
    cv::waitKey(0);
}

//����¼��ص�����
void mcb(int event, int x, int y, int flags, void* userdata) {
    Mat* image = static_cast<Mat*>(userdata);
}

void mousetype() {
    EVENT_MOUSEMOVE = 0, // ����ƶ�
    EVENT_LBUTTONDOWN = 1, // �������
    EVENT_RBUTTONDOWN = 2, // �Ҽ�����
    EVENT_MBUTTONDOWN = 3, // �м�����
    EVENT_LBUTTONUP = 4, // ����ͷ�
    EVENT_RBUTTONUP = 5, // �Ҽ��ͷ�
    EVENT_MBUTTONUP = 6, // �м��ͷ�
    EVENT_LBUTTONDBLCLK = 7, // ���˫��
    EVENT_RBUTTONDBLCLK = 8, // �Ҽ�˫��
    EVENT_MBUTTONDBLCLK = 9, // �м�˫��
    EVENT_MOUSEWHEEL = 10, // ������
    EVENT_MOUSEHWHEEL = 11  // ���ˮƽ����
    EVENT_FLAG_LBUTTON = 1,  // �������
    EVENT_FLAG_RBUTTON = 2,  // �Ҽ�����
    EVENT_FLAG_MBUTTON = 4,  // �м�����
    EVENT_FLAG_CTRLKEY = 8,  // Ctrl������
    EVENT_FLAG_SHIFTKEY = 16, // Shift������
    EVENT_FLAG_ALTKEY = 32  // Alt������
}

int main() {

    //����ͼƬ
    string path = "rm/rm_test/0b136e342cc8ff725a47d9d81fd97e0.png";
    Mat img = imread(path);
    if (img.empty()) {
        cout << "��ȡͼ��ʧ��" << endl;
        return -1;
    }
    // ������ͷ
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "�޷�������ͷ" << endl;
        return -1;
    }
    //��ȡ֡�ʺ�ͼƬ�ߴ�
    double fps = cap.get(CAP_PROP_FPS);//fps
    Size imgSize(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));

    VideoWriter writer("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, imgSize);
    //cout << "FPS:" << fps << endl;
    //cout << "SIZE" << imgSize << endl;
    //��ʾ��Ƶ֡
    while (true) {
        cap.read(img);


        imshow("Video", img);
        waitKey(20);
    }

    //��ĿͶӰ��projectPoints()����3D-->2D
    //��Ŀλ�˹��ƣ�solvePnP()������ת������ƽ������

    ////Ť��ͼ��
    //Point2f src[4]={}
    //Point2f dst[4] = {}

    //matrix = getPerspectiveTransform(src, dst);//͸�ӱ任
    //warpPerspective(img, imgWarp, matrix, Point(w, h));

    return 0;
}