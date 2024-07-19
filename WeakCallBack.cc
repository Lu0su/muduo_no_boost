#include <functional>
#include <memory>
#include <utility>

// 暂时没有理解WeakCallBack是干嘛的.
template<typename CLASS, typename... Args>
class WeakCallBack
{
public:
    WeakCallBack(const std::weak_ptr<CLASS>& obj, const std::function<void(CLASS*, Args...)>& func)
        : _obj{obj}
        , _func{func}
    {
    }

    void operator()(Args&&... args) const
    {
        std::shared_ptr<CLASS> obj{_obj.lock()};
        if (obj)
        {
            _func(obj.get(), std::forward<Args>(args)...);
        }
    }

private:
    std::weak_ptr<CLASS> _obj;
    std::function<void(CLASS*, Args...)> _func;
};

// 利用CLASS内部函数来注册弱回调
template<typename CLASS, typename... Args>
WeakCallBack<CLASS, Args...> makeWeakCallBack(const std::shared_ptr<CLASS>& obj, void (CLASS::*function)(Args...))
{
    return WeakCallBack<CLASS, Args...>(obj, function); 
}

template<typename CLASS, typename... Args>
WeakCallBack<CLASS, Args...> makeWeakCallBack(const std::shared_ptr<CLASS>& obj, void (CLASS::*function)(Args...) const)
{
    return WeakCallBack<CLASS, Args...>(obj, function); 
}