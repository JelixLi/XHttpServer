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
#include "Status.h"

namespace x_http_server {


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
    virtual bool CanRead(int fd) = 0;
    virtual bool CanWrite(int fd) = 0;

    virtual bool GetNextReadyEvent(int &fd) = 0;
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
    bool CanRead(int fd);
    bool CanWrite(int fd);

    bool GetNextReadyEvent(int &fd);

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

}

#endif