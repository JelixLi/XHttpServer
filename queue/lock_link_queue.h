#ifndef X_HTTP_SERVER_LOCK_LINK_QUEUE_H_
#define X_HTTP_SERVER_LOCK_LINK_QUEUE_H_

#include <condition_variable>  
#include <mutex>
#include "Status.h"

namespace x_http_server {
    
template<typename T>
struct LinkNode {
    T item; //assumed to be a shared_ptr<T>
    LinkNode<T> *next;
};

//Locked FIFO Queue
template<class T>   
class LockLinkQueue {
public:
    LockLinkQueue(int queue_size);
    ~LockLinkQueue();
    Status push(T item);
    T pop();
    int queueSize();
    int queueCapacity();
private:
    LinkNode<T> *_link_queue_head;
    LinkNode<T> *_link_queue_rear;
    int _queue_size;
    int _queue_capacity;
    std::condition_variable _cond;
    std::mutex _mu;
};

//create bidirectional circular link list with queue_size
template<typename T>
LockLinkQueue<T>::LockLinkQueue(int queue_capacity) {
    _link_queue_head = new LinkNode<T>;
    LinkNode<T> *p = _link_queue_head;
    for(int i=0;i<queue_capacity;i++) {
        p->next = new LinkNode<T>;
        p = p->next;
    }
    p->next = _link_queue_head;
    _link_queue_rear = _link_queue_head;
    _queue_capacity = queue_capacity;
    _queue_size = 0;
}

template<typename T>
LockLinkQueue<T>::~LockLinkQueue() {
    LinkNode<T> *p = _link_queue_head->next;
    LinkNode<T> *r;
    while(p!=_link_queue_head) {
        r = p->next;
        delete p;
        p = r;
    }
    delete _link_queue_head;
}


template<typename T>
Status LockLinkQueue<T>::push(T item) {
    std::lock_guard<std::mutex> lk(_mu);
    //push something
    if(_link_queue_head->next==_link_queue_rear) {
        //queue full add space
        LinkNode<T> *p = new LinkNode<T>;
        _link_queue_head->next = p;
        p->next = _link_queue_rear;
        _queue_capacity++;
    } 
    _link_queue_head->item = item;
    _link_queue_head = _link_queue_head->next;
    _queue_size++;
    _cond.notify_one();
    return STATUS_OK;
}

template<typename T>
T LockLinkQueue<T>::pop() {
    std::unique_lock<std::mutex> lk(_mu);
    _cond.wait(lk,[this]{return this->_link_queue_head!=this->_link_queue_rear;});
    //pop something
    T ret = _link_queue_rear->item;
    _link_queue_rear = _link_queue_rear->next;
    _queue_size--;
    return ret;
}

template<typename T>
int LockLinkQueue<T>::queueSize() {
    std::lock_guard<std::mutex> lk(_mu);
    return _queue_size;
}

template<typename T>
int LockLinkQueue<T>::queueCapacity() {
    std::lock_guard<std::mutex> lk(_mu);
    return _queue_capacity;
}
} // namespace x_http_server

#endif