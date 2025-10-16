#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//��Ŀ��3��������һ�ſɰ�èèͼƬ���㽫��������¼�֪ʶ��Ҫ�����£�
//- �������ͼƬ�п�ѡèè��Ȼ�󱣴��ѡ��ͼƬ��
//- ����϶�������Ҫ����ʾ��������Լ���굱ǰ���ص���Ϣ��RGB ֵ�����꣩
//- �϶���ɺ󵥶���ʾ��ȡ��ͼ��
//- �϶���ɺ� cout ������������ص�����

Mat imgcat;
Mat tempimage;
Point Point1;
Point Point2;
bool drawRect = false;

//����¼�
//ʹ��˵�������������ʼ���ƣ��ſ���������
void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
    case EVENT_LBUTTONDOWN:
        // ���������£���ʼ����
        Point1 = Point(x, y);
        Point2 = Point1;
        drawRect = true;

        // ����ԭͼ����ʱͼ��
        imgcat.copyTo(tempimage);
        break;

        //����ƶ��������һ�����β�ɾ����һ������,��ȡ����ʾRGB ֵ������
    case EVENT_MOUSEMOVE:
        if (drawRect) {
            Point2 = Point(x, y);
            // ����ԭͼ����ʱͼ�������һ����ʱ���Σ�
            imgcat.copyTo(tempimage);

            // ����ʱͼ���ϻ��Ƶ�ǰ����
            rectangle(tempimage, Point1, Point2, Scalar(200, 0, 255), 3);

            //��ȡ����ʾRGB ֵ������
            Vec3b pixel = imgcat.at<Vec3b>(y, x);
            //Vec3b��ʾ����3��unsigned char������; .at<Vec3b>(y,x)ģ�巽�������ڷ���ָ��λ�õ����أ��У��У�

            string rgbText = "RGB: (" + to_string(pixel[2]) + ", " + to_string(pixel[1]) + ", " + to_string(pixel[0]) + ")";
            string coordText = "(" + to_string(Point2.x) + ", " + to_string(Point2.y) + ")";
            putText(tempimage, coordText, Point(Point2.x + 10, Point2.y - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200, 0, 255), 2);
            putText(tempimage, rgbText, Point(Point2.x + 20, Point2.y + 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200, 0, 255), 2);

            imshow("Cat", tempimage);
        }
        break;

        //�϶���ɺ�
    case EVENT_LBUTTONUP:
        if (drawRect) {
            drawRect = false;
            Point2 = Point(x, y);
            //cout ������������ص�����
            Point center;
            center.x = (Point2.x - Point1.x) / 2;
            center.y = (Point2.y - Point1.y) / 2;

            cout << "����������:(" << center.x << ", " << center.y << ")" << endl;
        }
        //������ʾ��ȡ��ͼ��
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

    //����Ƿ�ɹ�����ͼƬ
    if (imgcat.empty()) {
        cout << "����ͼƬʧ�ܣ� " << path << endl;
        return -1;
    }

    cout << "ʹ��˵��: ������ѡèè�����ɿ�����Զ����沢��ʾ��ѡ����" << endl;

    //����¼����������ƾ���
    namedWindow("Cat");
    setMouseCallback("Cat", mouseCallback, NULL);

    imshow("Cat", imgcat);
    waitKey(0);
    return 0;
}