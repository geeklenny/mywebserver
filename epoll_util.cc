#include "epoll_util.h"
#include "sys/types.h"
#include "sys/fcntl.h"

namespace util {

void epoll_removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void epoll_modfd(int epollfd, int fd, int ev,bool oneshot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET;
    if(oneshot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);


}

int setnonblocking(int fd) /* 将文件描述符设置为非阻塞 */
{
		int old_option = fcntl(fd, F_GETFL);
		int new_option = old_option | O_NONBLOCK;
		fcntl(fd, F_SETFL, new_option);
		return old_option;
}

void epoll_addfd(int epollfd, int fd, bool oneshot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET; //ET mode
    if(oneshot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

}  // util
