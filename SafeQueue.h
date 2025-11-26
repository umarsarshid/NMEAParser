#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;

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
    void waitAndPop(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until queue is NOT empty
        // The lambda handles "Spurious Wakeups" (waking up for no reason)
        cv.wait(lock, [this]{ return !queue.empty(); });

        item = queue.front();
        queue.pop();
    } // Lock releases here
    
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};