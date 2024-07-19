#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
// 线程本地缓存
extern __thread int t_cachedTid; // 保存tid缓冲，避免多次系统调用

void cacheTid();

// 内联函数
inline int tid()
{
    // 预测便于优化，不必在意
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
        // 因为获取tid的系统调用还是挺耗时间的.
        cacheTid();
    }
    return t_cachedTid;
}
} // namespace CurrentThread

#endif // CURRENT_THREAD_H