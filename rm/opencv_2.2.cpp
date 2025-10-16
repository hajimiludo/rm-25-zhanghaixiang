#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

//��2������궨
//ͨ���궨����ȡ�Լ���ʹ�õ�����Ļ������������ڲ�����ȷʹ������ĵ�һ����
//Ҫ����Լ��ʼǱ�����ͷ�������õ�������ͷ���б궨��������궨������
//�������Ǻ�����װ�װ�ʶ����Ŀ�С���ࡰ�ı�Ҫ���裩

//�궨�����
//data:[ 651.3801137452575, 0,					338.5310777861925;
//		 0,				    651.6271630763472,  265.6301631724545;
//		 0,			   	    0,					1				  ]
//data: [-0.1102607696639885, -0.158525968788358, 0.001534575934276915, -0.002870102933147538, 1.184947882717307]

vector<Mat>imgs;//�洢���б궨��ͼ��
string paths;//�궨��ͼ��·��
ifstream fin("rm/rm_test/calibdata.txt");//����洢�궨��ͼ��·����txt�ļ�

//ȷ����ȡ����ͼ��
int readImg()
{
	if (!fin.is_open()) {
		cout << "�����޷��� calibdata.txt �ļ���" << endl;
		return -1;
	}
	int i = 0;
	while (getline(fin, paths))
	{
		i++;
		cout << "ͼ��" << i << "��ȡ����" << endl;
		Mat img = imread(paths);
		if (!img.empty()) {
			imgs.push_back(img);
		}
	}
	if (imgs.empty()) {
		cout << "û�гɹ���ȡ�κ�ͼ��" << endl;
		return -1;
	}
}
int main() {
	//��ȡ����ͼ��
	readImg();

	//�ҵ����б궨����ڽǵ�
	cout << "����Ѱ�����б궨����ڽǵ㣬���Ե�" << endl;
	Size board_size = Size(10, 7); //�궨���ڽǵ���Ŀ
	vector<vector<Point2f>>img_points;//
	for (int i = 0; i < imgs.size(); i++)
	{
		Mat img1 = imgs[i];
		Mat gray1;
		cvtColor(img1, gray1, COLOR_BGR2GRAY);
		vector<Point2f>img1_points;

		findChessboardCorners(gray1, board_size, img1_points);//���㷽���ڽǵ�
		find4QuadCornerSubpix(gray1, img1_points, Size(5, 5)); //ϸ������궨���ǵ�����
		img_points.push_back(img1_points);
	}

	cout << "���ҵ����б궨����ڽǵ�" << endl;
	cout << "��������ÿ���ڽǵ�Ŀռ���ά���꣬���Ե�..." << endl;

	//�������̸�ÿ���ڽǵ�Ŀռ���ά����
	Size squareSize = Size(15, 15);//ÿ���������ʵ�ߴ�
	vector<vector<Point3f>>points;

	for (int i = 0; i < img_points.size(); i++)
	{
		vector<Point3f>temp;
		for (int j = 0; j < board_size.height; j++)
		{
			for (int k = 0; k < board_size.width; k++)
			{
				Point3f realPoint;
				//�ٶ��궨��Ϊ��������ϵ��zƽ�棬��z=0
				realPoint.x = k * squareSize.width;
				realPoint.y = j * squareSize.height;
				realPoint.z = 0;
				temp.push_back(realPoint);
			}
		}
		points.push_back(temp);
	}

	//��ʼ��ÿ��ͼ���еĽǵ��������ٶ����ɿ��������궨
	Size imgsize;
	imgsize.width = imgs[0].cols;
	imgsize.height = imgs[0].rows;

	cout << "������ÿ���ڽǵ�Ŀռ���ά����" << endl << "�������ɻ����ڲξ���ͻ���ϵ��..." << endl;
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));
	vector<Mat>rvecs;//ÿ��ͼƬ����ת����
	vector<Mat>tvecs;//ÿ��ͼƬ��ƽ������
	//���������ڽǵ�ռ���ά�����ͼ���ά�����������ڲξ���ͻ���ϵ������
	calibrateCamera(points, img_points, imgsize, cameraMatrix, distCoeffs, rvecs, tvecs, 0);

	cout << "������ڲξ���" << endl  << cameraMatrix << endl;
	cout << "�������ϵ����" <<distCoeffs << endl;
	waitKey(0);
	return 0;
}
