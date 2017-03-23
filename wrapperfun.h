#ifndef _WRAPPER_FUN_H_
#define _WRAPPER_FUN_H_

#include <cstdio>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/mman.h>


namespace util {

using SA = struct sockaddr;

const int LISTENQ = 1024;

void unix_error(const char* msg);

int Open_listenfd(int port);

int Epoll_create(int size);
int Epoll_wait( int __epfd, struct epoll_event *__events, int __maxevents, int __timeout );

/* Memory mapping wrappers */
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);

}  // util

#endif
