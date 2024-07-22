#include <atomic>
#include <cstddef>
#include <cassert>

template<typename T>
class LockFreeQueue
{
public:
    explicit LockFreeQueue(size_t capacity)
        : capacity_(capacity)
    {
        assert((capacity > 0) && ((capacity & (capacity - 1)) == 0)); // Capacity must be a power of 2
        buffer_ = new T[capacity_];
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

    ~LockFreeQueue()
    {
        delete[] buffer_;
    }

    bool enqueue(const T& item)
    {
        size_t tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (tail + 1) & (capacity_ - 1);

        if (next_tail == head_.load(std::memory_order_acquire))
        {
            // Queue is full
            return false;
        }

        buffer_[tail] = item;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool dequeue(T& item)
    {
        size_t head = head_.load(std::memory_order_relaxed);

        if (head == tail_.load(std::memory_order_acquire))
        {
            // Queue is empty
            return false;
        }

        item = buffer_[head];
        head_.store((head + 1) & (capacity_ - 1), std::memory_order_release);
        return true;
    }

private:
    T* buffer_;
    const size_t capacity_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};
