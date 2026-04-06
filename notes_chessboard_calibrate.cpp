#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

void draw(string& path)
{
	Mat img = imread(path);
	//if (!(img.data))
	//{
	//	cout << "failed to read the pictures" << endl;
	//}

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	// 定义数目
	Size board_size = Size(10, 7);
	// 检测角点
	vector<Point2f> img_points;
	// 计算方格标定板角点
	findChessboardCorners(gray, board_size, img_points);
	// 细化方格标定板角点
	find4QuadCornerSubpix(gray, img_points, Size(5, 5));
	// 绘制角点检测结果
	drawChessboardCorners(img, board_size, img_points, true);

	imshow("result", img);
	waitKey(0);
}

int main()
{
	//
	vector<Mat> imgs;
	string imageName;
	ifstream fin("calibfata.txt");
	while (getline(fin, imageName))
	{
		Mat img = imread(imageName);
		imgs.push_back(img);
	}

	Size board_size = Size(10, 7);
	vector<vector<Point2f>> imgsPoints;
	for (int i = 0; i < imgs.size(); i++)
	{
		Mat img1 = imgs[i];
		Mat gray1;
		cvtColor(img1, gray1, COLOR_BGR2GRAY);
		vector<Point2f> img1_points;
		// 计算方格标定板角点
		findChessboardCorners(gray1, board_size, img1_points);
		// 细化方格标定板角点
		find4QuadCornerSubpix(gray1, img1_points, Size(5, 5));
		imgsPoints.push_back(img1_points);
	}

	//生成棋盘格每个内角点的空间三维坐标
	Size squareSize = Size(10, 10); // 每个方格的真实尺寸6.8

	vector<vector<Point3f>> objectPoints;
	for (int i = 0; i < imgsPoints.size(); i++)
	{
		vector<Point3f> tempPointSet;
		for (int j = 0; j < board_size.height; j++)
		{
			for (int k = 0; k < board_size.width; k++)
			{
				Point3f realPoint;
				realPoint.x = j * squareSize.width;
				realPoint.y = k * squareSize.height;
				realPoint.z = 0;
				tempPointSet.push_back(realPoint);
			}
		}
		objectPoints.push_back(tempPointSet);
	}

	// 初始化图像中的角点数量
	vector<int> point_number;
	for (int i = 0; i < imgsPoints.size(); i++)
	{
		point_number.push_back(board_size.width * board_size.height);
	}

	//图像尺寸
	Size imageSize;
	imageSize.width = imgs[0].cols;
	imageSize.height = imgs[0].rows;

	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));

	//相机的5个畸变系数：k1,k2,p1,p2,k3
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));
	vector<Mat> rvecs;
	vector<Mat> tvecs;
	calibrateCamera(objectPoints, imgsPoints, imageSize, 
		cameraMatrix, distCoeffs,rvecs, tvecs, 0);
	cout << "相机的内参矩阵=" << endl << cameraMatrix << endl;
	cout << "相机畸变系数" << distCoeffs << endl;
	waitKey(0);
	return 0;
}