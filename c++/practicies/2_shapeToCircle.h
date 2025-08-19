#include <string> 
#include <iostream>



    //抽象类
class Shape{
    protected:

    std::string name;
    std::string color;

    public:

    virtual double calculateArea() const = 0;
    virtual double calculatePerimeter() const = 0;
    virtual void displayInfo() const;

    //构造和析构
    Shape() = default;
//成员初始化列表
    // Shape(string name, string color) : name(name), color(color) {}
    Shape(std::string name, std::string color) {

        this->name = name;
        this->color = color;
    }

//若不虚析构可能导致派生类对象通过基类指针删除时资源释放不全
    virtual ~Shape() = default;

};


class Circle : public Shape{
    private:
    
    double radius;

    public:

    virtual double calculateArea() const;
    virtual double calculatePerimeter() const;

    Circle() = default;
    Circle(std::string name, std::string color, double radius){

        this->name = name;
        this->color = color;
        this->radius = radius;
    }

};


class Rectangles : public Shape{
    private:

    double width, height;

    public:

    virtual double calculateArea() const;
    virtual double calculatePerimeter() const;

    Rectangles() = default;
    Rectangles(std::string name, std::string color, double width , double height) {

        this->name = name;
        this->color = color;
        this->width = width;
        this->height = height;

    }
};
