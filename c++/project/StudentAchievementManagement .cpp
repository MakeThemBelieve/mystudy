#include <string>
#include <iostream>
#include <unordered_map>
#include <iomanip>

using namespace std;

struct Student{
    int id;        //学号
    string name;   //姓名
    float score;   //成绩
};

enum class operation{
    ADD,   //添加
    SHOW,  //显示
    STA,   //统计
    INQ,   //查询
    MOD,   //修改
    DEL,   //删除
    EXIT,  //退出
};

class System{

    public:

    int count = 0;
    unordered_map<int , int> idIndex;
    unordered_map<string , int> nameIndex;

//*number 解引用获取值， &number 获取地址值
//如果使用了指针需要解引用获取到值，不然直接赋值会导致获取的是内存地址（指针本质是个值是内存地址的变量） int index = *number;
    void newStudent(Student* class1, int& number) {
        
        if(number >= count) {
            cout << "班级已经满员,无法再新增,将自动返回操作菜单" <<endl;
            return ;
        }

        int isBack = 0;

        do{
            cout << "请输入学生学号：" << endl;//没有判断学号是有效的，待修复
            cin >> class1[number].id;
            while (idIndex[class1[number].id] != 0) {
                cout << "该学号已经存在，请重新输入学生学号：" << endl;
                cin >> class1[number].id;
            }
            idIndex[class1[number].id] = number + 1;

            cout << "请输入学号'" << class1[number].id  << "'对应的学生姓名：" << endl;
            cin >> class1[number].name;    //目前window输入中文会乱码，暂未修复
            while (nameIndex[class1[number].name] != 0) {
                cout << "该姓名已经存在，请重新输入学生姓名：" << endl;
                cin >> class1[number].name;
            }
            nameIndex[class1[number].name] = number + 1;

            cout << "请输入学号'" << class1[number].id  << "'学生姓名'" << class1[number].name << "'的成绩是：" << endl;
            cin >> class1[number].score;//没有判断成绩是有效的，待修复
// (*number)++;如果要操作指针的值需要解引用，不然会导致变动的是内存地址导致指针指向别的变量

            cout << "***************" << endl;
            cout << "已收录学号:'" << class1[number].id  << "'学生姓名:'" << class1[number].name << "'成绩:'" << class1[number].score << "'" <<endl;
            number++;
            if (number < count) {
                cout << "输入0    继续录入学生" << endl;
                cout << "输入1    返回操作菜单" << endl;
                cout << "***************" << endl;
            } else{
                cout << "班级已经满员,将自动返回操作菜单" <<endl;
                cout << "***************" << endl;
                return ;
            }
            
            do{
                cin >> isBack;
                if (isBack != 0 && isBack != 1) cout << "错误：无效操作符：‘" << isBack <<"’，请重新输入" <<endl;
            } while (isBack != 0 && isBack != 1);

        }while(isBack == 0);

        return ;
    }    
    
    void showStudent(Student* class1, int& number) {

        if (number == 0) {
            cout << "当前系统没有录入学生信息，请录入后重试，将自动返回操作菜单" << endl;
        }

//setw自动左对其10个字符，注意setw是对下一项生效
        cout << "------------------------" << endl;
        cout << left << setw(10) << "学号" << setw(8) << "姓名" << setw(5) << "成绩" << endl;
        for (int i = 0 ; i < number; i++)  {
            cout << left << setw(10) << class1[i].id << setw(8) << class1[i].name << setw(5) << class1[i].score << endl; 
        }
        cout << "------------------------" << endl;
        return ;
    }

    void statisticsStudent(Student* class1, int& number){

        if (number == 0) {
            cout << "当前系统没有录入学生信息，请录入后重试，将自动返回操作菜单" << endl;
        }
        
        int sum = 0;
        int maxIndex = 0 , minIndex = 0;
        for (int i = 0 ; i < number ; i++) {
            sum += class1[i].score;
            if (class1[i].score > maxIndex) maxIndex = i;
            if (class1[i].score < minIndex) minIndex = i;
        }

        cout << "------------------------" << endl;
        cout << "全班平均分：" << (double)sum/number << endl;
        cout << left << "全班最高分的学生\t学号:" << setw(10) << class1[maxIndex].id << "姓名" << setw(8) << class1[maxIndex].name << "成绩" << setw(5) << class1[maxIndex].score << endl;
        cout << left << "全班最低分的学生\t学号:" << setw(10) << class1[minIndex].id << "姓名" << setw(8) << class1[minIndex].name << "成绩" << setw(5) << class1[minIndex].score << endl;
        cout << "------------------------" << endl;

        return ;
    }

    void inquireStudent(Student* class1, int& number) {

        if (number == 0) {
            cout << "当前系统没有录入学生信息，请录入后重试，将自动返回操作菜单" << endl;
        }

        int i = 0;
        while(true){
        cout << "------------------------" << endl;
        cout << "通过学号查询 请输入1" << endl;
        cout << "通过姓名查询 请输入2" << endl;
        cout << "通过成绩查询 请输入3" << endl;
        cout << "返回操作菜单 请输入4" << endl;
        cout << "------------------------" << endl;
            cin >> i;
            switch(i) {
                case 1:
                    while(true) {

                        cout << "请输入要查询的学号：" << endl;
                        int inquire = 0;
                        cin >> inquire;
                        if (idIndex[inquire] == 0) {
                            cout << "该学生不存在" << endl;
                        } else {
                            cout << left << "学号:" << setw(10) << class1[idIndex[inquire]].id << "姓名" << setw(8) << class1[idIndex[inquire]].name << "成绩" << setw(5) << class1[idIndex[inquire]].score << endl;
                        }

                            cout << "想继续学号查询 请输入1" << endl;
                            cout << "想更换查询方式 请输入2" << endl;
                            int j = 0;
                            do {
                                cin >> j; 
                                if(j != 1 && j != 2) cout << "错误的操作符：'" << j << " ',请重新输入操作符" << endl;
                            } while(j != 1 && j != 2);
                            if (j == 2) break;
                    }
                    break;
                case 2:
                    while(true) {

                        cout << "请输入要查询的姓名：" << endl;
                        string inquire;
                        cin >> inquire;
                        if (nameIndex[inquire] == 0) {
                            cout << "该学生不存在" << endl;
                        } else {
                            cout << left << "学号:" << setw(10) << class1[nameIndex[inquire]].id << "姓名" << setw(8) << class1[nameIndex[inquire]].name << "成绩" << setw(5) << class1[nameIndex[inquire]].score << endl;
                        }

                            cout << "想继续姓名查询 请输入1" << endl;
                            cout << "想更换查询方式 请输入2" << endl;
                            int j = 0;
                            do {
                                cin >> j; 
                                if(j != 1 && j != 2) cout << "错误的操作符：'" << j << " ',请重新输入操作符" << endl;
                            } while(j != 1 && j != 2);
                            if (j == 2) break;
                    }
                    break;
                case 3:
                    while(true) {

                        cout << "请输入要查询的：" << endl;
                        

                            cout << "想继续姓名查询 请输入1" << endl;
                            cout << "想更换查询方式 请输入2" << endl;
                            int j = 0;
                            do {
                                cin >> j; 
                                if(j != 1 && j != 2) cout << "错误的操作符：'" << j << " ',请重新输入操作符" << endl;
                            } while(j != 1 && j != 2);
                            if (j == 2) break;
                    }
                    break;
                case 4:
                    return ;
                default:
                    cout << "错误的操作符：'" << i << "',请重新输入" << endl;
                    break;
            }
        }
    }

    void modifStudent(Student* class1, int& number) {

        if (number == 0) {
            cout << "当前系统没有录入学生信息，请录入后重试，将自动返回操作菜单" << endl;
        }
        
    }

    void deleteStudent(Student* class1, int& number) {
        
        if (number == 0) {
            cout << "当前系统没有录入学生信息，请录入后重试，将自动返回操作菜单" << endl;
        }

    }

    void simpleMenuControl(Student* class1, int& number,System course) {
        
        while(true) {
            cout << "请输入数字选择对应操作：" << endl;
            cout << "按键输入{0} 添加学生信息" << endl;
            cout << "按键输入{1} 显示学生成绩信息" << endl;
            cout << "按键输入{2} 统计学生成绩信息" << endl;
            cout << "按键输入{3} 查询学生成绩信息" << endl;
            cout << "按键输入{4} 修改学生信息" << endl;
            cout << "按键输入{5} 删除学生信息" << endl;
            cout << "按键输入{6} 退出系统（注意：系统未完善，退出后将丢失信息）" << endl;

            int choice;
            cin >> choice;

            if(choice > 6 || choice <0) {
                cout << "错误：无效操作符：‘" << choice <<"’，请重新输入" <<endl;
                cin >> choice;
            }

            switch(choice) {
                case 0:
                    course.newStudent(class1 , number);
                    break;
                case 1:
                    course.showStudent(class1 , number);
                    break;
                case 2:
                    course.statisticsStudent(class1 , number);
                    break;
                case 3:
                    course.inquireStudent(class1 , number);
                    break;
                case 4:
                    course.modifStudent(class1 , number);
                    break;
                case 5:
                    course.deleteStudent(class1 , number);
                    break;
                case 6:
                    return ;
                default:
                    cout << "错误：未实现的操作符" << endl;
                    break;
            }
        }
    }

};


int main() {

    System course;
    int number = 0;
    cout << "欢迎使用学生成绩管理系统" << endl;
    cout << "请输入你们班级多少人数：" << endl;
    do{
        cin >> course.count;
        if (course.count <= 0) cout << "错误：无效的班级人数，请重新输入你们班级多少人数：" << endl;
    }while(course.count <= 0);
    

    Student Class1[course.count];

    cout << "已经成功录入该班级" << endl;
//调用函数栈建立时，编译器会自动初始化引用，这里实际是两部 隐含了一步自动初始 int& number = number
    course.simpleMenuControl(Class1 , number , course);

    cout << "已成功登出系统" << endl;

    return 0;
}

/*
预计复杂版内容
1.排序按照输入顺序
2.每次功能提示对比上次表内容新增部分
3.无中文
4.优化返回选项，可以有多个选项比如跳转更新和查询
*/