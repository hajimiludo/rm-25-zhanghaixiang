#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace cv;

//3.赛事题
//1）装甲板识别
//加分项：若在ROS2环境下完成此题则加分（节点读取视频，通过话题发布帧信息，另一个节点读取该话题获取图像信息，并处理、识别图像中的装甲板，输出题目所需要输出的信息）
//a.识别待击打装甲板（注意下面有数字、较短的灯条为装甲板灯条；上面的长灯条为不用识别的场地干扰灯条，需要排除）
//b.识别并画出装甲板中心点
//c.使用相机位姿估计PnP算法解算相机到装甲板的距离（加分项）
//d.识别装甲板ROI并截取，获取装甲板具体数字（加分项）
// 
//输入 :
//读取上文给出含有装甲板的图片（图片识别成功后可以尝试识别视频）
//输出 :
//-运行效率 : 主要运算模块耗时 <= 20ms
//- 调试输出 :
//    -用矩形框绘制出目标装甲板在原图像中的位置
//    - 绘制出装甲板中心点
//    - 运行代码的延迟（运行效率）
//    - 相机到装甲板的距离（加分项）需完成应用题第二题（标定）
//    - 装甲板数字（加分项）



//常量定义
const float ARMOR_WIDTH = 235.0f;  //装甲板宽度
const float ARMOR_HEIGHT = 127.0f; //装甲板高度

//相机内参矩阵和畸变参数
const Mat CAMERA_MATRIX = (Mat_<double>(3, 3) <<
	651.380, 0, 338.531,
	0, 651.627, 265.630,
	0, 0, 1);
const Mat DIST_COEFFS = (Mat_<double>(1, 5) <<
	-0.11026, -0.15852, 0.00153, -0.00287, 1.18494);


///颜色分割阈值
//蓝色范围
const Scalar LOWER_BLUE_HSV(0, 0, 170);
const Scalar UPPER_BLUE_HSV(180, 255, 255);
//红色范围
const Scalar LOWER_RED_HSV_1(0, 50, 50);
const Scalar UPPER_RED_HSV_1(10, 255, 255);
const Scalar LOWER_RED_HSV_2(160, 50, 50);
const Scalar UPPER_RED_HSV_2(180, 255, 255);


//筛选参数
constexpr double MIN_LIGHT_AREA = 20.0;          //灯条最小面积
constexpr double MAX_LIGHT_AREA = 300.0;         //灯条最大面积
constexpr double MIN_LIGHT_ASPECT_RATIO = 5.0;   //灯条最小长宽比
constexpr double MAX_LIGHT_ASPECT_RATIO = 20.0;  //灯条最大长宽比
constexpr double MAX_LIGHT_Y_DIFFERENCE = 10.0;  //灯条最大y轴篇差 -->dy
constexpr double MIN_LIGHT_X_MULTIPLIER = 1.5;   //装甲板最小距离倍数 
constexpr double MAX_LIGHT_X_MULTIPLIER = 5.0;   //装甲板最大距离倍数
constexpr double MAX_LIGHT_ANGLE_DIFF = 10.0;   //两个灯条之间最大角度差


//灯条结构体
struct Light
{
	Point2f center; //灯条中心点坐标
	Rect bound; //灯条边界矩形
	RotatedRect rotated_rect;
	bool is_valid = false; //灯条是否有效

	Light() = default;
	Light(const RotatedRect& r) : rotated_rect(r)
	{
		center = r.center;
		bound = r.boundingRect();
		is_valid = true;
	}
};

// 装甲板结构体
struct Armor
{
	Point2f center;       //装甲板中心点
	Light light_pair[2];  //装甲板两侧灯条
	double distance = 0;      //装甲板到相机的距离
	int number = -1;      //识别到的数字
	RotatedRect bound;    //装甲板的最小外接矩形

	//构造函数
	Armor() = default;
	Armor(const Light& left, const Light& right) : light_pair{ left, right }
	{
		//装甲板中心点坐标
		center = (left.center + right.center) * 0.5f;

		// 计算装甲板外接矩形
		Point2f l[4], r[4];
		left.rotated_rect.points(l);
		right.rotated_rect.points(r);
		vector<Point2f> points{ l[0], l[1], l[2], l[3], r[0], r[1], r[2], r[3] };
		bound = minAreaRect(points);//计算这8个点的围成的矩形
	}
};

/// <summary>
/// 图像处理函数，把图像二值化并进行形态学操作
/// </summary>
/// <param name="img"></param>
Mat preprocessImage(const Mat& img)
{
	Mat hsv;
	// 转换为hsv
	cvtColor(img, hsv, COLOR_BGR2HSV);
	//二值化
	Mat mask_blue, mask_red_1, mask_red_2, mask_red, mask;      
	inRange(hsv, LOWER_BLUE_HSV, UPPER_BLUE_HSV, mask_blue);  //蓝色分割
	inRange(hsv, LOWER_RED_HSV_1, UPPER_RED_HSV_1, mask_red_1);//红色分割
	inRange(hsv, LOWER_RED_HSV_2, UPPER_RED_HSV_2, mask_red_2);
	mask_red = mask_red_1 | mask_red_2;// 合并红色
	mask = mask_blue | mask_red;  //合并蓝色和红色

	//imshow("1", mask);
	////形态学操作：开运算去噪和闭运算填补空隙
	//morphologyEx(mask, mask, MORPH_OPEN, 
	//	getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	//morphologyEx(mask, mask, MORPH_CLOSE,
	//	getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	return mask;
}

/// <summary>
/// 查找所有灯条矩形的面积的工具函数
/// </summary>
/// <param name="mask"></param>
/// <param name="img"></param>
void getLightData(const Mat& mask)
{
	Mat temp0 = mask.clone();
	vector<vector<Point>>contours;//存放找到的轮廓
	//查找轮廓
	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	double area = 0;
	cout << "----------contour info list-----------" << endl;
	for (size_t i = 0; i < contours.size(); ++i) {
		area = contourArea(contours[i]);
		//绘制所有轮廓的外接矩形
		if (area > 10 )
		{
			Rect bounding_rect = boundingRect(contours[i]);
			putText(mask, format("%.2f", area),
				Point(bounding_rect.x + 10, bounding_rect.y - 10),
				FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
			//计算单个灯条的最小包围矩形
			RotatedRect rect = minAreaRect(contours[i]);
			float w = rect.size.width;
			float h = rect.size.height;
			if (w < h) swap(w, h);// 确保w是长边，h是短边
			//计算长宽比和角度
			float aspect_ratio = w / h;
			// cout << "contour [" << i << "] area: " << area << " aspect_ratio:" << aspect_ratio << endl;
			printf("contour [%2zu] area: %8.2f\t\taspect_ratio: %.2f\n", i, area, aspect_ratio);
		}
		else continue;
	}
	imshow("temp0", temp0);
	/*----------contour info list-----------
*contour [ 0] area:   251.50             aspect_ratio: 5.13
*contour [ 1] area:    65.50             aspect_ratio: 10.89
*contour [ 2] area:   211.00             aspect_ratio: 6.02
*contour [ 3] area:    29.00             aspect_ratio: 16.22
contour [ 6] area:  1084.50             aspect_ratio: 22.76
contour [ 7] area:  1274.50             aspect_ratio: 22.71
contour [ 8] area:  2069.00             aspect_ratio: 17.62
contour [ 9] area:  2411.50             aspect_ratio: 16.65
contour [10] area:   375.00             aspect_ratio: 24.50
contour [14] area:    19.00             aspect_ratio: 2.39
contour [20] area:    20.50             aspect_ratio: 14.25*/
}

/// <summary>
/// 获取匹配条件数据
/// </summary>
/// <param name="lights"></param>
void getArmorData(const vector<Light>& lights)
{
	cout << "=============== results ==============" << endl;
	for (size_t i = 0; i < lights.size(); ++i)
	{
		for (size_t j = 0; j < lights.size(); ++j)
		{
			if (i == j) continue;

			const Light& left = lights[i];
			const Light& right = lights[j];
			//确保left是左侧灯条
			if (right.center.x < left.center.x)continue;
			//y轴偏差检查，筛选掉y坐标相差太大的灯条组合，
			// 保证同一个装甲板的左右灯条差不多在同一水平线上
			float dy = abs(left.center.y - right.center.y);
			//x轴距离检查，装甲板的宽度应该和灯条的宽度成比例
            // dx：两个灯条中心的水平距离，当作当前装甲板的宽度
			float dx = right.center.x - left.center.x;
			// avg_width两个灯条的平均宽度
			float avg_width = (left.bound.width + right.bound.width) / 2.0;
			float angle_diff = abs(left.rotated_rect.angle - right.rotated_rect.angle);
			// OpenCV的RotatedRect角度范围是[-90, 0)，需要处理环绕情况
			if (angle_diff > 90.0f) 
			{
				angle_diff = 180.0f - angle_diff;
			}
			cout << "Pair (" << i << ", " << j << "): "
				<< "dx=" << dx
				<< ", avg_width=" << avg_width
				<< ", dy=" << dy
				<< ", angle_diff=" << angle_diff
				<< endl;
			//cout << "left_y:" << left.center.y << "right_y:" << right.center.y << endl;
		}
	}
	cout << "======================================" << endl;
	/*
=============== results ==============
*Pair (0, 1): dx=53.0815, avg_width=26.5, dy=7.83636, angle_diff=0.784828
Pair (2, 0): dx=129.718, avg_width=30.5, dy=25.0119, angle_diff=74.1975
Pair (2, 1): dx=182.799, avg_width=27.5, dy=17.1755, angle_diff=73.4127
Pair (3, 0): dx=173.724, avg_width=27.5, dy=32.3422, angle_diff=73.355
Pair (3, 1): dx=226.805, avg_width=24.5, dy=24.5059, angle_diff=72.5701
*Pair (3, 2): dx=44.0063, avg_width=27.5, dy=7.33032, angle_diff=0.842525
Pair (3, 4): dx=42.536, avg_width=18, dy=446.148, angle_diff=16.2602
Pair (4, 0): dx=131.188, avg_width=30, dy=478.49, angle_diff=57.0948
Pair (4, 1): dx=184.269, avg_width=27, dy=470.654, angle_diff=56.3099
Pair (4, 2): dx=1.47025, avg_width=30, dy=453.478, angle_diff=17.1027
======================================*/
}

/// <summary>
/// 灯条筛选函数，根据灯条特征从二值化图像中筛选出最接近灯条的图像的外接旋转矩形
/// </summary>
/// <param name="binary">二值化的图像</param>
/// <returns>保存的灯条外接旋转矩形集</returns>
vector<Light>findLights(const Mat& mask)
{
	vector<Light>lights;// 存放灯条筛选结果
	vector<vector<Point>>contours;//存放找到的轮廓
	//查找轮廓
	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	//自动找出灯条轮廓，筛选面积->长宽比->旋转角度
	for (const auto& contour : contours)
	{
		//面积筛选
		double area = contourArea(contour);
		//过滤过小和过大的区域
		if (area < MIN_LIGHT_AREA ) continue;
		if (area > MAX_LIGHT_AREA) continue;

		//计算单个灯条的最小包围矩形
		RotatedRect rect = minAreaRect(contour);
		float w = rect.size.width;
		float h = rect.size.height;
		if (w < h) swap(w, h);// 确保w是长边，h是短边

		//计算长宽比和角度
		float aspect_ratio = w / h;
		//float angle = abs(rect.angle);
		//通过长宽比和角度来筛选
		if (aspect_ratio < MIN_LIGHT_ASPECT_RATIO ||
			aspect_ratio > MAX_LIGHT_ASPECT_RATIO) continue;

		//保存筛选出来的灯条
		lights.emplace_back(rect);
	}
	return lights;
}


/// <summary>
/// 计算相机到物体的距离，主要利用PnP算法
/// </summary>
/// <param name="armor">装甲板类，主要使用装甲板外接矩形顶点</param>
void PnPdistance(Armor& armor)
{
	//世界坐标系下的坐标，以装甲板平面为z轴，中心为原点
	float x = ARMOR_WIDTH / 2.0f;
	float y = ARMOR_HEIGHT / 2.0f;
	vector<Point3f>object_points =
	{
		Point3f(-x, -y, 0),
		Point3f(x, -y, 0),
		Point3f(x, y, 0),
		Point3f(-x, y, 0),
	};  //顺序：左上，右上，右下，左下

	//像素坐标系装甲板外接矩形坐标
	vector<Point2f>image_points;
	armor.bound.points(image_points);

	//PnP解算
	/*PnP算法原理：
	输入：
	3D世界坐标点（已知物体的真实尺寸）
	2D图像坐标点（在图像中检测到的点）
	相机内参矩阵
	相机畸变系数
	输出：
	rvec：旋转向量（描述物体相对相机的姿态）
	tvec：平移向量（描述物体相对相机的位置）*/
	Vec3d rvec, tvec;
	solvePnP(object_points, image_points,
		CAMERA_MATRIX, DIST_COEFFS, rvec, tvec);
	//solvePnP：通过最小化重投影误差来求解r旋转矩阵和t平移向量

	//计算装甲板中心到相机光心的直线距离（单位：mm）
	armor.distance = norm(tvec);//norm：计算向量的模长（欧几里得范数）
}
 
/// <summary>
/// 通过找到的灯条外界旋转矩形匹配（合成）装甲板
/// </summary>
/// <param name="lights">找到的灯条外接矩形</param>
/// <returns></returns>
vector<Armor> findArmors(const vector<Light>& lights)
{
	//存放结果，即装甲板的外接矩形顶点
	vector<Armor>armors;
	//暴力配对，遍历所有灯条组合
	for (size_t i = 0; i < lights.size(); ++i)
	{
		for (size_t j = 0; j < lights.size(); ++j)
		{
			if (i == j) continue;

			const Light& left = lights[i];
			const Light& right = lights[j];

			//确保left是左侧灯条
			if (right.center.x < left.center.x)continue;
			//y轴偏差检查，筛选掉y坐标相差太大的灯条组合，
			// 保证同一个装甲板的左右灯条差不多在同一水平线上
			float dy = abs(left.center.y - right.center.y);
			if (dy > MAX_LIGHT_Y_DIFFERENCE)continue;

			//x轴距离检查，装甲板的宽度应该和灯条的宽度成比例
			// dx：两个灯条中心的水平距离，当作当前装甲板的宽度
			float dx = right.center.x - left.center.x; 
			// avg_width两个灯条的平均宽度
			float avg_width = (left.bound.width + right.bound.width) / 2.0;
			if (dx < avg_width * MIN_LIGHT_X_MULTIPLIER ||
				dx > avg_width * MAX_LIGHT_X_MULTIPLIER)continue;

			//角度检查，同一装甲板上的两个灯条倾斜的角度一致
			//过滤掉角度相差太大的灯条组合
			if (abs(left.rotated_rect.angle - right.rotated_rect.angle) > MAX_LIGHT_ANGLE_DIFF)continue;

			//创建装甲板并计算距离
			Armor armor(left, right);
			PnPdistance(armor);//计算距离
			armors.push_back(armor);//保存装甲板
		}
	}
	return armors;
}


/// <summary>
/// 绘制结果（装甲板外接矩形，中心点，距离信息）
/// </summary>
/// <param name="img">目标图像</param>
/// <param name="armors">找到的装甲板</param>
void drawResults(Mat& img, const vector<Armor>& armors)
{
	//Mat result_image;
	//img.copyTo(result_image);
	for (const auto& armor : armors)
	{
		//绘制装甲板外接矩形
		Point2f points[4];
		armor.bound.points(points);//获取旋转矩形的4个顶点
		for (int i = 0; i < 4; i++)
		{
			line(img, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 2);
		}
		cout << "已用矩形框绘制出目标装甲板在原图像中的位置" << endl;

		//绘制装甲板中心点
		circle(img, armor.center, 5, Scalar(0, 255, 0), -1);
		cout << "已绘制装甲板中心点" << endl;

		//绘制距离信息
		putText(img, format("distance:%.2fmm", armor.distance),
			Point(armor.center.x, armor.center.y - 30),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
		cout << "相机到装甲板的距离：" << armor.distance << "mm" << endl;
	}
}


/// <summary>
/// 处理视频流
/// </summary>
/// <param name="path">目标视频路径</param>
void proprecessVideo(const string& path)
{
	VideoCapture cap(path);

	if (!cap.isOpened())
	{
		cerr << "视频文件打开失败" << endl;
		return;
	}

	Mat frame;
	int frame_count = 0;

	while (true)
	{
		cap >> frame;

		if (frame.empty())
		{
			cout << "读取失败" << endl;
			break;
		}


		//图像处理，获得二值掩码图像
		Mat mask = preprocessImage(frame);

		//获取所有矩形面积
		//getArea(mask);
		auto start_time = chrono::high_resolution_clock::now();


		//找到灯条（灯条外接矩形矩形）
		vector<Light>lights = findLights(mask);

		//获取灯条在装甲板上的特征数据
		//getArmorData(lights);

		//找到装甲板（装甲板外接矩形）
		vector<Armor>armors = findArmors(lights);

		//
		auto end_time = chrono::high_resolution_clock::now();
		double used_time = chrono::duration<double, milli>(end_time - start_time).count();
		cout << "消耗时间：" << used_time << "ms" << endl;

		//绘制结果
		drawResults(frame, armors);

		imshow("Video", frame);
		waitKey(0);

		frame_count++;

	}
	cap.release();
	destroyAllWindows();
}

/// <summary>
/// 处理图片流
/// </summary>
/// <param name="path">目标图片路径</param>
void processImage(const string& path)
{
	Mat img = imread(path);
	resize(img, img, Size(800, 600));


	//图像处理，获得二值掩码图像
	Mat mask = preprocessImage(img);

	//获取所有矩形面积
	//getArea(mask);

	auto start_time = chrono::high_resolution_clock::now();

	//找到灯条（灯条外接矩形矩形）
	vector<Light>lights = findLights(mask);
	//在图中绘制筛选找到的灯条进行检查
	//Mat temp1 = img.clone();
	//for (size_t i = 0; i < lights.size(); ++i)
	//{
	//	const Light& light = lights[i];
	//	putText(temp1, to_string(i), light.center,
	//		FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
	//}
	//imshow("temp1", temp1);

	//获取灯条在装甲板上的特征数据
	//getArmorData(lights);

	//找到装甲板（装甲板外接矩形）
	vector<Armor>armors = findArmors(lights);

	//计算运行效率
	auto end_time = chrono::high_resolution_clock::now();
	double used_time = chrono::duration<double, milli>(end_time - start_time).count();
	cout << "消耗时间：" << used_time << "ms" << endl;

	//绘制结果
	drawResults(img, armors);

	////显示图片
	imshow("mask", mask);
	imshow("image", img);
	waitKey(0);
	destroyAllWindows;
}

int main()
{
	string path = "./pictures/blue1.jpg";	

	processImage(path);

	return 0;
}


