#include <iostream>
#include <thread>
#include <vector>
#include "lock_queue.h"

using namespace x_http_server;

void consumer(threadsafe_queue<int>& q, int size) {
	for (int i = 0; i < size; ++i) {
		std::shared_ptr<int> value = q.wait_and_pop();
		// std::cout << "Consumer fetched" << *value << std::endl;
	}
}

void producer(threadsafe_queue<int>& q, int size) {
	for (int i = 0; i < size; ++i) {
		q.push(i);
		// std::cout << "Produced produced" << i << std::endl;
	}
}

int main(int argc, const char** argv) {
	static const int BUFFER_SIZE = 100;
    static const int C_Num = 10;
    static const int P_Num = 10;
	threadsafe_queue<int> q;
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
