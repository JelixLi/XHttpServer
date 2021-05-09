#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include "../queue/lock_queue.h"

namespace x_http_server {

class ThreadPool {
public:
    explicit ThreadPool(int num_threads) {
        for(int i=0;i<num_threads;i++) {
            threads_.push_back(std::thread(&ThreadPool::WorkLoop,this));
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool() {
        for(int i=0;i<threads_.size();i++) {
            queue_.push(nullptr);
        }
        for(auto& t : threads_) {
            t.join();
        }
    }

    void Schedule(std::function<void()> func) {
        queue_.push(std::move(func));
    }

private:

    void WorkLoop() {
        while(true) {
            auto func = std::move(queue_.wait_and_pop());
            if(*func==nullptr) break;
            else {
                (*func)();
            }
        }
    }

    std::vector<std::thread> threads_;
    threadsafe_queue<std::function<void()>> queue_;
};

} // namespace x_http_server



#endif 