#include "3_shared_ptr.h"
#include <iostream>

template <typename T>
void RefCountedPtr<T>::release() {
    
    if (control) {
        --control->ref_count;
        if (control->ref_count == 0) {

            delete ptr;
            delete control;
        }
    }
}

// RefCountedPtr a(new int(1))
template <typename T>
RefCountedPtr<T>::RefCountedPtr(T *p) : ptr(p) {

    if (ptr) {

        control = new ControlBlock();
    } else {

        ptr = nullptr;
    }
}

//RefCountedPtr a(b)
template <typename T>
RefCountedPtr<T>::RefCountedPtr(const RefCountedPtr &p) : ptr(p.ptr), control(p.control) {
    
    if (control) {

        ++control->ref_count;
    }
}

// RefCountedPtr a = b
template <typename T>
RefCountedPtr<T>& RefCountedPtr<T>::operator=(const RefCountedPtr& p){
    
    if (this != &p) {

        release();

        ptr = p.ptr;
        control = p .control;

        if(control) {     //control检测防止空指针

            ++control->ref_count;
        }
    }
    return *this;
}

// RefCountedPtr a (std::move(b))
template <typename T>
RefCountedPtr<T>::RefCountedPtr(RefCountedPtr &&p) noexcept : ptr(p.ptr), control(p.control) {
    
    p.ptr = nullptr;
    p.control = nullptr;
}

// RefCountedPtr a = std::move(b)
template <typename T>
RefCountedPtr<T>& RefCountedPtr<T>::operator=(RefCountedPtr &&p) noexcept {
    
    if (this != &p) {
        release();
        ptr = p.ptr;
        control = p.control;
        p.ptr = nullptr;
        p.control = nullptr;
    }
    return *this;
}

// p-> (new int5)
template <typename T>
T* RefCountedPtr<T>::operator->() const{
    
    return ptr;
}

// p-> *(new int5)
template <typename T>
T& RefCountedPtr<T>::operator*() const{
    
    return *ptr;
}

template <typename T>
T* RefCountedPtr<T>::get() const {
    
    return ptr;
}

template <typename T>
void RefCountedPtr<T>::reset(T* p) {
    
    release();
    ptr = p;
    control = p ? new ControlBlock() : nullptr;
}

//~Ref
template <typename T>
RefCountedPtr<T>::~RefCountedPtr(){

    release();
}

int main() {
    
    // 测试构造函数和get()
    RefCountedPtr<int> ptr1(new int(42));
    std::cout << "Value: " << *ptr1 << ", Raw pointer: " << ptr1.get() << std::endl;

    // 测试拷贝构造
    RefCountedPtr<int> ptr2 = ptr1;
    std::cout << "Ref count after copy: " << std::endl; // 注意：这里需要添加获取引用计数的方法才能实际查看

    // 测试移动构造
    RefCountedPtr<int> ptr3 = std::move(ptr1);
    std::cout << "After move, ptr1 is: " << (ptr1.get() ? "not null" : "null") << std::endl;

    // 测试reset
    ptr2.reset(new int(100));
    std::cout << "After reset, value: " << *ptr2 << std::endl;

    // 测试reset with nullptr
    ptr2.reset();
    std::cout << "After reset nullptr, ptr2 is: " << (ptr2.get() ? "not null" : "null") << std::endl;

    return 0;
}





