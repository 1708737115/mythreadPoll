#include "../../include/threadPool.h"

namespace my_thread_poll
{
    ThreadPool::ThreadPool(std::size_t inital_thread_count, std::size_t max_task_count)
        : max_task_count(max_task_count), status(status_t::RUNNING)
    {
        for (int i = 0; i < inital_thread_count; ++i)
        {
            worker_lists.emplace_back(this);
        }
    }

    ThreadPool::~ThreadPool()
    {
        terminate();
    }

    void ThreadPool::pause_with_status_lock()
    {
        switch (status.load())
        {
        case status_t::TERMINATED:  // 线程池已经终止
        case status_t::TERMINATING: // 线程池正在终止
        case status_t::PAUSED:      // 线程池已经暂停
        case status_t::SHUTDOWN:    // 线程池已经关闭
            return;
        case status_t::RUNNING:
            status.store(status_t::PAUSED);
            break;
        default:
            throw std::runtime_error("unknown status");
        }
        std::unique_lock<std::shared_mutex> lock(worker_lists_mutex);
        for (auto &worker : worker_lists)
        {
            worker.pause();
        }
    }

    void ThreadPool::resume_with_status_lock()
    {
        switch (status.load())
        {
        case status_t::TERMINATED:
        case status_t::TERMINATING:
        case status_t::RUNNING:
            return;
        case status_t::PAUSED:
            status.store(status_t::RUNNING);
            break;
        case status_t::SHUTDOWN:
            break;
        default:
            throw std::runtime_error("unknown status");
        }
        std::unique_lock<std::shared_mutex> lock(worker_lists_mutex);
        for (auto &worker : worker_lists)
        {
            worker.resume();
        }
    }


    void ThreadPool::shutdown_with_wait_status_lock(bool wait_for_tasks)
    {
        if (!wait_for_tasks)
            shutdown_with_status_lock();
        switch (status.load())
        {
        case status_t::TERMINATED:
        case status_t::TERMINATING:
        case status_t::SHUTDOWN:
            return;
        case status_t::PAUSED:
            resume_with_status_lock(); // 将处于暂停态的线程恢复为运行态，待任务队列中任务完成后,再进行终止
        case status_t::RUNNING:
            status.store(status_t::SHUTDOWN);
            break;
        default:
            throw std::runtime_error("unknown status");
        }
        std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
        while(!task_queue.empty())
        {
            task_queue_cv_empty.wait(lock);  //在任务队列不为空前阻塞当前线程，将更多的cpu资源分配给任务队列中任务
        }
    }

    void ThreadPool::terminate_with_status_lock()
    {
        switch (status.load())
        {
        case status_t::TERMINATED:
            return;
        case status_t::TERMINATING:
            
        case status_t::SHUTDOWN:
        case status_t::PAUSED:
        case status_t::RUNNING:
            status.store(status_t::TERMINATING);
            break;
        default:
            throw std::runtime_error("unknown status");
        }
        std::unique_lock<std::shared_mutex> lock(worker_lists_mutex);
        for(auto &worker : worker_lists)
        {
            worker.terminate();
        }
        task_queue_cv.notify_all();
        status.store(status_t::TERMINATED);
    }

    void ThreadPool::wait_with_status_lock()  //等待任务全部完成
    {
        switch (status.load())
        {
        case status_t::TERMINATED:
            return;
        case status_t::TERMINATING:
        case status_t::SHUTDOWN:
        case status_t::PAUSED:
        case status_t::RUNNING:
            break;
        default:
            throw std::runtime_error("unknown status");
        }
        std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
        while(!task_queue.empty())
        {
            task_queue_cv_empty.wait(lock);  //在任务队列为空前阻塞当亲线程，将更多的cpu资源分配给任务队列中任务
        } 
    }

    void ThreadPool::wait()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        wait_with_status_lock();
    }

    void ThreadPool::pause()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        pause_with_status_lock();
    }

    void ThreadPool::resume()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        resume_with_status_lock();
    }

    void ThreadPool::shutdown()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        shutdown_with_status_lock();
    }

    void ThreadPool::shutdown_wait()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        shutdown_with_wait_status_lock(true);
    }

    void ThreadPool::terminate()
    {
        std::shared_lock<std::shared_mutex> lock(status_mutex);
        terminate_with_status_lock();
    }

    void ThreadPool::add_thread(std::size_t count)
    {
        std::shared_lock<std::shared_mutex> status_lock(status_mutex);
        switch (status.load())
        {
            case status_t::TERMINATED:
            case status_t::TERMINATING:
            case status_t::PAUSED:
            throw std::runtime_error("[thread_pool::add_thread][error]: cannot add threads to the thread pool in this state");
            case status_t::RUNNING:
            case status_t::SHUTDOWN:
            break;
            default:
            throw std::runtime_error("[thread_pool::add_thread][error]: invalid thread pool state");
        }
        std::unique_lock<std::shared_mutex> worker_lists_lock(worker_lists_mutex);
        for(std::size_t i = 0; i < count; ++i)
        {
            worker_lists.emplace_back(this);
        }
        
    }

    void ThreadPool::remove_thread(std::size_t count)
    {
        std::shared_lock<std::shared_mutex> status_lock(status_mutex);
        switch (status.load())
        {
            case status_t::TERMINATED:
            case status_t::TERMINATING:
            case status_t::PAUSED:
            throw std::runtime_error("[thread_pool::add_thread][error]: cannot add threads to the thread pool in this state");
            case status_t::RUNNING:
            case status_t::SHUTDOWN:
            break;
            default:
            throw std::runtime_error("[thread_pool::add_thread][error]: invalid thread pool state");
        }
        std::unique_lock<std::shared_mutex> work_lists_lock(worker_lists_mutex);
        count=std::min(count, worker_lists.size());
        auto it=worker_lists.end();
        for(int i=0;i<count;++i)
        {
            it--;
            it->terminate();
        }
        task_queue_cv.notify_all(); //唤醒所有线程,让线程自身查看状态
        worker_lists.erase(it, worker_lists.end()); //删除线程
    }

    std::size_t ThreadPool::get_task_count()
    {
        std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
        return task_queue.size();
    }

    std::size_t ThreadPool::get_thread_count()
    {
        std::shared_lock<std::shared_mutex> lock(worker_lists_mutex);
        return worker_lists.size();
    }

};