#include <iostream>
#include <sys/types.h>

template<class T>
class UniquePtr{
public:
    UniquePtr():ptr_(nullptr) {}
    UniquePtr(T* ptr) : ptr_(ptr) {}
    ~UniquePtr() {
        delete ptr_;
    }

    // 拷贝构造和赋值构造都要禁用
    UniquePtr(const UniquePtr<T>& other) = delete;
    UniquePtr<T>& operator=(const UniquePtr<T>& other) = delete;

    UniquePtr(UniquePtr<T>&& other) {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }
    UniquePtr<T>& operator=(UniquePtr<T>&& other) {
        if(this != other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

int main() {
    UniquePtr<int> ptr(new int(36));
    std::cout << *ptr << std::endl;

    UniquePtr<int> ptr2(std::move(ptr));
    std::cout << (ptr ? "ptr is valid" : "ptr is nullptr") << std::endl;
    std::cout << *ptr2 << std::endl;

    return 0;
}