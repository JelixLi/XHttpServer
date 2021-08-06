#ifndef X_HTTP_SERVER_EPOLL_UTIL_H_
#define X_HTTP_SERVER_EPOLL_UTIL_H_

#include <sys/epoll.h>
#include "Status.h"

namespace x_http_server {

Status epoll_add_event(int epollfd,int fd,int state);

Status epoll_delete_event(int epollfd,int fd,int state);

Status epoll_modify_event(int epollfd,int fd,int state);

}
#endif 