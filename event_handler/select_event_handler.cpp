#include <algorithm>
#include "event_handler.h"
#include "Status.h"

namespace x_http_server {


SelectEventHandler::SelectEventHandler(int event_size,int timeout_sec,int timeout_usec)
    :event_size_(event_size),timeout_sec_(timeout_sec),timeout_usec_(timeout_usec) {}

Status SelectEventHandler::InitEventHandler() {
    FD_ZERO(&readfds_);
    FD_ZERO(&writefds_);
    conn_fds_.resize(event_size_,-1);
    tv_.tv_sec = timeout_sec_;
    tv_.tv_usec = timeout_usec_;
    max_sock_fd_ = -1;
    return STATUS_OK;
}

Status SelectEventHandler::WaitForEvent() {
    if(max_sock_fd_ >= 0) {
        if(select(max_sock_fd_+1,&readfds_,&writefds_,NULL,&tv_) >= 0) {
            return STATUS_OK;
        }
    }
    return STATUS_ERROR;
}

bool SelectEventHandler::GetNextReadyEvent(Event &ev) {
    auto is_set = [&](int conn_fd){
        return FD_ISSET(conn_fd,&readfds_) || FD_ISSET(conn_fd,&writefds_);
    };
    // maybe can be optimized here
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_set);
    if(iter != conn_fds_.end()) {
        ev.fd = *iter;
        return true;
    }
    return false;
}

bool SelectEventHandler::CanRead(Event &ev) {
    return FD_ISSET(ev.fd,&readfds_);
}

bool SelectEventHandler::CanWrite(Event &ev) {
    return FD_ISSET(ev.fd,&writefds_);
}

Status SelectEventHandler::AddReadEvent(int fd) {
    auto iter = std::find(conn_fds_.begin(),conn_fds_.end(),fd);
    if(iter == conn_fds_.end()) {
        iter = std::find(conn_fds_.begin(),conn_fds_.end(),-1);
        if(iter != conn_fds_.end()) {
            *iter = fd;
        } else {
            return STATUS_ERROR;
        }
    }
    FD_SET(fd,&readfds_);
    if(fd > max_sock_fd_) max_sock_fd_ = fd;
    return STATUS_OK;
}

Status SelectEventHandler::AddWriteEvent(int fd) {
    auto iter = std::find(conn_fds_.begin(),conn_fds_.end(),fd);
    if(iter == conn_fds_.end()) {
        iter = std::find(conn_fds_.begin(),conn_fds_.end(),-1);
        if(iter != conn_fds_.end()) {
            *iter = fd;
        } else {
            return STATUS_ERROR;
        }
    }
    FD_SET(fd,&writefds_);
    if(fd > max_sock_fd_) max_sock_fd_ = fd;
    return STATUS_OK;
}

Status SelectEventHandler::CloseReadEvent(int fd) {
    auto iter = std::find(conn_fds_.begin(),conn_fds_.end(),fd);
    if(iter != conn_fds_.end()) {
        FD_CLR(fd,&readfds_);
        if(!FD_ISSET(fd,&writefds_)) {
            *iter = -1;
            if(fd == max_sock_fd_) {
                max_sock_fd_ = *std::max_element(conn_fds_.begin(),conn_fds_.end());
            }
        }
        return STATUS_OK;
    }
    return STATUS_ERROR;
}

Status SelectEventHandler::CloseWriteEvent(int fd) {
    auto iter = std::find(conn_fds_.begin(),conn_fds_.end(),fd);
    if(iter != conn_fds_.end()) {
        FD_CLR(fd,&writefds_);
        if(!FD_ISSET(fd,&readfds_)) {
            *iter = -1;
            if(fd == max_sock_fd_) {
                max_sock_fd_ = *std::max_element(conn_fds_.begin(),conn_fds_.end());
            }
        }
        return STATUS_OK;
    }
    return STATUS_ERROR;
}


}