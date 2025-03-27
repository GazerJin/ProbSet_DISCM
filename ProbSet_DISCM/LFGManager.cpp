#include "LFGManager.h"

// Constructor to initialize the LFGManager
LFGManager::LFGManager(int n, int t, int h, int d, int t1, int t2)
    : n(n), t(t), h(h), d(d), t1(t1), t2(t2), instances(n) {}

// Function to process a party in an instance
void LFGManager::processParty(int instanceIndex) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(t1, t2);

    int randomTime = distrib(gen); // Random time between t1 and t2

    {
        std::lock_guard<std::mutex> lock(mtx);
        instances[instanceIndex].isActive = true; // Mark instance as active
        instances[instanceIndex].partiesServed++;
        instances[instanceIndex].totalTimeServed += randomTime;
    }

    std::this_thread::sleep_for(std::chrono::seconds(randomTime)); // Simulate dungeon clearing

    {
        std::lock_guard<std::mutex> lock(mtx);
        instances[instanceIndex].isActive = false; // Mark instance as empty
        cv.notify_all(); // Notify waiting threads that an instance is now free
    }
}

// Function to print the summary of all instances
void LFGManager::printSummary() {
    std::cout << "\n--- Summary ---\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "Instance " << i + 1 << ": Served " << instances[i].partiesServed
            << " parties, Total Time: " << instances[i].totalTimeServed << " seconds\n";
    }
}

// Main function to start the LFG manager
void LFGManager::start() {
    // Populate queues with players
    for (int i = 0; i < t; ++i) tankQueue.push(i);
    for (int i = 0; i < h; ++i) healerQueue.push(i);
    for (int i = 0; i < d; ++i) dpsQueue.push(i);

    std::vector<std::thread> threads;

    while (!tankQueue.empty() && !healerQueue.empty() && !dpsQueue.empty()) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until there is at least one free instance
        cv.wait(lock, [this]() {
            for (const auto& instance : instances) {
                if (!instance.isActive) return true;
            }
            return false;
            });

        // Find the first available instance
        int instanceIndex = -1;
        for (int i = 0; i < n; ++i) {
            if (!instances[i].isActive) {
                instanceIndex = i;
                break;
            }
        }

        if (instanceIndex != -1) {
            // Remove players from queues
            tankQueue.pop();
            healerQueue.pop();
            for (int i = 0; i < 3; ++i) dpsQueue.pop();

            // Start processing the party in a new thread
            threads.emplace_back(&LFGManager::processParty, this, instanceIndex);
        }
    }

    // Join all threads
    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    // Print final status of all instances
    std::cout << "\n--- Final Status ---\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "Instance " << i + 1 << ": " << (instances[i].isActive ? "active" : "empty") << "\n";
    }

    // Print summary
    printSummary();
}