#include "FunctionWrapper.h"
#include "Thread.h"
#include <atomic>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>
#include "ConcurrentQueue.h"

class ThreadPool
{
public:
    ThreadPool()
    {
        std::size_t n = std::thread::hardware_concurrency();
        try
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                threads_.emplace_back([this]() {
                    this->worker_thread();
                });
            }
        }
        catch (...)
        {
            done_ = true;
            for (auto& x : threads_)
            {
                if (x.joinable())
                {
                    x.join();
                }
            }
            throw;
        }
    }

    ~ThreadPool()
    {
        done_ = true;
        for (auto& x : threads_)
        {
            if (x.joinable())
            {
                x.join();
            }
        }
    }

    // 构造可调用对象的任务交给了使用者
    template<typename F>
    std::future<std::invoke_result_t<F>> submit(F f)
    {
        std::packaged_task<std::invoke_result_t<F>()> task(std::move(f));
        std::future<std::invoke_result_t<F>> res(task.get_future());
        q_.push(std::move(task));
        return res;
    }

private:
    void worker_thread()
    {
        while (!done_)
        {
            FunctionWrapper task;
            if (q_.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

private:
    std::atomic<bool> done_ = false;
    ConcurrentQueue<FunctionWrapper> q_;
    std::vector<Thread> threads_; // 要在 done_ 和 q_ 之后声明
};