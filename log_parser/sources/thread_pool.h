#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

using ThreadTask = std::function<void(void)>;

class ThreadPool final
{
public:
    explicit ThreadPool(int desiredThreadCount);
    ~ThreadPool();

    ThreadPool(ThreadPool&&) = delete;

    ThreadPool(ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void addTask(ThreadTask newTask);
    void terminate();
    bool isEnded();

private:
    void loop();
    std::condition_variable m_condition;
    std::mutex m_mutex;
    std::queue<ThreadTask> m_taskQueue;
    std::vector<std::thread> m_pool;
    std::atomic<bool> m_terminatePool;
    std::atomic<int> m_tasksInProcess;
};
