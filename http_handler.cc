#include "http_handler.h"
#include "epoll_util.h"
#include <sys/stat.h>
#include "common.h"
#include <string.h>
#include <iostream>

using namespace std;

using namespace util;



int HttpHandler::m_epollfd = -1;
Cache g_cache;
Cache& HttpHandler::m_cache = g_cache;

void HttpHandler::reset()
{
    m_status = kExpectRead;
    m_keepAlive = false;
    m_sendFile = false;
    m_fileWritten = 0;
    m_readBuffer.retrieveAll();
    m_writeBuffer.retrieveAll();
    m_request.reset();
}

void HttpHandler::init( int fd )
{
    m_sockfd = fd;
    reset();
}

bool HttpHandler::read()
{
    int nbytesRead = 0;
    while(true)
    {
        int iErrno = 0;
        cout << "http handler LIne :" << __LINE__ << " " << m_sockfd << endl;
        nbytesRead = m_readBuffer.readFd(m_sockfd, &iErrno);
        if(nbytesRead == -1)
        {
            if((iErrno == EAGAIN) || (iErrno == EWOULDBLOCK ))
                break;
            m_status = kError;
            return false;
        }
        else if (nbytesRead == 0)
        {
            m_status = kClosed;
            return false;
        }
    }
    return true;
}

void HttpHandler::processRead()
{
    struct stat sbuf;

    cout << "http handler LIne :" << __LINE__ << endl;
    if(!read())
    {
        m_status = kClosed;
        return;
    }

    cout << "http handler LIne :" << __LINE__ << endl;
    RequestParseState status = m_request.parseRequest(m_readBuffer);

    if( status == RequestParseState::kError)
    {
        m_status = kError;
        return;
    }
    else if (status != RequestParseState::kGotAll)
    {
        return;
    }

    cout << "http handler LIne :" << __LINE__ << endl;
    if(strcasecmp(m_request.m_method.c_str(), "GET"))
    {
        clientError(m_request.m_method.c_str(), "501", "Not Implemented", "FlashWebserver does not support this method yet!");
        goto end;
    }
    cout << "http handler LIne :" << __LINE__ << endl;                     
    if (m_request.m_static) { /* 只支持静态网页 */
        if (stat(m_request.m_path.c_str(), &sbuf) < 0) {
            clientError(m_request.m_path.c_str(), "404", "Not found",
                        "flashWebServer couldn't find this file"); /* 没有找到文件 */
            goto end;
        }

        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            clientError(m_request.m_path.c_str(), "403", "Forbidden",
                        "Tiny couldn't read the file"); /* 权限不够 */
            goto end;
        }
        serveStatic(m_request.m_path.c_str(), sbuf.st_size);
		
    }
    else { /* Serve dynamic content */
        clientError(m_request.m_method.c_str(), "501", "Not Implemented",
                    "Tiny does not implement this method");
        goto end;
    }
end:
    cout << "http handler LIne :" << __LINE__ << endl;
    m_status = kExpectWrite;
    return processWrite();
}

void HttpHandler::processWrite()
{
	int res;
	size_t nRemain = m_writeBuffer.readableBytes(); /* writeBuf_中还有多少字节要写 */
	if (nRemain > 0) {
		while (true) {
        size_t len = m_writeBuffer.readableBytes();
			res = write(m_sockfd, m_writeBuffer.peek(), len);
			if (res < 0) {
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { /* 资源暂时不可用 */
					return;
				}
				m_status = kError;
				return;
			}
			m_writeBuffer.retrieve(res);
			if (m_writeBuffer.readableBytes() == 0)
          break;
		}
	}

		
	/*-
	* 第2步,要发送html网页数据.
	*/
	if (m_sendFile) {
		char *fileAddr = (char *)m_fileInfo->addr_;
		size_t fileSize = m_fileInfo->size_;
		while (true) {
			res = write(m_sockfd, fileAddr + m_fileWritten, fileSize - m_fileWritten);
			if (res < 0) {
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) { /* 资源暂时不可用 */
					return;
				}
				m_status = kError; /* 出现了错误 */
				return;
			}
			m_fileWritten += res;
			if (m_fileWritten == m_fileInfo->size_)
				break;	
		}
		
	}

	/* 数据发送完毕 */
	reset();
	if (m_keepAlive) /* 如果需要保持连接的话 */
		m_status = kExpectRead;
	else
		m_status = kSuccess;
}

void HttpHandler::process()
{
    cout << "httphandler process" << endl;
    switch (m_status) {
        case kExpectRead: {
            cout << "http handler LIne :" << __LINE__ << endl;
            processRead();
            break;
        }
        case kExpectWrite:{
            cout << "http handler LIne :" << __LINE__ << endl;
            processWrite();
            break;
        }
        default:{
            epoll_removefd(m_epollfd, m_sockfd );
            break;
        }
    }
    cout << "http handler LIne :" << __LINE__ << endl;

    switch(m_status) {
        case kExpectRead:{
            cout << "http handler LIne :" << __LINE__ << endl;
            epoll_modfd(m_epollfd, m_sockfd, EPOLLIN, true);
            break;
        }
        case kExpectWrite:{
            cout << "http handler LIne :" << __LINE__ << endl;
            epoll_modfd(m_epollfd,m_sockfd, EPOLLOUT, true);
            break;
        }
        default:{
            cout << "http handler LIne :" << __LINE__ << endl;
            epoll_removefd(m_epollfd, m_sockfd);
            break;
        }
    }
}


void HttpHandler::clientError(const char *cause, const char *errnum, const char *shortmsg, const char *longmsg)
{
    char body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    m_writeBuffer.appendStr("HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    m_writeBuffer.appendStr("Content-type: text/html\r\n");
    m_writeBuffer.appendStr("Content-length: %d\r\n\r\n", (int)strlen(body));
    m_writeBuffer.appendStr("%s", body);
}

void HttpHandler::serveStatic(const char *fileName, size_t fileSize)
{ /* 用于处理静态的网页 */
    /* Send response headers to client */
    m_writeBuffer.appendStr("HTTP/1.0 200 OK\r\n");
    m_writeBuffer.appendStr("Server: Tiny Web Server\r\n");
    m_writeBuffer.appendStr("Content-length: %d\r\n", fileSize);
    m_writeBuffer.appendStr("Content-type: %s\r\n\r\n", m_request.getFileType());
    m_cache.getFileAddr(fileName, fileSize, m_fileInfo); /* 添加文件 */
    m_sendFile = true;
}
