#include "thread_pool.h"
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

ThreadPool::ThreadPool(int threadnum, int maxTaskNum)
        :m_threadnum(threadnum)
        ,m_maxTaskNum(maxTaskNum)
{
    pthread_t pid;
    for(int i = 0; i < threadnum; i++)
    {
        pthread_create(&pid, NULL, startThread, this);
    }
}

bool ThreadPool::isFull()
{

    return m_maxTaskNum > 0 && m_queue.size() >= m_maxTaskNum; 
}

bool ThreadPool::append( Task && task)
{
    cout << "thread poll addpend" << endl;
    std::unique_lock<std::mutex> lock(m_queueMutex);
    while(isFull())
    {
    cout << "thread poll addpend is full" << endl;
        m_notFull.wait(lock);
    }

    m_queue.push_back(std::move(task));
    m_notEmpty.notify_all();
    return true;
}

void ThreadPool::run()
{
    while(true)
    {
cout << "Threadpool run b4 get one task" << endl;
        Task task(getOneTask());
cout << "Threadpool run aft get on task" << endl;
        if(task)
        {
            task();
        }
    }
}

void* ThreadPool::startThread(void* obj)
{
    pthread_detach(pthread_self());

    ThreadPool* threadPool = static_cast<ThreadPool*>(obj);
    threadPool->run();
    return threadPool;
}

int ThreadPool::getTaskQueueSize()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_queue.size();
}

ThreadPool::Task ThreadPool::getOneTask()
{
    cout << "thread poll getOneTask" << endl;
    std::unique_lock<std::mutex> lock(m_queueMutex);
    while(m_queue.empty())
    {
    cout << "thread poll getOneTask empty" << endl;
        m_notEmpty.wait(lock);
    cout << "thread poll getOneTask notempty" << endl;
    }
    Task task;
    if(!m_queue.empty())
    {
        task = m_queue.front();
        m_queue.pop_front();
    cout << "thread poll getOneTask pop" << endl;
        if(m_maxTaskNum > 0)
        {
            m_notFull.notify_all();
        }
    }
    return task;
}




