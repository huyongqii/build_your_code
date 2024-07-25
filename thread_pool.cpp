#include <iostream>
#include <functional>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(int num_thread) {
        for(int i = 0; i < num_thread; i++) {
            works_.emplace_back(std::thread([this](){
                while(true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this](){
                            return stop_ || !tasks_.empty();
                        });
                        // 主动停止并且任务队列为空
                        if(stop_ && tasks_.empty()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }

                    task();
                }
            }));
        }
    }

    // 这里的析构函数很重要，要通知，所有线程终结且要等待线程终结完成
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }

        cv_.notify_all();
        for(auto& work : works_) {
            work.join();
        }
    }

template<class F, class... Args>
void enqueue(F&& f, Args&&... args) {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.emplace(task);
    }
    cv_.notify_one();
}

private:
    std::vector<std::thread> works_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};

int main() {
    ThreadPool pool(5);
    for(int i = 0; i < 10; i++) {
        pool.enqueue([](int i){
            std::cout << "hello " << i << std::endl;
        }, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}