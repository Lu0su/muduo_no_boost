#include "Thread.h"
#include <cstdio>
#include <memory>
#include <thread>
#include <iostream>
#include "CurrentThread.h"

std::atomic<int32_t> _numCreated{0};

Thread::Thread(Thread::ThreadFunc&& func, const std::string& name)
    : _threadFunc(std::move(func))
    , _cntDown(1)
    , _name(name)
{
    setDefaultName();
    // run(); 去手动调用run吧
}


void Thread::run()
{
    _thread = std::make_shared<std::thread>([this]() {
        _tid = CurrentThread::tid();
        // printf("cur tid = %d\n", _tid);
        _cntDown.countDown();
        _threadFunc();
    });

    _cntDown.wait();
}

void Thread::setDefaultName()
{
    if (_name != "") return;

    char buf[36] = {0};
    sprintf(buf, "Thread%d", _numCreated.load());
    _name = buf;
}


int main()
{
    int a = 10;
    Thread t1([a]() {
        printf("hello world\n");
    });

    t1.join();

    return 0;
}