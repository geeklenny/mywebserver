// 源码地址https://github.com/lishuhuakai/Spweb
// 自己敲一遍练练手


#include "wrapperfun.h"
#include "epoll_util.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "signal.h"
#include "http_handler.h"
#include "thread_pool.h"
#include <iostream>

using namespace std;

using namespace util;

const int MAXEVENTNUM = 1000;

void blockSig(int signo)
{
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, signo);
    int sigfd = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if(sigfd !=0)
        unix_error("block sig error");
}

int main(int argc, char *argv[])
{
    int eventnum, sockfd, connfd;
    int listenfd = Open_listenfd(8080);

    epoll_event events[MAXEVENTNUM];
    sockaddr clntaddr;
    socklen_t clntaddrlen = sizeof(clntaddr);

    blockSig(SIGPIPE);

    int epollfd = Epoll_create( 1024 );
    epoll_addfd(epollfd, listenfd, false);

    ThreadPool pool(10, 30000);

    HttpHandler::setEpoll(epollfd);
    HttpHandler httpHandler[2000];





    while(true)
    {
        eventnum = Epoll_wait( epollfd, events, MAXEVENTNUM, -1 );
        for(int i = 0 ; i < eventnum; i++)
        {
            cout << "." << endl;
            sockfd = events[i].data.fd;
            if(sockfd == listenfd)
            {
                while(true)
                {
                    cout << "0" << endl;
                    connfd = accept(listenfd, &clntaddr, &clntaddrlen);
                    if(connfd == -1)
                    {
                        if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                            break;
                        }
                        unix_error("accept error");
                    }
                    cout << "connfd: " << connfd << endl;
                    httpHandler[connfd].init(connfd);
                    epoll_addfd(epollfd, connfd, true);
                    cout << "1" << endl;
                }
            }
            else
            {
                cout << "2" << endl;
                pool.append(std::bind(&HttpHandler::process, &httpHandler[sockfd]));
            }
        }
    }


    return 0;
}

