#include <algorithm>
#include "event_handler.h"
#include "Status.h"

namespace x_http_server {

PollEventHandler::PollEventHandler(int event_size,int timeout_msec)
                :event_size_(event_size),timeout_msec_(timeout_msec){}


Status PollEventHandler::InitEventHandler() {
    conn_fds_.resize(event_size_);
    max_index_ = -1;
    return STATUS_OK;
}

Status PollEventHandler::WaitForEvent() {
    if(max_index_ >= 0) {
        if(poll(conn_fds_.data(),max_index_+1,timeout_msec_) >= 0) {
            return STATUS_OK;
        }
    }
    return STATUS_ERROR;
}


Status PollEventHandler::AddReadEvent(int fd) {
    auto is_fd = [&](struct pollfd& poll_fd){
        return poll_fd.fd == fd;
    };
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_fd);
    if(iter == conn_fds_.end()) {
        auto is_empty = [&](struct pollfd& poll_fd){
            return poll_fd.fd == -1;
        };
        iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_empty);
    }
    if(iter != conn_fds_.end()) {
        iter->fd = fd;
        iter->events |= POLLIN;
        if(iter - conn_fds_.begin() > max_index_) max_index_ = iter - conn_fds_.begin();
        return STATUS_OK;
    }
    return STATUS_ERROR;
}

Status PollEventHandler::AddWriteEvent(int fd) {
    auto is_fd = [&](struct pollfd& poll_fd){
        return poll_fd.fd == fd;
    };
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_fd);
    if(iter == conn_fds_.end()) {
        auto is_empty = [&](struct pollfd& poll_fd){
            return poll_fd.fd == -1;
        };
        iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_empty);
    }
    if(iter != conn_fds_.end()) {
        iter->fd = fd;
        iter->events |= POLLOUT;
        if(iter - conn_fds_.begin() > max_index_) max_index_ = iter - conn_fds_.begin();
        return STATUS_OK;
    }
    return STATUS_ERROR;
}

Status PollEventHandler::CloseReadEvent(int fd) {
    auto is_fd = [&](struct pollfd& poll_fd){
        return (poll_fd.fd == fd) && (poll_fd.events & POLLIN);
    };
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_fd);
    if(iter != conn_fds_.end()) {
        iter->events ^= POLLIN;
        if(iter->events == 0) {
            iter->fd = -1;
            if(iter - conn_fds_.begin() == max_index_) max_index_--;
        }
        iter->revents = 0;
        return STATUS_OK;
    }
    return STATUS_ERROR;
}

Status PollEventHandler::CloseWriteEvent(int fd) {
    auto is_fd = [&](struct pollfd& poll_fd){
        return (poll_fd.fd == fd) && (poll_fd.events & POLLOUT);
    };
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_fd);
    if(iter != conn_fds_.end()) {
        iter->events ^= POLLOUT;
        if(iter->events == 0) {
            iter->fd = -1;
            if(iter - conn_fds_.begin() == max_index_) max_index_--;
        }
        iter->revents = 0;
        return STATUS_OK;
    }
    return STATUS_ERROR;
}

bool PollEventHandler::CanRead(Event &ev) {
    return ev.poll_event.revents & POLLIN;
}

bool PollEventHandler::CanWrite(Event &ev) {
    return ev.poll_event.revents & POLLOUT;
}

bool PollEventHandler::GetNextReadyEvent(Event &ev) {
    auto is_set = [&](struct pollfd& poll_fd){
        return (poll_fd.fd != -1) && ((poll_fd.revents & POLLIN) || (poll_fd.revents & POLLOUT));
    };
    auto iter = std::find_if(conn_fds_.begin(),conn_fds_.end(),is_set);
    if(iter != conn_fds_.end()) {
        ev.fd = iter->fd;
        ev.poll_event = *iter;
        return true;
    }
    return false;
}


}