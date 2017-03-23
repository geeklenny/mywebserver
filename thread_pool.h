#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "noncopyable.h"
#include <functional>
#include <list>
#include <mutex>
#include <condition_variable>

class ThreadPool : Noncopyable
{
  public:
    using Task = std::function<void()>;
    ThreadPool(int, int);
    bool append(Task&&);
    void run();
    static void* startThread(void*);
  private:

    int getTaskQueueSize();
    Task getOneTask();
    bool isFull();

    int m_threadnum;
    int m_maxTaskNum;
    std::list<Task> m_queue;
    std::mutex m_queueMutex;
    std::condition_variable m_notFull;
    std::condition_variable m_notEmpty;
};


#endif
