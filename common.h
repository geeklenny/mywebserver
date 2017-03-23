#ifndef _COMMOM_H_
#define _COMMOM_H_
#include <memory>

enum class RequestParseState {
    kExpectRequestLine,
        kExpectHeaders,
        kExperctBody,
        kGotAll,
        kError
            };

#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */

class FileInfo;
using pInfo = std::shared_ptr<FileInfo>;

#endif
