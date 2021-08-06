#include "Status.h"
#include "epoll_util.h"

namespace x_http_server {

Status epoll_add_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev) < 0) 
        return STATUS_ERROR;
    return STATUS_OK;
}

Status epoll_delete_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if(epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev) < 0)
        return STATUS_ERROR;
    return STATUS_OK;
}

Status epoll_modify_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if(epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev) < 0) 
        return STATUS_ERROR;
    return STATUS_OK;
}

}