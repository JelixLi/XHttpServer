#ifndef X_HTTP_SERVER_EPOLL_UTIL_H_
#define X_HTTP_SERVER_EPOLL_UTIL_H_

namespace x_http_server {

void epoll_add_event(int epollfd,int fd,int state);

void epoll_delete_event(int epollfd,int fd,int state);

void epoll_modify_event(int epollfd,int fd,int state);

}
#endif 