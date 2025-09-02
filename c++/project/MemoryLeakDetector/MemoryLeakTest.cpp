#include "MemoryLeakDetector.h"
#include <vector>
#include <thread>


#define new new(__FILE__, __LINE__)


// 测试用例1: 基本内存泄漏检测
void test_basic_leak() {
    
    // 分配但不释放
    int* leak_int = new int(42);
    std::string* leak_string = new std::string("Hello, Leak!");
    
    // 分配并释放（不应报告为泄漏）4
    int* non_leak = new int(100);

    delete non_leak;
}
#if 0

// 测试用例2: 数组内存泄漏
void test_array_leak() {
    
    // 泄漏数组
    int* leak_array = new int[10];
    char* leak_char_array = new char[100];
    
    // 正常数组操作
    int* non_leak_array = new int[5];
    delete[] non_leak_array;
}

// 测试用例3: 类对象内存泄漏
class TestClass {
public:
    TestClass() : data(new int[10]) {}
    ~TestClass() { delete[] data; }
private:
    int* data;
};

void test_class_leak() {

    // 泄漏类对象
    TestClass* leak_obj = new TestClass();
    
    // 正常类对象
    TestClass* non_leak_obj = new TestClass();
    delete non_leak_obj;
}

// 测试用例4: 嵌套分配泄漏
void test_nested_allocation() {
    
    // 嵌套结构
    struct Node {
        int value;
        Node* next;
    };
    
    // 创建泄漏链表
    Node* head = new Node{1, new Node{2, new Node{3, nullptr}}};
    
    // 正常链表（会被正确释放）
    Node* valid_head = new Node{10, new Node{20, nullptr}};
    delete valid_head->next;
    delete valid_head;
}

// 测试用例5: 多线程环境下的内存泄漏
void thread_func(int id) {
    for (int i = 0; i < 10; ++i) {
        int* data = new int(i);
        if (id % 2 == 0) {
            // 偶数线程会泄漏
            // 不释放data
        } else {
            // 奇数线程正确释放
            delete data;
        }
    }
}

void test_multithreaded_leak() {
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(thread_func, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// 测试用例6: 混合场景测试
void test_mixed_scenario() {
    
    // 各种类型的内存分配
    int* int_val = new int(42);
    double* double_val = new double(3.14);
    char* char_array = new char[100];
    std::string* string_val = new std::string("test");
    
    // 释放部分内存
    delete int_val;
    delete double_val;
    // 故意不释放char_array和string_val
}
#endif

// 主测试函数
int main() {

    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 内存泄漏检测工具测试 ===" << std::endl;
    
    std::cout << "测试1: 基本内存泄漏检测" << std::endl;
    test_basic_leak();
    std::cout << "=======================";

    #if 0
    std::cout << "\n测试2: 数组内存泄漏\n";
    test_array_leak();
    
    std::cout << "\n测试3: 类对象内存泄漏\n";
    test_class_leak();
    
    std::cout << "\n测试4: 嵌套分配泄漏\n";
    test_nested_allocation();
    
    std::cout << "\n测试5: 多线程内存泄漏\n";
    test_multithreaded_leak();
    
    std::cout << "\n测试6: 混合场景测试\n";
    test_mixed_scenario();
    
    std::cout << "\n=== 所有测试完成 ===\n";
    #endif

    std::cout << "leak: " << MemoryLeakDetector::getInstance().reportLeak() << std::endl;
}