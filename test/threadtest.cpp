#include <iostream>
#include <cassert>
#include "threadPool.h"
#include "ini.h"

using namespace utils;
// Test function
int add(int a, int b) {
    return a + b;
}

// Test function that throws an exception
void throw_exception() {
    throw std::runtime_error("Test exception");
}

int main() {
    IniFile ini;
    if(ini.load("/home/fengxu/thread/mythreadPoll/config/config.ini")==false)
    {
        perror("load failed");
    }

    int inital_thread_count = ini.get("thread_pool","inital_thread_count");
    int max_task_count = ini.get("thread_pool","max_task_count");

    std::cout<<"inital_thread_count:"<<inital_thread_count<<std::endl;

    // 创建线程池
    my_thread_poll::ThreadPool pool(inital_thread_count,max_task_count);
    // 添加任务
    auto future=pool.submit(add,1,2);
    std::cout<<future.get()<<std::endl;
    //动态调整线程数量与最大任务数量
    pool.set_max_task_count(10);
    pool.add_thread(2);
    std::cout<<pool.get_thread_count()<<std::endl;
    pool.remove_thread(2);
    std::cout<<pool.get_thread_count()<<std::endl;

}