#pragma once

// 不限制增长的站

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>


// TODO: 使用shared_ptr优化
// TODO: 实现无锁循环队列，实现固定长度的BlockingQueue
template<typename T>
class BlockingQueue
{
    using queue_type = std::queue<T>;

public:
    BlockingQueue()
        : _mtx{}
        , _cv_no_empty{}
        , _que{}
    {
    }
    BlockingQueue(BlockingQueue&& other)
    {
        {
            std::lock_guard lg{other._mtx};
            this->_que = std::move(other._que);
        }
    }

    void push(const T& elem)
    {
        {
            std::lock_guard lg{_mtx};
            _que.push(elem);
        }
        _cv_no_empty.notify_one();
    }

    void push(T&& elem)
    {
        {
            std::lock_guard lg{_mtx};
            _que.emplace(std::move(elem));
        }
        _cv_no_empty.notify_one();
    }

    T pop()
    {
        T ret; // 构造可能抛出异常，如果T是个复杂类型
        {
            std::lock_guard lg{_mtx};
            ret = std::move(_que.front());
        }
        return ret;
    }

    T try_pop()
    {
        T ret;
        {
            std::unique_lock ul{_mtx};
            _cv_no_empty.wait(ul, [this]() {
                return !this->_que.empty();
            });
            ret = std::move(_que.front());
            _que.pop(); // 别忘了弹出
        }

        return ret;
    }

    bool empty()
    {
        std::lock_guard lg{_mtx};
        return _que.empty();
    }

    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator=(const BlockingQueue&) = delete;

private:
    std::mutex _mtx;
    std::condition_variable _cv_no_empty;
    queue_type _que;
};

// int main()
// {
//     BlockingQueue<int> bq;

//     std::thread t1{[&bq]() {
//         for (int i = 0; i < 100000; ++i)
//         {
//             printf("the cur pop is %d\n", bq.try_pop());
//         }
//     }};

//     for (int i = 0; i < 100; ++i)
//     {
//         bq.push(i);
//     }

//     t1.join();
//     return 0;
// }