#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/////（2）相机标定
//通过标定来获取自己所使用的相机的畸变参数和相机内参
// 对摄像头进行标定，并保存标定参数。
// 本题是后续“装甲板识别“题目中”测距“的必要步骤）


/// <summary>
/// 相机标定类，封装了读取路径，角点检测，标定，参数保存
/// </summary>
class CameraCalibrator
{
public:
	//标定板参数
	Size boardSize;    //内角点数量(10,7)
	Size squareSize;   //每个方格的实际尺寸6.9
	float aspectRatio; //期望的纵横比

	CameraCalibrator()
		: boardSize(Size(10, 7)),
		squareSize(Size(15, 15)),
		aspectRatio(1.0f) { }


	/// <summary>
	/// 读取图像路径
	/// </summary>
	/// <param name="filename">calibdata.txt文件，包含标定板图像路径</param>
	/// <param name="imageList">输出的图像容器</param>
	/// <returns>读取结果：是否成功读取</returns>
	bool readImageList(const string& filename, vector<Mat>& imageList)
	{
		ifstream fin(filename.c_str());
		if (!fin.is_open())
		{
			cerr << "failed to open file:" << filename << endl;
			return false;
		}

		string path;
		while (getline(fin, path))
		{
			if (path.empty()) continue;

			Mat img = imread(path);
			if (img.empty())
			{
				cerr << "skip wrong image:" << path << endl;
				continue;
			}
			imageList.push_back(img);
			cout << "load image successfully" << path << endl;
		}
		fin.close();
		return !imageList.empty();
	}


	/// <summary>
	/// 执行相机标定的主要流程：
	/// </summary>
	/// <param name="calibrationImages">标定所用图像列表</param>
	/// <param name="cameraMatrix">输出的相机内参矩阵</param>
	/// <param name="distCoeffs">输出的畸变系数</param>
	/// <returns>平均重投影误差，若rms>1.0，说明标定结果很差</returns>
	double calibrate(const vector<Mat>& calibrationImages,
		Mat& cameraMatrix, Mat& distCoeffs)
	{
		//储存标定板的世界坐标系坐标
		vector < vector<Point3f>> objectPoints;
		vector<Point3f> tempObjectPoints;

		//以标定板平面为z=0平面,生成坐标
		for (int j = 0; j < boardSize.height; j++)
		{
			for (int k = 0; k < boardSize.width; k++)
			{
				Point3f realPoint;
				realPoint.x = k * squareSize.width;
				realPoint.y = j * squareSize.height;
				realPoint.z = 0;
				tempObjectPoints.push_back(realPoint);
			}
		}

		//存储检测到的2d图像坐标
		vector<vector<Point2f>> imagePoints;
		vector<Mat> validImages; //保存成功检测的图像

		for (size_t i = 0; i < calibrationImages.size(); i++)
		{
			Mat img = calibrationImages[i];
			Mat gray;
			cvtColor(img, gray, COLOR_BGR2GRAY);

			vector<Point2f> imgPoints;

			// 计算方格标定板角点
			findChessboardCorners(gray, boardSize, imgPoints);
			// 细化方格标定板角点
			find4QuadCornerSubpix(gray, imgPoints, Size(5, 5));
			imagePoints.push_back(imgPoints);
			objectPoints.push_back(tempObjectPoints);
			validImages.push_back(img.clone());// 用于可视化

			// 在图中画出角点，显示图像
			//drawChessboardCorners(img, boardSize, imgPoints, true);
			//imshow("calibration", img);
			//waitKey(0);
		}

		// 标定计算
		int flags = 0;    // flags |= CALIB_ZERO_TANGENT_DIST(切向畸变为0)
		Mat rvecs, tvecs; //旋转向量和平移向量

		//rms重投影误差，rms>1.0，说明标定结果很差
		double rms = calibrateCamera(
			objectPoints, imagePoints,
			calibrationImages[0].size(),
			cameraMatrix, distCoeffs, rvecs, tvecs, flags);

		cout << "平均重投影误差:" << rms << "pixels" << endl;
		cout << "相机内参矩阵:" << endl << cameraMatrix << endl;
		cout << "畸变系数:" << endl << distCoeffs << endl;

		return rms;
	}


	/// <summary>
	/// 保存标定结果
	/// </summary>
	/// <param name="filename">文件名</param>
	/// <param name="cameraMatrix">相机内参</param>
	/// <param name="distCoeffs">畸变系数</param>
	void saveCalibration(const string& filename, 
		const Mat& cameraMatrix, const Mat& distCoeffs)
	{
		FileStorage fs(filename, FileStorage::WRITE);
		//检查文件是否成功打开，并写入数据fs
		if (fs.isOpened())
		{
			fs << "camera_matrix" << cameraMatrix;
			fs << "distortion_coefficients" << distCoeffs;
			fs << "board_width" << boardSize.width;
			fs << "board_height" << boardSize.height;
			fs << "square_size" << squareSize.width;
			fs.release();//释放资源
		}
		else
		{
			cout << "文件保存失败" << endl;
		}
	}
};

// 流程：读取图像，标定，保存结果
int main()
{
	// 初始化标定器
	CameraCalibrator calibrator;

	//标定板数据：内角（10， 7），长度6.8mm
	calibrator.boardSize = Size(10, 7);
	calibrator.squareSize = Size(6.8, 6.8);

	//读取图像
	vector<Mat>images;
	string listFile = "calibdata.txt";
	calibrator.readImageList(listFile, images);
	
	//标定
	Mat cameraMatrix, distCoeffs;
	double rms = calibrator.calibrate(images, cameraMatrix, distCoeffs);
	if (rms < 0)
	{
		cout << "标定失败";
		return -1;
	}
	//保存结果
	calibrator.saveCalibration("calibrate_result.xml", cameraMatrix, distCoeffs);

	return 0;
}

//平均重投影误差:0.917884pixels
//相机内参矩阵 :
//[651.3801137452574, 0, 338.5310777861941;
//0, 651.6271630763471, 265.6301631724539;
//0, 0, 1]
//畸变系数 :
//	[-0.1102607696639906, -0.1585259687883431, 0.001534575934276704, -0.002870102933146971, 1.184947882717316]