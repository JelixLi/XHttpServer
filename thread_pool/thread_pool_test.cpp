#include <iostream>
#include <cstdio>
#include "thread_pool.h"

int main() {
    x_http_server::ThreadPool pool(4);
    for(int i=0;i<10;i++) {
        pool.Schedule([=](){
            printf("thread id: %d\n",i);
        });
    }
}