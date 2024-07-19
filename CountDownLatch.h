#pragma once

#include <condition_variable>
#include <mutex>

class CountDownLatch
{
public:
    explicit CountDownLatch(int count)
        : _mtx{}
        , _cv_count_over{}
        , _count{count}
    {
    }
    void wait();
    void countDown();
    int getCount();

    CountDownLatch(const CountDownLatch&) = delete;
    CountDownLatch& operator=(const CountDownLatch&) = delete;

private:
    std::mutex _mtx;
    std::condition_variable _cv_count_over;
    int _count;
};