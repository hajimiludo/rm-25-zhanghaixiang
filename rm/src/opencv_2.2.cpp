#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

//（2）相机标定
//通过标定来获取自己所使用的相机的畸变参数和相机内参是正确使用相机的第一步，
//要求对自己笔记本摄像头或其他用到的摄像头进行标定，并保存标定参数。
//（本题是后续“装甲板识别“题目中”测距“的必要步骤）

//标定结果：
//data:[ 651.3801137452575, 0,					338.5310777861925;
//		 0,				    651.6271630763472,  265.6301631724545;
//		 0,			   	    0,					1				  ]
//data: [-0.1102607696639885, -0.158525968788358, 0.001534575934276915, -0.002870102933147538, 1.184947882717307]

vector<Mat>imgs;//存储所有标定板图像
string paths;//标定板图像路径
ifstream fin("rm/rm_test/calibdata.txt");//输入存储标定板图像路径的txt文件

//确保读取所有图像
int readImg()
{
	if (!fin.is_open()) {
		cout << "错误：无法打开 calibdata.txt 文件！" << endl;
		return -1;
	}
	int i = 0;
	while (getline(fin, paths))
	{
		i++;
		cout << "图像" << i << "读取正常" << endl;
		Mat img = imread(paths);
		if (!img.empty()) {
			imgs.push_back(img);
		}
	}
	if (imgs.empty()) {
		cout << "没有成功读取任何图像！" << endl;
		return -1;
	}
}
int main() {
	//读取所有图像
	readImg();

	//找到所有标定板的内角点
	cout << "正在寻找所有标定板的内角点，请稍等" << endl;
	Size board_size = Size(10, 7); //标定板内角点数目
	vector<vector<Point2f>>img_points;//
	for (int i = 0; i < imgs.size(); i++)
	{
		Mat img1 = imgs[i];
		Mat gray1;
		cvtColor(img1, gray1, COLOR_BGR2GRAY);
		vector<Point2f>img1_points;

		findChessboardCorners(gray1, board_size, img1_points);//计算方格内角点
		find4QuadCornerSubpix(gray1, img1_points, Size(5, 5)); //细化方格标定极角点坐标
		img_points.push_back(img1_points);
	}

	cout << "已找到所有标定板的内角点" << endl;
	cout << "正在生成每个内角点的空间三维坐标，请稍等..." << endl;

	//生成棋盘格每个内角点的空间三维坐标
	Size squareSize = Size(15, 15);//每个方格的真实尺寸
	vector<vector<Point3f>>points;

	for (int i = 0; i < img_points.size(); i++)
	{
		vector<Point3f>temp;
		for (int j = 0; j < board_size.height; j++)
		{
			for (int k = 0; k < board_size.width; k++)
			{
				Point3f realPoint;
				//假定标定板为世界坐标系的z平面，即z=0
				realPoint.x = k * squareSize.width;
				realPoint.y = j * squareSize.height;
				realPoint.z = 0;
				temp.push_back(realPoint);
			}
		}
		points.push_back(temp);
	}

	//初始化每幅图像中的角点数量，假定都可看到完整标定
	Size imgsize;
	imgsize.width = imgs[0].cols;
	imgsize.height = imgs[0].rows;

	cout << "已生成每个内角点的空间三维坐标" << endl << "正在生成机的内参矩阵和畸变系数..." << endl;
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat distCoeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));
	vector<Mat>rvecs;//每张图片的旋转向量
	vector<Mat>tvecs;//每张图片的平移向量
	//根据棋盘内角点空间三维坐标和图像二维坐标计算相机内参矩阵和畸变系数矩阵
	calibrateCamera(points, img_points, imgsize, cameraMatrix, distCoeffs, rvecs, tvecs, 0);

	cout << "相机的内参矩阵：" << endl  << cameraMatrix << endl;
	cout << "相机畸变系数：" <<distCoeffs << endl;
	waitKey(0);
	return 0;
}
