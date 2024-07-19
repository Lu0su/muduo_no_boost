#pragma once
#include <algorithm>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <iostream>

template<typename T>
class BoundedBlockingQueue
{
public:
    BoundedBlockingQueue(int maxsize)
        : _mtx{}
        , _cv_no_empty{}
        , _cv_no_full{}
        , _circle_que(maxsize, 0)
        , _front{0}
        , _rear{0}
        , _size{0}
    {
    }

    BoundedBlockingQueue(BoundedBlockingQueue&& other)
    {
        {
            std::lock_guard lg{other._mtx};
            _circle_que = std::move(other._circle_que);
            _front = other._front;
            _rear = other._rear;
            _size = other._size;
        }
    }

    BoundedBlockingQueue(const BoundedBlockingQueue&) = delete;
    BoundedBlockingQueue& operator=(const BoundedBlockingQueue&) = delete;

    void push(std::shared_ptr<T> ptr)
    {
        {
            std::unique_lock ul{_mtx};
            _cv_no_full.wait(ul, [this]() {
                return _size < _circle_que.size();
            });
            _circle_que[_rear++] = ptr;
            _rear %= _circle_que.size();
            _size++;
        }
        _cv_no_empty.notify_one();
    }

    std::shared_ptr<T> pop()
    {
        std::shared_ptr<T> ret;
        {
            std::unique_lock ul{_mtx};
            _cv_no_empty.wait(ul, [this]() {
                return _size > 0;
            });
            ret = std::move(_circle_que[_front++]);
            _front %= _circle_que.size();
            --_size;
        }
        _cv_no_full.notify_one();
        return std::move(ret);
    }

    bool empty() const
    {
        std::lock_guard lg{_mtx};
        return _size == 0;
    }

    bool full() const
    {
        std::lock_guard lg{_mtx};
        return _size == _circle_que.size();
    }


private:
    std::mutex _mtx;
    std::condition_variable _cv_no_empty;
    std::condition_variable _cv_no_full;
    std::vector<std::shared_ptr<T>> _circle_que;
    int _front;
    int _rear;
    int _size;
};

// int main()
// {
//     BoundedBlockingQueue<int> bq(100);

//     std::thread t1{[&bq]() {
//         for (int i = 0; i < 100000; ++i)
//         {
//             printf("the cur pop is %d\n", *bq.pop());
//         }
//     }};

//     for (int i = 0; i < 100; ++i)
//     {
//         bq.push(std::make_shared<int>(i));
//     }

//     t1.join();
//     return 0;
// }