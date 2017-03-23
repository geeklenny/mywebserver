#include "http_request.h"
#include "buff.h"

const char HttpRequest::rootDir_[] = "../WebSiteSrc/html_book_20150808/reference";
const char HttpRequest::homePage_[] = "index.html";

void HttpRequest::reset() /* 清零 */
{
	m_state = RequestParseState::kExpectRequestLine;
	m_path.clear();
}

RequestParseState HttpRequest::parseRequest(Buffer& buf)
{
	bool ok = true;
	bool hasMore = true;
	while (hasMore) {
		if (m_state == RequestParseState::kExpectRequestLine) {
			const char* crlf = buf.findCRLF(); /* 找到回车换行符 */
			if (crlf) { /* 如果找到了! */
				ok = processRequestLine(buf);
			}
			else {
				hasMore = false; /* 没有找到,可能表示对方还没有发送完整的一行数据,要继续去监听客户的写事件 */
			}

			if (ok) 
				m_state = RequestParseState::kExpectHeaders;
			else {
				m_state = RequestParseState::kError; /* 出现错误 */
				hasMore = false;
			}
		}
		else if (m_state == RequestParseState::kExpectHeaders) { /* 处理头部的信息 */
			if (true == (ok = processHeaders(buf))) 
				m_state = RequestParseState::kGotAll;
			hasMore = false;
		}
	}
	return m_state;
}

const char* HttpRequest::getFileType()
{ /* 获得文件的类型 */
	if (strstr(m_path.c_str(), ".html"))
		m_fileType = "text/html";
	else if (strstr(m_path.c_str(), ".gif"))
		m_fileType = "image/gif";
	else if (strstr(m_path.c_str(), ".jpg"))
		m_fileType = "image/jpeg";
	else if (strstr(m_path.c_str(), ".png"))
		m_fileType = "image/png";
	else if (strstr(m_path.c_str(), ".css"))
		m_fileType = "text/css";
	else if (strstr(m_path.c_str(), ".ttf") || strstr(m_path.c_str(), ".otf"))
		m_fileType = "application/octet-stream";
	else
		m_fileType =  "text/plain";
	return m_fileType.c_str();
}

bool HttpRequest::processHeaders(Buffer& buf) /* 处理其余的头部信息 */
{ /* 其余的玩意,我就不处理啦! */
	char line[1024];
	while (buf.getLine(line, sizeof line)) {
		if (strlen(line) == 0) { /* 只有取到了最后一个才能返回true */
			return true;
		}
		if (strstr(line, "keep-alive")) {
			m_keepAlive = true; /* 保持连接 */
		}

	}
	return false;
}

void HttpRequest::setMethod(const char* start, size_t len)
{
	m_method.assign(start, start + len);
}

void HttpRequest::setPath(const char* uri, size_t len)
{
	if (!strstr(uri, "mwiki")) {
		m_path += rootDir_;
		m_path += uri;
		if (uri[strlen(uri) - 1] == '/')
			m_path += homePage_;
		m_static = true; /* 请求的是静态文件 */
		return;
	}
	else {  /* Dynamic content */
		/* 暂时不支持动态文件 */
		m_static = false;
		return;
	}
}

bool HttpRequest::processRequestLine(Buffer& buf)
{
	char line[256];
	char method[64], path[256], version[64];
	buf.getLine(line, sizeof line); 
	sscanf(line, "%s %s %s", method, path, version);
	setMethod(method, strlen(method));
	setPath(path, strlen(path));
	/* version就不处理了 */
	return true;
}
