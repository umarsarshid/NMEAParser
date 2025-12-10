#include <iostream>
#include <thread>
#include <vector>
#include "SafeQueue.h"

SafeQueue<int> q;
const int NUM_ITEMS = 1000;

void producer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        q.push(1); // Just push the number 1
    }
}

void consumer(int& sum) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        int val;
        q.waitAndPop(val);
        sum += val;
    }
}

int main() {
    std::cout << "Testing Thread Safety..." << std::endl;
    int total = 0;

    // Launch Producer and Consumer in parallel
    std::thread t1(producer);
    std::thread t2(consumer, std::ref(total));

    // Wait for them to finish
    t1.join();
    t2.join();

    // Verify Result
    if (total == NUM_ITEMS) {
        std::cout << "PASSED: Processed " << total << " items without race conditions." << std::endl;
    } else {
        std::cout << "FAILED: Race condition detected! Count: " << total << std::endl;
    }
    return 0;
}