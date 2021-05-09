#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "lock_link_queue.h"

using namespace x_http_server;

void basic_test() {
    LockLinkQueue<std::shared_ptr<int>> lq(2);
    int a[10];
    for(int i=0;i<10;i++) {
        a[i] = i;
        lq.push(std::make_shared<int>(a[i]));
    }
    for(int i=0;i<10;i++) {
        std::shared_ptr<int> pa = lq.pop();
        std::cout<<*pa<<std::endl;
    }
    std::cout<<lq.queueCapacity()<<std::endl;
    std::cout<<lq.queueSize()<<std::endl;
    lq.pop();
}

void consumer(LockLinkQueue<std::shared_ptr<int>>& q, int size) {
	for (int i = 0; i < size; ++i) {
		std::shared_ptr<int> value = q.pop();
		// std::cout << "Consumer fetched" << *value << std::endl;
	}
}

void producer(LockLinkQueue<std::shared_ptr<int>>& q, int size) {
	for (int i = 0; i < size; ++i) {
		q.push(std::make_shared<int>(i));
		// std::cout << "Produced produced" << i << std::endl;
	}
}

int main(int argc,const char** argv) {
	static const int BUFFER_SIZE = 100;
    static const int C_Num = 10;
    static const int P_Num = 10;
    LockLinkQueue<std::shared_ptr<int>> q(0);
    std::vector<std::thread> cthreads,pthreads;
	auto start = std::chrono::system_clock::now();
    for(int i=0;i<C_Num;i++) {
	    cthreads.push_back(std::thread(consumer, std::ref(q), BUFFER_SIZE));
    }
    for(int i=0;i<P_Num;i++) {
        pthreads.push_back(std::thread(producer, std::ref(q), BUFFER_SIZE));
    }
    for(int i=0;i<P_Num;i++) {
        pthreads[i].join();
    }
    for(int i=0;i<C_Num;i++) {
        cthreads[i].join();
    }
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	return 0;
}

