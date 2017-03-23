#ifndef _CACHE_H_
#define _CACHE_H_

#include <map>
#include <string>
#include <memory>
#include "wrapperfun.h"
#include "noncopyable.h"
#include <mutex>
#include "common.h"
using namespace util;


class FileInfo : Noncopyable
{
  public:
    FileInfo(std::string& fileName, int fileSize);
    ~FileInfo() {
        Munmap(addr_, size_); /* 解除映射 */
    }
  public:
    void *addr_; /* 地址信息 */
    int size_; /* 文件大小 */

};

class Cache : Noncopyable
{
    using Router = std::map<std::string, std::shared_ptr<FileInfo>>;
  private:
    Router cache_; /* 实现文件名称到地址的一个映射 */
    static const size_t MAX_CACHE_SIZE = 100; /* 最多缓存100个文件 */
    std::mutex mutex_;
	
  public:
    Cache()
            : mutex_()
    {}
    /* 下面的版本线程不安全 */
    pInfo getFileAddr(std::string fileName, int fileSize);
    /* 线程安全版本的getFileAddr */
    void getFileAddr(std::string fileName, int fileSize, pInfo& ptr);
};

#endif /* _CACHE_H_ */
