#ifndef SHARED_PTR_H
#define SHARED_PTR_H


class ControlBlock{
public:
    int ref_count;

    ControlBlock() : ref_count(1) {}
};


template <typename T>
class RefCountedPtr{
    private:
    T* ptr;
    ControlBlock* control;

    void release();

    public:
    RefCountedPtr() : ptr(nullptr), control(nullptr) {}
    explicit RefCountedPtr(T *p);
    RefCountedPtr(const RefCountedPtr &p);
    RefCountedPtr& operator=(const RefCountedPtr& p);
    RefCountedPtr(RefCountedPtr &&p) noexcept;
    RefCountedPtr& operator=(RefCountedPtr&& p) noexcept; 
    ~RefCountedPtr();

    T* operator->() const;
    T& operator *() const;

    T* get() const;
    void reset(T* p = nullptr);
};


#endif