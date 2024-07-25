#include <iostream>

template<class T>
class SharePtr {
public:
    SharePtr() : ptr_(nullptr), ref_count_(nullptr) {}
    ~SharePtr() { release(); }
    SharePtr(T* ptr) : ptr_(ptr), ref_count_(new int(1)) {}
    SharePtr(const SharePtr<T>& other) {
        ptr_ = other.ptr_;
        ref_count_ = other.ref_count_;
        ++(*ref_count_);
    }
    SharePtr(const SharePtr<T>&& other) {
        ptr_ = other.ptr_;
        ref_count_ = other.ref_count_;
        ptr_ = nullptr;
        ref_count_ = nullptr;
    }
    SharePtr<T>& operator=(const SharePtr<T>& other) {
        if(this != other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            ++(*ref_count_);
        }
        return *this;
    }

    SharePtr<T>& operator=(const SharePtr<T>&& other) {
        if(this != other) {
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            other.ptr_ = nullptr;
            other.ref_count_ = nullptr;
        }
    }

    void reset(T* p = nullptr) {
        release();
        if(p) {
            ptr_ = p;
            *ref_count_ = 1;
        } else {
            ptr_ = nullptr;
            ref_count_ = nullptr;
        }
    }

    T& operator*(){
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

    int use_count() {
        if(ref_count_) return *ref_count_;
        return -1;
    }

private:
    void release() {
        if(--(*ref_count_) == 0) {
            delete ptr_;
            delete ref_count_;
        }
    }
    int *ref_count_;
    T* ptr_;
};

int main() {
    SharePtr<int> ptr1(new int(5));
    SharePtr<int> ptr2 = ptr1;

    std::cout << "ptr1 use count: " << ptr1.use_count() << std::endl;
    std::cout << "ptr2 use count: " << ptr2.use_count() << std::endl;

    return 0;
}