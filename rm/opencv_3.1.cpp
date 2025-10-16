#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//获取图像--图像预处理--筛选矩阵--识别装甲板--距离解算、
//装甲板识别
//a.识别待击打装甲板（注意下面有数字、较短的灯条为装甲板灯条；上面的长灯条为不用识别的场地干扰灯条，需要排除）
//b.识别并画出装甲板中心点
//c.使用相机位姿估计PnP算法解算相机到装甲板的距离（加分项）
//d.识别装甲板ROI并截取，获取装甲板具体数字（加分项）

//输出:
//-运行效率 : 主要运算模块耗时 <= 20ms
//- 调试输出 :
//    - 用矩形框绘制出目标装甲板在原图像中的位置
//    - 绘制出装甲板中心点
//    - 运行代码的延迟（运行效率）
//    - 相机到装甲板的距离（加分项）需完成应用题第二题（标定）

//获取图像
Mat getImg(string path) {
    Mat img = imread(path);
    if (img.empty()) {
        cout << "读取图像失败" << endl;
    }
    resize(img, img, Size(), 0.5, 0.5, INTER_AREA);
    return img;
}

//转换图像通道，图像滤波，阈值化,形态学处理
Mat ProcessImg(const Mat& img) {
    Mat imghsv, mask;
    cvtColor(img, imghsv, COLOR_BGR2HSV);//转化为hsv颜色空间进行形态学处理
    
    //装甲板蓝：0,255,0,255,200,255
    Scalar lower(0, 0, 220);
    Scalar upper(255, 255, 255);
    inRange(imghsv,lower, upper, mask);//得到装甲蓝色灯条的轮廓

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(mask, mask, kernel);

    imshow("mask", mask);
    return mask;
}

//边缘检测，查找轮廓，筛选去除部分矩阵,遍历所有矩阵，两两组合判断
struct goalPoints {
    Point2f p[4];//灯条矩形四个顶点
    Point2f cpt;//灯条矩形的中心点
    bool datas = false;//是否存入所有数据
};
vector<goalPoints>gp;

vector<goalPoints> Detect(const Mat& mask, const Mat& img) {
    //查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<double> areas;
    int m = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        areas.push_back(area);
        cout << "area:" << area << endl;
        //这里先用面积识别灯条
        if (area >= 80 && area <= 400) {
            RotatedRect rect1 = minAreaRect(contours[i]);//轮廓外接最小矩形,输出四个顶点,中心坐标.center和旋转角度
            //记录每个灯条矩形的四顶点和中心点坐标
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
    //在终端输出识别到的灯条轮廓的中的和顶点
    //for (int i = 0; i < num1; i++) {
    //    if (!gp.empty() && gp[i].datas == true) {
    //        cout << i + 1 << ":" << endl << "center point:" << gp[i].cpt << endl;
    //        for (int j = 0; j < 4; j++) cout << "p:" << gp[i].p[j] << endl;
    //    }
    //    else cout << "未成功识别到灯条" << endl;
    //}
    return gp;
}

//绘图，输出装甲板中心相对画面中心的坐标
void draw(const Mat& img) {
    int num1 = gp.size();
    //sort（）按照中心点x坐标值从小到大进行排序
    sort(gp.begin(), gp.end(), [](const goalPoints& a, const goalPoints& b) {
        return a.cpt.x < b.cpt.x;
        });

    //绘制中心点
    for (int i = 0; i < num1 - 1; i = i + 2) {
        Point cpt1 = (gp[i].cpt + gp[i + 1].cpt) / 2;//求矩形中点
        circle(img, cpt1, 3, Scalar(0, 200, 255), -1);
    }

    /////////////////用来查看重新排序后各顶点在gp.p[]中的编号
    //for (int i = 0; i < 4; i++) {
    //    string num1 = to_string(i);
    //    for (int j = 0; j < 4; j++) {
    //        string num = to_string(j);
    //        putText(img, num, gp[i].p[j], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 200, 255), 1);
    //   }
    //    putText(img, num1, gp[i].cpt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 200, 255), 1);
    //}
    
    //绘制矩形
    for (int i = 0; i < num1 - 1; i += 2) {
        // 绘制装甲板四边形（连接左右灯条的外侧点）
        vector<Point> armor_points;
        float y0 = gp[i].p[0].y;//灯条的0号顶点的y值
        float y2 = gp[i + 1].p[2].y;//灯条的2号顶点的y值

        //如若y0 > y2，则0号顶点是左上角，2号顶点是右下角，1号顶点是右上角，3号是左下角
        if (y0 > y2)
        {
            armor_points.push_back(gp[i].p[1]);     // 左灯条左上角
            armor_points.push_back(gp[i + 1].p[2]);   // 右灯条右上角  
            armor_points.push_back(gp[i + 1].p[3]);   // 右灯条右下角
            armor_points.push_back(gp[i].p[0]);     // 左灯条左下角
        }
        //如若y0 > y2，则0号顶点是左下角，2号顶点是右上角，1号顶点是左上角，3号是右下角
        else if(y0 < y2)
        {
            armor_points.push_back(gp[i].p[0]);     // 左灯条左上角
            armor_points.push_back(gp[i + 1].p[1]);   // 右灯条右上角  
            armor_points.push_back(gp[i + 1].p[2]);   // 右灯条右下角
            armor_points.push_back(gp[i].p[3]);     // 左灯条左下角
        }
        // 绘制装甲板边界
        polylines(img, armor_points, true, Scalar(0, 255, 0), 2);
    }
}



//pnp解算出相机离装甲板距离
float distance(const Mat& img) {
    //单目位姿估计，solvePnP(),solvePnPRansac()计算旋转向量和平移向量
    //旋转向量和旋转矩阵之间相互转换Rodrigues()

    //装甲板的真实宽和高
    float width = 0.13f;////////////？？？
    float height = 0.055f;//////////？？？
    // 装甲板的3D世界坐标（以装甲板中心为原点）
    vector<Point3f> objPoints;
    objPoints.push_back(Point3f(-width / 2, -height / 2, 0)); // 左上角
    objPoints.push_back(Point3f(width / 2, -height / 2, 0));  // 右上角
    objPoints.push_back(Point3f(width / 2, height / 2, 0));   // 右下角
    objPoints.push_back(Point3f(-width / 2, height / 2, 0));  // 左下角

    Mat cameraMatrix = (Mat_<float>(3, 3) << 651.380, 0, 338.531,
        0, 651.627, 265.630,
        0, 0, 1);
    Mat distCoeffos = (Mat_<float>(1, 5) << -0.1102607696639885, -0.158525968788358,
        0.001534575934276915, -0.002870102933147538, 1.184947882717307);
    Mat rvec;//旋转向量
    Mat tvec;//平移向量

    for (int i = 0; i < gp.size() - 1; i += 2) {
        goalPoints& leftLight = gp[i];
        goalPoints& rightLight = gp[i + 1];

        // 计算装甲板的四个图像角点
        // 使用两个灯条的适当点构建装甲板的四个角点
        vector<Point2f> imgPoints;
        // 左上角：左灯条的左上点
        imgPoints.push_back(leftLight.p[0]);
        // 右上角：右灯条的右上点
        imgPoints.push_back(rightLight.p[1]);
        // 右下角：右灯条的右下点
        imgPoints.push_back(rightLight.p[2]);
        // 左下角：左灯条的左下点
        imgPoints.push_back(leftLight.p[3]);

        bool success = solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffos, rvec, tvec);
        if (success) {
            // 计算距离（平移向量的模）
            double distance = norm(tvec);
            // 输出距离信息
            cout << "Distance:" << distance << endl;

            // 在图像上显示距离
            Point2f cpt2 = (leftLight.cpt + rightLight.cpt) / 2;
            string distText = "Distance: " + to_string(distance);
            putText(img, distText, cpt2 + Point2f(-150, -400), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);

            ////输出旋转向量和旋转矩阵
            //cout << "世界坐标系变换到相机坐标系的旋转向量" << rvec << endl;
            //Mat R;
            //Rodrigues(rvec, R);
            //cout << "旋转矩阵：" << R << endl;

            return distance;
        }
    }
}



int main()
{   
    // 创建计时器
    TickMeter tm;
    //步骤一获取装甲板图像
    string path = "./rm/rm_test/蓝1.jpg";
    Mat img = getImg(path);//传入图像路径获取图像，并进行获取结果检测和初步调整

    //步骤二形态学处理
    Mat mask = ProcessImg(img);

    tm.start();
    //步骤三找到并绘制矩形
    Detect(mask, img);
    draw(img);
    
    //步骤四计算距离
    distance(img);
    tm.stop();
    cout << "主要运算模块耗时: " << tm.getTimeMilli() << " ms" << endl;

    imshow("img", img);
    waitKey(0);
    return 0;
}