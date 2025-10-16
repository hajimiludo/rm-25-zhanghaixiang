#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat image;
Point previousPoint;
bool drawing = false;

// ���ص�����
void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
        // 1. ����ƶ�
    case EVENT_MOUSEMOVE:
        if (drawing) {
            // ���ƶ�ʱ��������
            line(image, previousPoint, Point(x, y), Scalar(0, 255, 0), 2);
            previousPoint = Point(x, y);
            imshow("Mouse Events", image);
        }
        break;

        // 2. �������
    case EVENT_LBUTTONDOWN:
        cout << "������� - ����: (" << x << ", " << y << ")" << endl;
        previousPoint = Point(x, y);
        drawing = true;
        // �ڵ��λ�û�Բ
        circle(image, Point(x, y), 5, Scalar(255, 0, 0), -1);
        imshow("Mouse Events", image);
        break;

        // 3. ����ͷ�
    case EVENT_LBUTTONUP:
        cout << "����ͷ� - ����: (" << x << ", " << y << ")" << endl;
        drawing = false;
        break;

        // 4. �Ҽ�����
    case EVENT_RBUTTONDOWN:
        cout << "�Ҽ����� - ����: (" << x << ", " << y << ")" << endl;
        // �Ҽ����ͼ��
        image = Mat::zeros(500, 500, CV_8UC3);
        putText(image, "Click and drag to draw", Point(50, 50),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
        imshow("Mouse Events", image);
        break;

        // 5. ���˫��
    case EVENT_LBUTTONDBLCLK:
        cout << "���˫�� - ����: (" << x << ", " << y << ")" << endl;
        // ˫��λ�û�����
        rectangle(image, Point(x - 15, y - 15), Point(x + 15, y + 15),
            Scalar(0, 0, 255), 2);
        imshow("Mouse Events", image);
        break;

        // 6. �Ҽ�˫��
    case EVENT_RBUTTONDBLCLK:
        cout << "�Ҽ�˫�� - ����: (" << x << ", " << y << ")" << endl;
        break;

        // 7. �м�����
    case EVENT_MBUTTONDOWN:
        cout << "�м����� - ����: (" << x << ", " << y << ")" << endl;
        break;
    }

    // �����ϼ�״̬
    if (flags & EVENT_FLAG_CTRLKEY) {
        cout << "Ctrl��������" << endl;
    }
    if (flags & EVENT_FLAG_SHIFTKEY) {
        cout << "Shift��������" << endl;
    }
    if (flags & EVENT_FLAG_LBUTTON) {
        // �����ǰ������
    }
}

int main() {
    // �����հ�ͼ��
    image = Mat::zeros(500, 500, CV_8UC3);
    putText(image, "Click and drag to draw", Point(50, 50),
        FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);

    // �������ڲ��������ص�
    namedWindow("Mouse Events");
    setMouseCallback("Mouse Events", mouseCallback, NULL);

    imshow("Mouse Events", image);

    cout << "ʹ��˵��:" << endl;
    cout << "- ������: ������" << endl;
    cout << "- �����ק: ������" << endl;
    cout << "- ���˫��: �������" << endl;
    cout << "- �Ҽ����: �������" << endl;
    cout << "- ��ESC�˳�" << endl;

    while (true) {
        int key = waitKey(1);
        if (key == 27) break; // ESC���˳�
    }

    return 0;
}