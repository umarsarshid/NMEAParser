#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template <typename T>
class SafeQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> active{true};

public:
    // PRODUCER: Push data into the queue
    void push(const T& item) {
        {
            // Acquire lock
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(item);
        } // Lock releases here automatically
        
        // Wake up the consumer thread
        cv.notify_one(); 
    }

    // CONSUMER: Wait for data, then pop it
    // Returns false if we should stop (optional design, but good for shutdown)
    bool waitAndPop(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until queue is NOT empty
        // The lambda handles "Spurious Wakeups" (waking up for no reason)
        cv.wait(lock, [this]{ return !queue.empty() || !active;});

        if (!active && queue.empty()) return false; // Optional: handle shutdown case

        item = queue.front();
        queue.pop();
        return true;
    } // Lock releases here
    // New Helper: Break the deadlock
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            active = false;
        }
        cv.notify_all(); // Wake up the consumer immediately
    }
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};