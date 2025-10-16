#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//��ȡͼ��--ͼ��Ԥ����--ɸѡ����--ʶ��װ�װ�--������㡢
//װ�װ�ʶ��
//a.ʶ�������װ�װ壨ע�����������֡��϶̵ĵ���Ϊװ�װ����������ĳ�����Ϊ����ʶ��ĳ��ظ��ŵ�������Ҫ�ų���
//b.ʶ�𲢻���װ�װ����ĵ�
//c.ʹ�����λ�˹���PnP�㷨���������װ�װ�ľ��루�ӷ��
//d.ʶ��װ�װ�ROI����ȡ����ȡװ�װ�������֣��ӷ��

//���:
//-����Ч�� : ��Ҫ����ģ���ʱ <= 20ms
//- ������� :
//    - �þ��ο���Ƴ�Ŀ��װ�װ���ԭͼ���е�λ��
//    - ���Ƴ�װ�װ����ĵ�
//    - ���д�����ӳ٣�����Ч�ʣ�
//    - �����װ�װ�ľ��루�ӷ�������Ӧ����ڶ��⣨�궨��

//��ȡͼ��
Mat getImg(string path) {
    Mat img = imread(path);
    if (img.empty()) {
        cout << "��ȡͼ��ʧ��" << endl;
    }
    resize(img, img, Size(), 0.5, 0.5, INTER_AREA);
    return img;
}

//ת��ͼ��ͨ����ͼ���˲�����ֵ��,��̬ѧ����
Mat ProcessImg(const Mat& img) {
    Mat imghsv, mask;
    cvtColor(img, imghsv, COLOR_BGR2HSV);//ת��Ϊhsv��ɫ�ռ������̬ѧ����
    
    //װ�װ�����0,255,0,255,200,255
    Scalar lower(0, 0, 220);
    Scalar upper(255, 255, 255);
    inRange(imghsv,lower, upper, mask);//�õ�װ����ɫ����������

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(mask, mask, kernel);

    imshow("mask", mask);
    return mask;
}

//��Ե��⣬����������ɸѡȥ�����־���,�������о�����������ж�
struct goalPoints {
    Point2f p[4];//���������ĸ�����
    Point2f cpt;//�������ε����ĵ�
    bool datas = false;//�Ƿ������������
};
vector<goalPoints>gp;

vector<goalPoints> Detect(const Mat& mask, const Mat& img) {
    //��������
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<double> areas;
    int m = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        areas.push_back(area);
        cout << "area:" << area << endl;
        //�����������ʶ�����
        if (area >= 80 && area <= 400) {
            RotatedRect rect1 = minAreaRect(contours[i]);//���������С����,����ĸ�����,��������.center����ת�Ƕ�
            //��¼ÿ���������ε��Ķ�������ĵ�����
            goalPoints gp1;
            gp1.cpt = rect1.center;

            //string num = to_string(++m);
            rect1.points(gp1.p);
            //putText(img, num, gp1.cpt, FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,200, 255), 2);
            gp1.datas = true;
            gp.push_back(gp1);
        }
    }
    int num1 = gp.size();
    //���ն����ʶ�𵽵ĵ����������еĺͶ���
    //for (int i = 0; i < num1; i++) {
    //    if (!gp.empty() && gp[i].datas == true) {
    //        cout << i + 1 << ":" << endl << "center point:" << gp[i].cpt << endl;
    //        for (int j = 0; j < 4; j++) cout << "p:" << gp[i].p[j] << endl;
    //    }
    //    else cout << "δ�ɹ�ʶ�𵽵���" << endl;
    //}
    return gp;
}

//��ͼ�����װ�װ�������Ի������ĵ�����
void draw(const Mat& img) {
    int num1 = gp.size();
    //sort�����������ĵ�x����ֵ��С�����������
    sort(gp.begin(), gp.end(), [](const goalPoints& a, const goalPoints& b) {
        return a.cpt.x < b.cpt.x;
        });

    //�������ĵ�
    for (int i = 0; i < num1 - 1; i = i + 2) {
        Point cpt1 = (gp[i].cpt + gp[i + 1].cpt) / 2;//������е�
        circle(img, cpt1, 3, Scalar(0, 200, 255), -1);
    }

    /////////////////�����鿴����������������gp.p[]�еı��
    //for (int i = 0; i < 4; i++) {
    //    string num1 = to_string(i);
    //    for (int j = 0; j < 4; j++) {
    //        string num = to_string(j);
    //        putText(img, num, gp[i].p[j], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 200, 255), 1);
    //   }
    //    putText(img, num1, gp[i].cpt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 200, 255), 1);
    //}
    
    //���ƾ���
    for (int i = 0; i < num1 - 1; i += 2) {
        // ����װ�װ��ı��Σ��������ҵ��������㣩
        vector<Point> armor_points;
        float y0 = gp[i].p[0].y;//������0�Ŷ����yֵ
        float y2 = gp[i + 1].p[2].y;//������2�Ŷ����yֵ

        //����y0 > y2����0�Ŷ��������Ͻǣ�2�Ŷ��������½ǣ�1�Ŷ��������Ͻǣ�3�������½�
        if (y0 > y2)
        {
            armor_points.push_back(gp[i].p[1]);     // ��������Ͻ�
            armor_points.push_back(gp[i + 1].p[2]);   // �ҵ������Ͻ�  
            armor_points.push_back(gp[i + 1].p[3]);   // �ҵ������½�
            armor_points.push_back(gp[i].p[0]);     // ��������½�
        }
        //����y0 > y2����0�Ŷ��������½ǣ�2�Ŷ��������Ͻǣ�1�Ŷ��������Ͻǣ�3�������½�
        else if(y0 < y2)
        {
            armor_points.push_back(gp[i].p[0]);     // ��������Ͻ�
            armor_points.push_back(gp[i + 1].p[1]);   // �ҵ������Ͻ�  
            armor_points.push_back(gp[i + 1].p[2]);   // �ҵ������½�
            armor_points.push_back(gp[i].p[3]);     // ��������½�
        }
        // ����װ�װ�߽�
        polylines(img, armor_points, true, Scalar(0, 255, 0), 2);
    }
}



//pnp����������װ�װ����
float distance(const Mat& img) {
    //��Ŀλ�˹��ƣ�solvePnP(),solvePnPRansac()������ת������ƽ������
    //��ת��������ת����֮���໥ת��Rodrigues()

    //װ�װ����ʵ��͸�
    float width = 0.13f;////////////������
    float height = 0.055f;//////////������
    // װ�װ��3D�������꣨��װ�װ�����Ϊԭ�㣩
    vector<Point3f> objPoints;
    objPoints.push_back(Point3f(-width / 2, -height / 2, 0)); // ���Ͻ�
    objPoints.push_back(Point3f(width / 2, -height / 2, 0));  // ���Ͻ�
    objPoints.push_back(Point3f(width / 2, height / 2, 0));   // ���½�
    objPoints.push_back(Point3f(-width / 2, height / 2, 0));  // ���½�

    Mat cameraMatrix = (Mat_<float>(3, 3) << 651.380, 0, 338.531,
        0, 651.627, 265.630,
        0, 0, 1);
    Mat distCoeffos = (Mat_<float>(1, 5) << -0.1102607696639885, -0.158525968788358,
        0.001534575934276915, -0.002870102933147538, 1.184947882717307);
    Mat rvec;//��ת����
    Mat tvec;//ƽ������

    for (int i = 0; i < gp.size() - 1; i += 2) {
        goalPoints& leftLight = gp[i];
        goalPoints& rightLight = gp[i + 1];

        // ����װ�װ���ĸ�ͼ��ǵ�
        // ʹ�������������ʵ��㹹��װ�װ���ĸ��ǵ�
        vector<Point2f> imgPoints;
        // ���Ͻǣ�����������ϵ�
        imgPoints.push_back(leftLight.p[0]);
        // ���Ͻǣ��ҵ��������ϵ�
        imgPoints.push_back(rightLight.p[1]);
        // ���½ǣ��ҵ��������µ�
        imgPoints.push_back(rightLight.p[2]);
        // ���½ǣ�����������µ�
        imgPoints.push_back(leftLight.p[3]);

        bool success = solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffos, rvec, tvec);
        if (success) {
            // ������루ƽ��������ģ��
            double distance = norm(tvec);
            // ���������Ϣ
            cout << "Distance:" << distance << endl;

            // ��ͼ������ʾ����
            Point2f cpt2 = (leftLight.cpt + rightLight.cpt) / 2;
            string distText = "Distance: " + to_string(distance);
            putText(img, distText, cpt2 + Point2f(-150, -400), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);

            ////�����ת��������ת����
            //cout << "��������ϵ�任���������ϵ����ת����" << rvec << endl;
            //Mat R;
            //Rodrigues(rvec, R);
            //cout << "��ת����" << R << endl;

            return distance;
        }
    }
}



int main()
{   
    // ������ʱ��
    TickMeter tm;
    //����һ��ȡװ�װ�ͼ��
    string path = "./rm/rm_test/��1.jpg";
    Mat img = getImg(path);//����ͼ��·����ȡͼ�񣬲����л�ȡ������ͳ�������

    //�������̬ѧ����
    Mat mask = ProcessImg(img);

    tm.start();
    //�������ҵ������ƾ���
    Detect(mask, img);
    draw(img);
    
    //�����ļ������
    distance(img);
    tm.stop();
    cout << "��Ҫ����ģ���ʱ: " << tm.getTimeMilli() << " ms" << endl;

    imshow("img", img);
    waitKey(0);
    return 0;
}