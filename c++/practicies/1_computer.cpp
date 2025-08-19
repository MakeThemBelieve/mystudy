#include <iostream>
#include <string>
#include <map>

using namespace std;


enum class operation {
    ADD,  //加法
    SUB,  //减法
    MUl,  //乘法
    DIV,  //除法
    REM,  //取余
    EQU,  //等于
    CLE,  //清空
};


map<string , operation> opeMap = {
        {"ADD",operation::ADD},
        {"add", operation::ADD},
        {"+",operation::ADD},
        {"SUB",operation::SUB},
        {"sub", operation::SUB},
        {"-",operation::SUB},
        {"MUl",operation::MUl},
        {"mul", operation::MUl},
        {"*",operation::MUl},
        {"DIV",operation::DIV},
        {"div", operation::DIV},
        {"/",operation::DIV},
        {"REM",operation::REM},
        {"rem", operation::REM},
        {"%",operation::REM},
        {"EQU",operation::EQU},
        {"equ", operation::EQU},
        {"=",operation::EQU},
        {"CLE",operation::CLE},
        {"cle", operation::CLE},
        {"c",operation::CLE},
    };


class Computer{
    public:
    int result(int total , int b, string ope) {

        auto it = opeMap.find(ope);

        //find没找到时候会返回end，这时候直接访问value可能会导致未定位行为
        if (it == opeMap.end()) {
            cout << "错误: 无效操作符 '" << ope << "'，请重新输入" << endl;
            return total; // 返回当前总值，不改变
        }

        switch(it->second) {

            case operation::ADD:
                cout << "请输入下一个数:" << endl;
                cin >> b;
                total = total + b;
                cout << "加法运算完成，新结果: " << total << endl;
                return total;

            case operation::SUB:
                cout << "请输入下一个数:" << endl;
                cin >> b;
                total = total - b;
                cout << "减法运算完成，新结果: " << total << endl;
                return total;

            case operation::MUl:
                cout << "请输入下一个数:" << endl;
                cin >> b;
                total = total * b;
                cout << "乘法运算完成，新结果: " << total << endl;
                return total;

            case operation::DIV:
                cout << "请输入下一个数:" << endl;
                cin >> b;
                total = total / b;
                cout << "除法运算完成，新结果: " << total << endl;
                return total;

            case operation::REM:
                cout << "请输入下一个数:" << endl;
                cin >> b;
                total = total % b;
                cout << "取余运算完成，新结果: " << total << endl;
                return total;

            case operation::EQU:
                cout << "当前结果是:" << total << endl;
                return total;
            case operation::CLE:
                total = 0;
                cout << "当前结果以已清空，新结果:" << total << endl;
                return total;
            default:
                cout << "错误: 未实现的操作" << endl;
                return total;
        }
    }
};

// 字符串不能直接和枚举对象对标，需要map映射   it->first访问key，it->second 访问value
int main() {

    cout << "欢迎使用计算器，请输入第一个数:"<<endl;

    int b,total = 0;
    string ope;
    cout << "请输入第一个数:" << endl;
    cin >> total;
    Computer c;

    while(true) {
        cout << "请输入操作：" << endl;
        // getline(cin , ope);
        cin >> ope;
        total = c.result(total, b, ope);
        cout << endl;
    }

    return 1;
}