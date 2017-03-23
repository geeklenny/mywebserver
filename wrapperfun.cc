#include "wrapperfun.h"

namespace util {

void unix_error(const char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

int open_listenfd(int port)
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        return -1;

    // eliminates "address already in use" error from bind
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, static_cast<const void*>(&optval), sizeof(int)) < 0 )
        return -1;

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);
    if(bind(listenfd, (SA*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    if(listen(listenfd, LISTENQ) < 0 )
        return -1;
 
    return listenfd;
}

int Open_listenfd(int port)
{
    int listenfd;
    if((listenfd = open_listenfd(port)) < 0)
        unix_error("open listenfd error");
    return listenfd;
}

int Epoll_create(int size)
{
    int epollfd;

    if ((epollfd = epoll_create(size)) < 0)
        unix_error("epoll create failed");

    return epollfd;
}

int Epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout)
{
    int eventnum;
    if(( eventnum = epoll_wait(epfd, events, maxevents, timeout)) == -1)
        unix_error("epoll wait failed");
    return eventnum;
}

/***************************************
 * Wrappers for memory mapping functions
 ***************************************/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
		void *ptr;

		if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *)-1))
        unix_error("mmap error");
		return(ptr);
}

void Munmap(void *start, size_t length)
{
		if (munmap(start, length) < 0)
        unix_error("munmap error");
}

}  // util
