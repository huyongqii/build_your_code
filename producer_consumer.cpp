#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mutex;
std::condition_variable not_full;
std::condition_variable not_empty;
const int MAX_SIZE = 50;
std::queue<int> buffer;

void producer(int id) {
    int item = 0;
    while(true) {
        std::unique_lock<std::mutex> lock(mutex);
        not_full.wait(lock, [](){
            return buffer.size() < MAX_SIZE;
        });
        buffer.push(item);
        std::cout << "生产者：" << id << ", 生产了 item" << item++ << std::endl;
        not_empty.notify_all();
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(int id) {
    while(true) {
        std::unique_lock<std::mutex> lock(mutex);
        not_empty.wait(lock, [](){
            return !buffer.empty();
        });
        int item = buffer.front();
        buffer.pop();
        std::cout << "消费者：" << id << ", 消费了 item" << item << std::endl;
        not_full.notify_all();
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

int main() {
    std::thread producers[3];
    std::thread consumers[2];

    for(int i = 0; i < 3; i++) {
        producers[i] = std::thread(producer, i + 1);
    }

    for(int i = 0; i < 2; i++) {
        consumers[i] = std::thread(consumer, i + 1);
    }
    for(int i = 0; i < 3; i++) {
        producers[i].join();
    }

    for(int i = 0; i < 2; i++) {
        consumers[i].join();
    }
    return 0;
}