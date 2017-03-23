#ifndef  _EPOLL_UTIL_H_
#define _EPOLL_UTIL_H_

#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

namespace util {

void epoll_removefd(int epollfd, int fd);
void epoll_modfd(int epollfd, int fd, int ev,bool oneShot);
void epoll_addfd(int epollfd, int fd, bool oneShot);

}  // util

#endif
