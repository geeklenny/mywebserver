#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <string>
#include "noncopyable.h"
#include "common.h"

class Buffer;

class HttpRequest : Noncopyable
{
  public:
	
    HttpRequest()
            : m_state(RequestParseState::kExpectRequestLine)
    {}

    RequestParseState parseRequest(Buffer& buf);
    bool processRequestLine(Buffer& buf);
    bool processHeaders(Buffer& buf);
  private:
    RequestParseState m_state;
  public:
    bool m_keepAlive;
    bool m_sendFile;
    bool m_static; /* 是否为静态页面 */
    std::string m_method;
    std::string m_path;
    std::string m_fileType;
    const char* getFileType();
    void reset();
  private:
    static const char rootDir_[]; /* 网页的根目录 */
    static const char homePage_[]; /* 所指代的网页 */
    void setMethod(const char* start, size_t len);
    void setPath(const char* start, size_t len);
};

#endif
