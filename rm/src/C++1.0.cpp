#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// ��ʵ�ʱ����У����ǻ��һ��װ�װ�����Խ��м�¼����������ID��1-6������ɫ����ɫΪ0����ɫΪ1�������ĵ����꣬����ߡ�
// ��дһ��Armor���һ��Rect�ṹ�壬���ĵ�����Ĵ洢��ʽ�������ƣ����м���װ�װ����ţ����з��ӡ�
// - Ҫ�����£�
// ��1��Rect��Ҫ���������ID��װ�װ���ɫcolor������point����width����height�⼸�����ԣ�Rect�ĳ�ʼ����ʽ�������з��ӡ�
// ��2��Armor��Ҫ����м���װ�װ���������ĺ���Central_Point()��
// ��3��Armor��Ҫ����м���װ�װ�Խ��߳��ȵĺ���Diagonal() (������λС��)��
// ��4��Armor��Ҫ��������װ�װ�4������ĺ���Armor_Point()�������Ͻ����꿪ʼ˳ʱ�������
// ��5��Armor��Ҫ��������װ�װ���ɫ�Ĺ���Armor_Color()��
// ��6��Ҫ���к���ע�ͣ������߼������������������������˼��ʵ������Ҫ��


struct Rect {
    int id = 0;
    int color = 0;
    pair<int, int> point = { 0, 0 };
    int width = 0;
    int height = 0;

    //���캯��
    Rect(int i, int c, int x, int y, int w, int h)
        : id(i), color(c), point(x, y), width(w), height(h) {
    }
};

class Armor {
    //����Ȩ��
public:
    //����
    Rect num1;

    //ʹ�ó�Ա��ʼ���б�����num1��ֵ����ĳ�Ա����num1
    Armor(const Rect& num1) : num1(num1) {}

    //��Ϊ
        //����װ�װ���������ĺ���
    pair<double, double> Central_Point() {
        pair<double, double>point = {
            num1.width / 2 + num1.point.first,
            num1.height / 2 + num1.point.second };
        return point;
    }

    //����װ�װ�Խ��߳���
    double Diagonal() {//���ɶ���
        double lenth = hypot(num1.width, num1.height);
        return round(lenth * 100) / 100.0;
    }

    //���װ�װ�4�����꣬�����Ͻ����꿪ʼ˳ʱ�����
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

    //���װ�װ���ɫ
    string Armor_Color() {
        string Color;

        if (num1.color == 0)Color = "��ɫ";
        else if (num1.color == 1)Color = "��ɫ";
        return Color;
    }

    int ID() {
        return num1.id;
    }
};


int main() {
    int id, color, x, y, width, height;

    //����
    cout << "����������ID����ɫ��0��ɫ/1��ɫ��: ";
    cin >> id >> color;
    cout << endl << "���������Ͻǵ�����Ϳ���ߣ�x,y,width,height��: ";
    cin >> x >> y >> width >> height;

    Rect num1(id, color, x, y, width, height);
    Armor armor(num1);

    //���
    //ID����ɫ
    cout << "ID:" << id << "��ɫ��" << armor.Armor_Color() << endl;
    //���ĵ�����
    pair<double, double> center = armor.Central_Point();
    cout << "(" << center.first << ", " << center.second << ") ";
    //ǿ�Ʊ�����λС��
    cout << fixed << setprecision(2) << "���ȣ�" << armor.Diagonal() << endl;
    //�Ľǵ�����
    armor.Armor_Point();

    return 0;
}