#include "thread_pool.h"
#include <functional>
#include <cassert>

ThreadPool::ThreadPool(int desiredThreadCount) :
        m_terminatePool(false), m_tasksInProcess(0)
{
    assert(("desired thread count must be greater than 0", desiredThreadCount > 0));
    if (desiredThreadCount > std::thread::hardware_concurrency())
    {
        desiredThreadCount = std::thread::hardware_concurrency();
    }
    for (int i = 0; i < desiredThreadCount; i++)
    {
        m_pool.push_back(std::thread(std::bind(&ThreadPool::loop, this)));
    }
}

ThreadPool::~ThreadPool()
{
    terminate();
}

void ThreadPool::addTask(ThreadTask newTask)
{
    m_tasksInProcess++;

    std::unique_lock<std::mutex> lock(m_mutex);
    m_taskQueue.push(newTask);
    lock.unlock();

    m_condition.notify_one();
}
bool ThreadPool::isEnded() const
{
    return m_tasksInProcess == 0;
}

void ThreadPool::terminate()
{
    m_terminatePool = true;
    m_condition.notify_all();

    for (auto& thread : m_pool)
    {
        if (thread.joinable()) thread.join();
    }
}

void ThreadPool::loop()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condition.wait(lock, [this]
        {
            return !m_taskQueue.empty() || m_terminatePool;
        });
        if (m_terminatePool) return;
        auto job = m_taskQueue.front();
        m_taskQueue.pop();
        lock.unlock();
        job();
        m_tasksInProcess--;
    }
};
