#include "../../include/threadPool.h"
#include <iostream>

namespace my_thread_poll
{
    /*work_thread构造函数主要是在构造函数中执行lambda表达式，该表达式中执行工作线程的循环工作函数
    工作流程如下:
    - 确定工作线程状态,决定是否由该线程执行任务
    - 查看工作队列是否为空,不为空则取出一个任务
    - 执行任务
    - 根据线程池状态变更，如接收到暂停、恢复、终止等指令，工作线程调整自身状态并执行相应操作
    */

    ThreadPool::worker_thread::worker_thread(ThreadPool *pool):pool(pool),status(status_t::RUNNING),sem(0),thread(
    [this](){
        while (true)
        {
            // 实现线程状态的判断，决定是否由该线程执行任务
            std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
            while (true)
            {
                if (!unique_lock_status.owns_lock()) // 当锁被释放时，重新获取锁
                {
                    unique_lock_status.lock();
                }
                bool break_flag = false; // 当线程为运行态的时候，跳出循环
                switch (this->status.load())
                {
                case status_t::TERMINATING:
                    this->status.store(status_t::TERMINATED);
                case status_t::TERMINATED:
                    return;
                case status_t::RUNNING:
                    break_flag = true;
                    break;
                case status_t::PAUSE: // PAUSE状态下需要其他的线程来唤醒该线程需要解锁避免出现死锁
                    unique_lock_status.unlock();
                    this->sem.acquire(); // 阻塞当前线程
                    break;
                case status_t::BLOCKED: // 不支持Blocked
                default:
                    unique_lock_status.unlock();
                    throw std::runtime_error("invalid status");
                }
                if (break_flag)
                {
                    unique_lock_status.unlock();
                    break;
                }
            }

            // 判断队列是否为空，如果为空则阻塞当前线程
            std::unique_lock<std::shared_mutex> unique_lock_task(this->pool->task_queue_mutex);
            while (this->pool->task_queue.empty())
            {
                while (true)
                {
                    if (!unique_lock_status.owns_lock())
                    {
                        unique_lock_status.lock();
                    }
                    bool break_flag = false;
                    switch (this->status.load())
                    {
                    case status_t::TERMINATING:
                        status.store(status_t::TERMINATED);
                    case status_t::TERMINATED:
                        return;
                    case status_t::PAUSE:
                        unique_lock_task.unlock();
                        unique_lock_status.unlock();
                        this->sem.acquire(); // 阻塞线程
                        break;
                    case status_t::RUNNING:
                        this->status.store(status_t::BLOCKED);
                    case status_t::BLOCKED:
                        break_flag = true;
                        break;
                    default:
                        unique_lock_status.unlock();
                        unique_lock_task.unlock();
                        throw std::runtime_error("invalid status");
                    }
                    if (break_flag) // 若为阻塞状态等待唤醒
                    {
                        unique_lock_status.unlock();
                        break;
                    }
                }
                this->pool->task_queue_cv.wait(unique_lock_task);
                while (true)
                {
                    if (!unique_lock_status.owns_lock())
                    {
                        unique_lock_status.lock();
                    }
                    bool break_flag = false;
                    switch (this->status.load())
                    {
                    case status_t::TERMINATING:
                        status.store(status_t::TERMINATED);
                    case status_t::TERMINATED:
                        return;
                    case status_t::PAUSE:
                        unique_lock_task.unlock();
                        unique_lock_status.unlock();
                        this->sem.acquire(); // 阻塞线程
                        break;
                    case status_t::BLOCKED:
                        this->status.store(status_t::RUNNING);
                    case status_t::RUNNING:
                        break_flag = true;
                        break;
                    default:
                        unique_lock_status.unlock();
                        throw std::runtime_error("invalid status");
                    }
                    if (break_flag) // 若为阻塞状态等待唤醒
                    {
                        unique_lock_status.unlock();
                        break;
                    }
                }
            }
            // 尝试取出任务并执行
            try
            {
                auto task = this->pool->task_queue.front();
                this->pool->task_queue.pop();
                if (this->pool->task_queue.empty())
                {
                    this->pool->task_queue_cv_empty.notify_all();
                }
                unique_lock_task.unlock();
                task();
            }
            catch (const std::exception &e)
            {
                std::cerr<<e.what()<<std::endl;
            }
        }
    }){}

    ThreadPool::worker_thread::~worker_thread()
    {
        status_t last_status = terminate();
        if(thread.joinable())
        {
            if(last_status == status_t::PAUSE)
            {
                this->pool->task_queue_cv.notify_all();  //唤醒所有阻塞的线程,避免析构时阻塞
            }
            thread.join();
        }
    }
    void ThreadPool::worker_thread::resume_with_status_lock()
    {
        switch (this->status.load())
        {
            case status_t::TERMINATING:
            case status_t::TERMINATED:
            case status_t::BLOCKED:
            case status_t::RUNNING:
                return;
            case status_t::PAUSE:
            status.store(status_t::RUNNING);
            break;
            default:
                throw std::runtime_error("invalid status");
        }
        this->sem.release(); //唤醒线程
    }
    
    void ThreadPool::worker_thread::resume()
    {
        std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
        this->resume_with_status_lock();
    }

    void ThreadPool::worker_thread::pause_with_status_lock()
    {
        switch (this->status.load())
        {
            case status_t::TERMINATED:
            case status_t::TERMINATING:
            case status_t::PAUSE:
                return;
            case status_t::RUNNING:
            case status_t::BLOCKED:
                this->status.store(status_t::PAUSE);
                break;
            default:
                throw std::runtime_error("invalid status");
        }
    }

    void ThreadPool::worker_thread::pause()
    {
        std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
        this->pause_with_status_lock();
    }

    ThreadPool::worker_thread::status_t ThreadPool::worker_thread::terminate_with_status_lock()
    {
        status_t last_status=status.load();
        switch (this->status.load())
        {
            case status_t::TERMINATING:
            status.store(status_t::TERMINATED);
            case status_t::TERMINATED:
            return last_status;
            case status_t::RUNNING:
            case status_t::BLOCKED:
            status.store(status_t::TERMINATING);
            break;
            case status_t::PAUSE:
            status.store(status_t::TERMINATING);
            this->sem.release();
            break;
            default:
                throw std::runtime_error("invalid status");
        }
        return last_status;
    }

    ThreadPool::worker_thread::status_t ThreadPool::worker_thread::terminate()
    {
        std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
        return this->terminate_with_status_lock();
    }
};
