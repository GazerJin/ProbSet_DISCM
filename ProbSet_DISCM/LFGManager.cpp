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

    // Check if there are enough players to form at least one party
    if (t == 0 || h == 0 || d < 3) {
        std::cerr << "Error: Not enough players to form any parties.\n";
        return;
    }

    std::vector<std::thread> threads;
    int nextInstance = 0;

    while (!tankQueue.empty() && !healerQueue.empty() && dpsQueue.size() >= 3) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until there is at least one free instance
        cv.wait(lock, [this]() {
            return std::any_of(instances.begin(), instances.end(), [](const Instance& inst) { return !inst.isActive; });
            });

        // Find the next available instance in a round-robin manner
        int instanceIndex = -1;
        for (int i = 0; i < n; ++i) {
            int idx = (nextInstance + i) % n;
            if (!instances[idx].isActive) {
                instanceIndex = idx;
                break;
            }
        }

        if (instanceIndex != -1) {
            // Mark the instance as active before starting the thread
            instances[instanceIndex].isActive = true;

            // Remove players from queues
            tankQueue.pop();
            healerQueue.pop();
            for (int i = 0; i < 3; ++i) {
                dpsQueue.pop();
            }

            // Assign the party to the instance
            nextInstance = (instanceIndex + 1) % n; // Update round-robin index
            threads.emplace_back(&LFGManager::processParty, this, instanceIndex);

            // Print intermediate status
            std::cout << "\n--- Instance Status ---\n";
            for (int i = 0; i < n; ++i) {
                std::cout << "Instance " << i + 1 << ": " << (instances[i].isActive ? "active" : "empty") << "\n";
            }
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