#ifndef _HTTP_HANDLER_H_
#define _HTTP_HANDLER_H_

#include <stdlib.h>
#include "http_request.h"
#include "cache.h"
#include "buff.h"

class Buffer;
class HttpRequest;
class Cache;
class FileInfo;
class HttpHandler
{
  public:
    void init(int fd);
    void process();
    static void setEpoll(int epollfd){ m_epollfd = epollfd; }

  private:
    void processRead();
    void processWrite();
    void reset();
    bool read();
    void clientError(const char *, const char *, const char *, const char *);
    void serveStatic(const char* filename, size_t filesize);

  private:
    enum HttpHandleState {
        kExpectReset, /* 需要初始化 */
        kExpectRead, /* 正在处理读 */
        kExpectWrite, /* 正在处理写 */
        kError, /* 出错 */
        kSuccess, /* 成功 */
        kClosed /* 对方关闭连接 */
    } m_status;
    int m_sockfd;
    static int m_epollfd;

    bool m_keepAlive;
    bool m_sendFile;
    size_t m_fileWritten;

    HttpRequest m_request;

    Buffer m_readBuffer;
    Buffer m_writeBuffer;

    static Cache& m_cache;
    pInfo m_fileInfo;



};


#endif
