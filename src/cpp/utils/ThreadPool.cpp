#include "ThreadPool.h"
#include <algorithm>

ThreadPool::ThreadPool(size_t numThreads) : m_stop(false), m_activeThreads(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back([this] { worker(); });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

// Template implementation moved to header file

void ThreadPool::waitForAll() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_condition.wait(lock, [this] { return m_tasks.empty() && m_activeThreads == 0; });
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    
    m_condition.notify_all();
    
    for (std::thread &worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPool::getQueueSize() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_queueMutex));
    return m_tasks.size();
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
            
            if (m_stop && m_tasks.empty()) {
                return;
            }
            
            task = std::move(m_tasks.front());
            m_tasks.pop();
            m_activeThreads++;
        }
        
        task();
        
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_activeThreads--;
        }
        
        m_condition.notify_all();
    }
}

// Template instantiation will be handled by the compiler
