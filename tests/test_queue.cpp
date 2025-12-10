#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "SafeQueue.h"

TEST(ConcurrencyTest, QueueHandlesMultipleThreads) {
    SafeQueue<int> queue;
    const int NUM_THREADS = 10;
    const int PUSHES_PER_THREAD = 1000;
    
    std::atomic<int> processed_count{0};
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // Launch Producers
    for(int i=0; i<NUM_THREADS; ++i) {
        producers.emplace_back([&queue]() {
            for(int j=0; j<PUSHES_PER_THREAD; ++j) {
                queue.push(1);
            }
        });
    }

    // Launch Consumers
    for(int i=0; i<NUM_THREADS; ++i) {
        consumers.emplace_back([&queue, &processed_count]() {
            int val;
            for(int j=0; j<PUSHES_PER_THREAD; ++j) {
                queue.waitAndPop(val);
                processed_count++;
            }
        });
    }

    // Join all
    for(auto& t : producers) t.join();
    for(auto& t : consumers) t.join();

    // Verification
    // If the queue wasn't thread safe, we would likely lose numbers or crash
    EXPECT_EQ(processed_count, NUM_THREADS * PUSHES_PER_THREAD);
    EXPECT_TRUE(queue.empty());
}