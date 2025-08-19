#include "2_shapeToCircle.h"
#include <vector>
#include <iomanip>  // 用于格式化输出
// #include <windows.h>

using namespace std;

void Shape::displayInfo() const {

        cout << "形状：" << name << endl;
        cout << "颜色：" << color << endl;
        cout << "面积：" << fixed << setprecision(2) << calculateArea() << endl;
        cout << "周长：" << fixed << setprecision(2) << calculatePerimeter() << endl;
        cout << "-------------------" << endl;
}

double Circle::calculateArea() const {

   return radius * radius * 3.14159;
    
}

double Circle::calculatePerimeter() const {

    return radius * 2 * 3.14159;
}


double Rectangles::calculateArea() const {

   return width * height;
}

double Rectangles::calculatePerimeter() const {

    return 2 *(width + height);
}



int main() {
//中文输入，设置控制台编码
    // SetConsoleOutputCP(CP_UTF8);

    Circle circle("圆" , "red" , 5.0);
    Rectangles rectangle("长方形", "blue" , 4.0 , 6.0);

    
    // 创建形状指针容器
    vector<Shape*> shapes;
    shapes.push_back(&circle);
    shapes.push_back(&rectangle);
    
    // 添加更多形状
    shapes.push_back(new Circle("cir" , "green", 3.0));
    shapes.push_back(new Rectangles("rec" , "yello", 5.0, 5.0));
    
    // 多态处理所有形状
    cout << "===== 形状信息 =====" << endl;
    for (Shape* shape : shapes) {
        shape->displayInfo();
    }
    
    // 释放动态分配的内存
    for (Shape* shape : shapes) {
        // 只删除动态分配的对象
        if (shape != &circle && shape != &rectangle) {
            delete shape;
        }    
    }

    return 0;
}