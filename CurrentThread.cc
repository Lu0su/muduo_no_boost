#include "CurrentThread.h"

namespace CurrentThread
{
__thread int t_cachedTid = 0;

void cacheTid()
{
    if (t_cachedTid == 0)
    {
        // 获取系统线程唯一的tid并缓存
        t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}
} // namespace CurrentThread