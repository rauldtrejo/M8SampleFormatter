#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result_t<F, Args...>> {
        
        using return_type = typename std::invoke_result_t<F, Args...>;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            if (m_stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            
            m_tasks.emplace([task]() { (*task)(); });
        }
        
        m_condition.notify_one();
        return result;
    }
    
    void waitForAll();
    void shutdown();
    
    size_t getActiveThreads() const { return m_activeThreads.load(); }
    size_t getQueueSize() const;

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;
    std::atomic<size_t> m_activeThreads;
    
    void worker();
};
