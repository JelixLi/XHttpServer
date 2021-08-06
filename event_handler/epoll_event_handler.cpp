#include <algorithm>
#include "event_handler.h"
#include "Status.h"
#include "epoll_util.h"


namespace x_http_server {

#define FD_SIZE 1000

EPollEventHandler::EPollEventHandler(int event_size,int timeout_msec)
                :event_size_(event_size),timeout_msec_(timeout_msec) {}

Status EPollEventHandler::InitEventHandler() {
    events_.resize(event_size_);
    maxevents_ = 0;
    revents_ = 0;
    cur_event_ = 0;
    epoll_fd_ = epoll_create(FD_SIZE);
    if(epoll_fd_ < 0) return STATUS_ERROR;
    return STATUS_OK;
}

Status EPollEventHandler::AddReadEvent(int fd) {
    if(maxevents_ >= event_size_) return STATUS_ERROR;
    if(epoll_add_event(epoll_fd_,fd,EPOLLIN) < 0)
        return STATUS_ERROR;
    maxevents_++;
    return STATUS_OK;
}

Status EPollEventHandler::AddWriteEvent(int fd) {
    if(maxevents_ >= event_size_) return STATUS_ERROR;
    if(epoll_add_event(epoll_fd_,fd,EPOLLOUT) < 0)
        return STATUS_ERROR;
    maxevents_++;
    return STATUS_OK;
}

Status EPollEventHandler::CloseReadEvent(int fd) {
    if(maxevents_ <= 0) return STATUS_ERROR;
    if(epoll_delete_event(epoll_fd_,fd,EPOLLIN) < 0)
        return STATUS_ERROR;
    maxevents_--;
    return STATUS_OK;
}

Status EPollEventHandler::CloseWriteEvent(int fd) {
    if(maxevents_ <= 0) return STATUS_ERROR;
    if(epoll_delete_event(epoll_fd_,fd,EPOLLOUT) < 0)
        return STATUS_ERROR;
    maxevents_--;
    return STATUS_ERROR;
}

bool EPollEventHandler::CanRead(Event &ev) {
    return (ev.ep_event.events & EPOLLIN);
}

bool EPollEventHandler::CanWrite(Event &ev) {
    return (ev.ep_event.events & EPOLLOUT);
}

Status EPollEventHandler::WaitForEvent() {
    if(maxevents_ > 0) {
        revents_ = epoll_wait(epoll_fd_,events_.data(),maxevents_,timeout_msec_);
        if(revents_ >= 0) return STATUS_OK;
    }
    return STATUS_ERROR;
}

bool EPollEventHandler::GetNextReadyEvent(Event &ev) {
    if(revents_ <= 0 || cur_event_ >= revents_) {
        cur_event_ = 0;
        revents_ = 0;
        return false;
    }
    ev.fd = events_[cur_event_].data.fd;
    ev.ep_event = events_[cur_event_];
    cur_event_++;
    return true;
}

EPollEventHandler::~EPollEventHandler() {
    close(epoll_fd_);
}

}