#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

//二，c++应用题
//在实际比赛中，我们会对一个装甲板的属性进行记录，包括数字ID（1 - 6），颜色（蓝色为0、红色为1），中心点坐标，宽与高。
//请写一个Armor类和一个Rect结构体，中心点坐标的存储方式不做限制，其中假设装甲板正放，自行发挥。
//输入：
//第一行输入数字ID和颜色
//第二行输入左上角点坐标和宽与高
//输出：
//第一行输出数字ID和颜色
//第二行输出装甲板中心坐标和对角线长度
//第三行输出装甲板4点坐标，从左上角顺时针开始


//（1）Rect内要求具有数字ID，装甲板颜色color，坐标point，宽width，高height这几个属性，Rect的初始化方式可以自行发挥。
//定义装甲板属性
struct Rect {
    int id = 0;// 装甲板id
    int color = 0;// 装甲板颜色
    pair<int, int> point = { 0, 0 };//装甲板左上角点坐标
    int width = 0;//装甲板的宽
    int height = 0;//装甲板的高

    //构造函数
    
    Rect(int i, int c, int x, int y, int w, int h)
        : id(i), color(c), point(x, y), width(w), height(h) {
    }
};


class Armor {
private:
    Rect num1;//存储传入的装甲板数据

public:
    /*
    * @brief 构造函数，接受一个Rect对象来初始化Armor类
    * @pafam rest 包含装甲板初始数据的rect对象
    */
     explicit Armor(const Rect& rect) : num1(rect) {}

    //（2）Armor类要求具有计算装甲板中心坐标的函数Central_Point()。
    // 通过宽和高和左上角点坐标求中心点坐标
    pair<double, double> Central_Point() {
        pair<double, double>point = {
            num1.width / 2 + num1.point.first,// 宽 / 2 + x
            num1.height / 2 + num1.point.second };// 高 / 2 + y
        return point;
    }

    //（3）Armor类要求具有计算装甲板对角线长度的函数Diagonal() (保留两位小数)。
    // 求对角线长度
    double Diagonal() {
        return sqrt(pow(num1.width, 2) + pow(num1.height, 2));
    }

    //（4）Armor类要求具有输出装甲板4点坐标的函数Armor_Point()，从左上角坐标开始顺时针输出。
    // 从左上角坐标开始顺时针输出装甲板四角点坐标
    void Armor_Point() {
        int x = num1.point.first;// 左上角点x
        int y = num1.point.second;//左上角点y
        int w = num1.width;// 装甲板宽
        int h = num1.height;//装甲板高

        cout << "(" << x << ", " << y << ")";
        cout << "(" << x + w << ", " << y << ")";
        cout << "(" << x + w << ", " << y + h << ")";
        cout << "(" << x << ", " << y + h << ")" << endl;
    }

    //（5）Armor类要求具有输出装甲板颜色的功能Armor_Color()。
    // 输出装甲板颜色
    void Armor_Color() {
        string Color;

        if (num1.color == 0) {
            Color = "蓝";
            cout << "颜色：" << Color << endl;
        }
        else if (num1.color == 1) {
            Color = "红";
            cout << "颜色：" << Color << endl;
        };
    }

};



int main() {
    int id, color, x, y, width, height;

    //实例输入：
    //1 0
    //10 20 30 40
    cin >> id >> color;
    cin >> x >> y >> width >> height;

    Rect num1(id, color, x, y, width, height);
    Armor armor(num1);

    //实例输出：
    //ID：1   颜色：蓝
    //(25, 40) 长度：50.00
    //(10, 20) (40, 20) (40, 60) (10, 60)
    cout << "ID: " << id << "\t";
    armor.Armor_Color();
    pair<double, double> center = armor.Central_Point();
    cout << "(" << center.first << ", " << center.second << ") ";
    cout << "长度: " << fixed << setprecision(2) << armor.Diagonal() << endl;
    armor.Armor_Point();

    return 0;
}
