#pragma once
#include "CurrentThread.h"
#include "CountDownLatch.h"
#include <cstdint>
#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>


class Thread
{
    using ThreadFunc = std::function<void()>;

public:
    Thread(ThreadFunc&& func, const std::string& name = "");
    ~Thread()
    {
        if (_thread->joinable()) _thread->detach();
    }

    void run();

    bool joinable()
    {
        return _thread->joinable();
    }

    void join()
    {
        _thread->join();
    }

    void detach()
    {
        _thread->detach();
    }

    const std::string& getThreadName() const
    {
        return _name;
    }

    static int getNumCreated()
    {
        return _numCreated.load();
    }

private:
    void setDefaultName();

private:
    std::shared_ptr<std::thread> _thread;
    ThreadFunc _threadFunc;
    CountDownLatch _cntDown;
    pid_t _tid; // 线程id

    std::string _name;
    static std::atomic<int32_t> _numCreated;
};