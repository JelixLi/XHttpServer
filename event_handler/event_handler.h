#ifndef X_HTTP_SERVER_EVENT_HANDLER_H_
#define X_HTTP_SERVER_EVENT_HANDLER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <poll.h>
#include <sys/epoll.h>
#include "Status.h"

namespace x_http_server {

struct Event {
    int fd;
    struct pollfd poll_event; 
    struct epoll_event ep_event;
};


class EventHandler {
public:
    EventHandler() = default;

    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;

    virtual Status InitEventHandler() = 0;       

    virtual Status AddReadEvent(int fd) = 0;
    virtual Status AddWriteEvent(int fd) = 0;

    virtual Status CloseReadEvent(int fd) = 0;
    virtual Status CloseWriteEvent(int fd) = 0;

    virtual Status WaitForEvent() = 0;
    virtual bool CanRead(Event &ev) = 0;
    virtual bool CanWrite(Event &ev) = 0;

    virtual bool GetNextReadyEvent(Event &ev) = 0;

    virtual ~EventHandler() {}
};

class SelectEventHandler : public EventHandler {
public:
    SelectEventHandler(int event_size,int timeout_sec,int timeout_usec);

    SelectEventHandler(const SelectEventHandler&) = delete;
    SelectEventHandler& operator=(const SelectEventHandler&) = delete;

    Status InitEventHandler();

    Status AddReadEvent(int fd);
    Status AddWriteEvent(int fd);

    Status CloseReadEvent(int fd);
    Status CloseWriteEvent(int fd);

    Status WaitForEvent();
    bool CanRead(Event &ev);
    bool CanWrite(Event &ev);

    bool GetNextReadyEvent(Event &ev);

private:
    int event_size_;
    int timeout_sec_;
    int timeout_usec_;

    fd_set readfds_;
    fd_set writefds_;

    std::vector<int> conn_fds_; 

    int max_sock_fd_;

    struct timeval tv_;
};

class PollEventHandler : public EventHandler {
public:
    PollEventHandler(int event_size,int timeout_msec);

    PollEventHandler(const PollEventHandler&) = delete;
    PollEventHandler& operator=(const PollEventHandler&) = delete;

    Status InitEventHandler();

    Status AddReadEvent(int fd);
    Status AddWriteEvent(int fd);

    Status CloseReadEvent(int fd);
    Status CloseWriteEvent(int fd);

    Status WaitForEvent();
    bool CanRead(Event &ev);
    bool CanWrite(Event &ev);

    bool GetNextReadyEvent(Event &ev);

private:
    int event_size_;
    int timeout_msec_;

    std::vector<struct pollfd> conn_fds_; 

    int max_index_;   
};

class EPollEventHandler : public EventHandler {
public:
    EPollEventHandler(int event_size,int timeout_msec);

    EPollEventHandler(const EPollEventHandler&) = delete;
    EPollEventHandler& operator=(const EPollEventHandler&) = delete;

    Status InitEventHandler();

    Status AddReadEvent(int fd);
    Status AddWriteEvent(int fd);

    Status CloseReadEvent(int fd);
    Status CloseWriteEvent(int fd);

    Status WaitForEvent();
    bool CanRead(Event &ev);
    bool CanWrite(Event &ev);

    bool GetNextReadyEvent(Event &ev);

    ~EPollEventHandler();

private:
    int event_size_;
    int timeout_msec_;

    std::vector<struct epoll_event> events_;

    int maxevents_;

    int epoll_fd_;

    int revents_;

    int cur_event_;
};

}

#endif