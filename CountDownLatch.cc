#include "CountDownLatch.h"
// #include <chrono>
// #include <iostream>
#include <mutex>
// #include <thread>

void CountDownLatch::wait()
{
    std::unique_lock ul{_mtx};
    _cv_count_over.wait(ul, [this]() { return this->_count == 0; });
}
void CountDownLatch::countDown()
{
    std::unique_lock ul{_mtx};
    if (--_count == 0)
    {
        _cv_count_over.notify_all();
    }
}
int CountDownLatch::getCount()
{
    std::lock_guard lg{_mtx};
    return _count;
}

// int main()
// {
//     CountDownLatch cdl{10};
//     std::thread t1{[&cdl]() {
//         while (cdl.getCount())
//         {
//             std::this_thread::sleep_for(std::chrono::seconds{1});
//             cdl.countDown();
//         }
//     }};

//     std::thread t2{[&cdl]() {
//         std::printf("in t2 thread\n");
//         cdl.wait();
//         std::printf("t2 count to 0\n");
//     }};

//     std::thread t3{[&cdl]() {
//         std::printf("in t3 thread\n");
//         cdl.wait();
//         std::printf("t3 count to 0\n");
//     }};

//     t1.join();
//     t2.join();
//     t3.join();

//     return 0;
// }