#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace psh
{
    class ThreadPool 
    {
    public:
        explicit ThreadPool(size_t threadCount = 1);
        ~ThreadPool();

        // 작업 예약
        void EnqueueTask(std::function<void()> task);
        
        // 스레드 풀 시작/정지
        void Start();
        void Stop();

        // 상태 확인
        bool IsRunning() const { return running_; }
        size_t GetThreadCount() const { return threadCount_; }

    private:
        void WorkerThread();

        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        
        std::mutex mutex_;
        std::condition_variable condition_;
        
        size_t threadCount_;
        bool running_{false};
    };
}

#endif //THREADPOOL_H
