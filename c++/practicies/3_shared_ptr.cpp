#include "3_shared_ptr.h"

template <typename T>
void RefCountedPtr<T>::release() {
    --control->ref_count;
    if (!control->ref_count) {

        delete ptr;
        delete control;
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

        ++control.ref_count;
    }
}

// RefCountedPtr a = b
template <typename T>
RefCountedPtr<T>& RefCountedPtr<T>::operator=(const RefCountedPtr&& p){
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





