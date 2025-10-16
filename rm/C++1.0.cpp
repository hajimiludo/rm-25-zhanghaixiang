#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// 在实际比赛中，我们会对一个装甲板的属性进行记录，包括数字ID（1-6），颜色（蓝色为0、红色为1），中心点坐标，宽与高。
// 请写一个Armor类和一个Rect结构体，中心点坐标的存储方式不做限制，其中假设装甲板正放，自行发挥。
// - 要求如下：
// （1）Rect内要求具有数字ID，装甲板颜色color，坐标point，宽width，高height这几个属性，Rect的初始化方式可以自行发挥。
// （2）Armor类要求具有计算装甲板中心坐标的函数Central_Point()。
// （3）Armor类要求具有计算装甲板对角线长度的函数Diagonal() (保留两位小数)。
// （4）Armor类要求具有输出装甲板4点坐标的函数Armor_Point()，从左上角坐标开始顺时针输出。
// （5）Armor类要求具有输出装甲板颜色的功能Armor_Color()。
// （6）要求有合理注释，代码逻辑条理清晰，建议用面向对象思想实现上述要求


struct Rect {
    int id = 0;
    int color = 0;
    pair<int, int> point = { 0, 0 };
    int width = 0;
    int height = 0;

    //构造函数
    Rect(int i, int c, int x, int y, int w, int h)
        : id(i), color(c), point(x, y), width(w), height(h) {
    }
};

class Armor {
    //访问权限
public:
    //属性
    Rect num1;

    //使用成员初始化列表将参数num1赋值给类的成员变量num1
    Armor(const Rect& num1) : num1(num1) {}

    //行为
        //计算装甲板中心坐标的函数
    pair<double, double> Central_Point() {
        pair<double, double>point = {
            num1.width / 2 + num1.point.first,
            num1.height / 2 + num1.point.second };
        return point;
    }

    //计算装甲板对角线长度
    double Diagonal() {//勾股定理
        double lenth = hypot(num1.width, num1.height);
        return round(lenth * 100) / 100.0;
    }

    //输出装甲板4点坐标，从左上角坐标开始顺时针输出
    void Armor_Point() {
        int x = num1.point.first;
        int y = num1.point.second;
        int w = num1.width;
        int h = num1.height;

        cout << "(" << x << "," << y << ")";
        cout << "(" << x + w << "," << y << ")";
        cout << "(" << x + w << "," << y + h << ")";
        cout << "(" << x << "," << y + h << ")";
    }

    //输出装甲板颜色
    string Armor_Color() {
        string Color;

        if (num1.color == 0)Color = "蓝色";
        else if (num1.color == 1)Color = "红色";
        return Color;
    }

    int ID() {
        return num1.id;
    }
};


int main() {
    int id, color, x, y, width, height;

    //输入
    cout << "请输入数字ID和颜色（0蓝色/1红色）: ";
    cin >> id >> color;
    cout << endl << "请输入左上角点坐标和宽与高（x,y,width,height）: ";
    cin >> x >> y >> width >> height;

    Rect num1(id, color, x, y, width, height);
    Armor armor(num1);

    //输出
    //ID和颜色
    cout << "ID:" << id << "颜色：" << armor.Armor_Color() << endl;
    //中心点坐标
    pair<double, double> center = armor.Central_Point();
    cout << "(" << center.first << ", " << center.second << ") ";
    //强制保存两位小数
    cout << fixed << setprecision(2) << "长度：" << armor.Diagonal() << endl;
    //四角点坐标
    armor.Armor_Point();

    return 0;
}