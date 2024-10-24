/**
 * @file threadPool.h
 * @author fengxu (2112873995@qq.com)
 * @brief 线程池的头文件,包含线程池的创建、销毁、添加任务、获取任务数量等函数
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <queue>
#include <future>
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <functional>
#include <shared_mutex>
#include <condition_variable>

namespace my_thread_poll
{

    class ThreadPool
    {
    private:
        class worker_thread; // 工作线程类
        enum class status_t : std::int8_t
        {
            TERMINATED = -1,
            TERMINATING = 0,
            RUNNING = 1,
            PAUSED = 2,
            SHUTDOWN = 3
        }; // 线程池的状态: 已终止:-1,正在终止:0,正在运行:1,已暂停:2,等待线程池中任务完成,但是不接收新任务:3
        std::atomic<status_t> status;                    // 线程池的状态
        std::atomic<std::size_t> max_task_count;         // 线程池中任务的最大数量
        std::shared_mutex status_mutex;                  // 线程池状态互斥锁
        std::shared_mutex task_queue_mutex;              // 任务队列的互斥锁
        std::shared_mutex worker_lists_mutex;            // 工作线程列表的互斥锁
        std::condition_variable_any task_queue_cv;       // 任务队列的条件变量
        std::condition_variable_any task_queue_cv_full;  // 任务队列满的条件变量
        std::condition_variable_any task_queue_cv_empty; // 任务队列空的条件变量
        std::queue<std::function<void()>> task_queue;    // 任务队列,其中存储待执行的任务
        std::list<worker_thread> worker_lists;           // 工作线程列表
        // 考虑到为了确保线程池的唯一性和安全性,禁止使用拷贝赋值与移动赋值
        ThreadPool(ThreadPool &) = delete;
        ThreadPool &operator=(ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &operator=(ThreadPool &&) = delete;
        // 在取得对状态变量的访问权后,调用下列函数来改变线程池的状态
        void pause_with_status_lock();                            // 暂停线程池
        void resume_with_status_lock();                           // 恢复线程池
        void shutdown_with_status_lock();                         // 立刻关闭线程池
        void shutdown_with_wait_status_lock(bool wait_for_tasks); // 等待任务执行完毕关闭线程池
        void terminate_with_status_lock();                        // 终止线程池
        void wait_with_status_lock();                             // 等待所有任务执行完毕
    public:
        ThreadPool(std::size_t max_task_count,std::size_t inital_thread_count=0); // 构造函数
        ~ThreadPool();                                                               // 析构函数
        template <typename Func, typename... Args>
        auto submit(Func &&f, Args &&...args) -> std::future<decltype(f(args...))>; // 提交任务,实现对线程任务的异步提交
        void pause();                                                                  // 暂停线程池
        void resume();                                                                 // 恢复线程池
        void shutdown();                                                               // 立刻关闭线程池
        void shutdown_wait();                                                          // 等待任务执行完毕关闭线程池
        void terminate();                                                              // 终止线程池
        void wait();                                                                   // 等待所有任务执行完毕
        void add_thread(std::size_t count);                                            // 增加线程
        void remove_thread(std::size_t count);                                         // 删除线程
        void set_max_task_count(std::size_t count);
        std::size_t get_task_count();   // 获取任务数量
        std::size_t get_thread_count(); // 获取线程数量
    };
    inline void ThreadPool::set_max_task_count(std::size_t count_to_set)
    { // 设置任务队列中任务的最大数量；如果设置后的最大数量小于当前任务数量，则会拒绝新提交的任务，直到任务数量小于等于最大数量
        max_task_count.store(count_to_set);
    }

    inline void ThreadPool::shutdown_with_status_lock()
    {
        terminate_with_status_lock();
    }

    /*
    sumbit函数实现线程池中任务的提交，它的工作流程如下:
    1.首先查看当前线程池的状态，如果不是RUNNING状态,抛出异常
    2.查看当前的任务队列是否已满，如果已满，则抛出异常
    3.将任务转换为std::packaged_task对象，并将其包装为std::function对象，
    以便在线程池中执行，这里使用了std::forward将参数传递给任务函数实现完美
    转发,通过std::function对象将任务函数包装为std::function<void()>类型,
    以便在线程池中通过在工作线程中可以用统一的格式（直接用 () 进行调用）对任
    何形式的任务进行调用执行
    4.将std::packaged_task对象添加到任务队列中，并返回一个std::future对象,该对象可以用于获取任务函数的返回值
    */
    template <typename Func, typename... Args>
    auto ThreadPool::submit(Func &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        std::shared_lock<std::shared_mutex> status_lock(status_mutex);
        switch (status.load())
        {
            case status_t::TERMINATED:
            throw std::runtime_error("ThreadPool is terminated");
            case status_t::TERMINATING:
            throw std::runtime_error("ThreadPool is terminating");
            case status_t::PAUSED:
            throw std::runtime_error("ThreadPool is paused");
            case status_t::SHUTDOWN:
            throw std::runtime_error("ThreadPool is shutdown");
            case status_t::RUNNING:
            break;
        }
        if(max_task_count > 0&&max_task_count.load()==get_task_count())
            throw std::runtime_error("ThreadPool is full");
        using return_type=decltype(f(args...));
        auto task=std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
        std::unique_lock<std::shared_mutex> lock2(task_queue_mutex);
        task_queue.emplace([task](){ (*task)(); }); 
        lock2.unlock();
        status_lock.unlock();
        task_queue_cv.notify_one();  //唤醒一个线程来执行当前任务
        std::future<return_type> res=task->get_future();
        return res;
    }

    class ThreadPool::worker_thread
    {
        private:
            enum class status_t:int8_t{
                TERMINATED=-1,
                TERMINATING=0,
                PAUSE=1,
                RUNNING=2,
                BLOCKED=3
            };   // 1- 线程已终止 0- 线程正在终止 1- 线程已暂停 2- 线程正在运行 3- 线程已阻塞,等待任务中
            ThreadPool *pool; //指向线程池
            std::atomic<status_t> status; //线程状态
            std::shared_mutex status_mutex; //线程状态互斥锁
            std::binary_semaphore sem; //信号量,要来控制线程的阻塞和唤醒
            std::thread thread; //工作线程
            //禁用拷贝构造与移动构造以及相关复赋值
            worker_thread(const worker_thread &) = delete;
            worker_thread(worker_thread &&) = delete;
            worker_thread &operator=(const worker_thread &) = delete;
            worker_thread &operator=(worker_thread &&) = delete;

            void resume_with_status_lock();
            status_t terminate_with_status_lock();
            void pause_with_status_lock();
            
        public:
            worker_thread(ThreadPool *pool);
            ~worker_thread();
            void pause();
            void resume();
            status_t terminate();
    };
};

#endif // THREAD_POOL_H