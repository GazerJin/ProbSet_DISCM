#pragma once

#ifndef LFGMANAGER_H
#define LFGMANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>

class Instance {
public:
    bool isActive;
    int partiesServed;
    int totalTimeServed;

    Instance() : isActive(false), partiesServed(0), totalTimeServed(0) {}
};

class LFGManager {
private:
    int n; // Maximum number of concurrent instances
    int t; // Number of tank players in the queue
    int h; // Number of healer players in the queue
    int d; // Number of DPS players in the queue
    int t1; // Minimum time before an instance is finished
    int t2; // Maximum time before an instance is finished

    std::vector<Instance> instances; // Tracks all instances
    std::queue<int> tankQueue; // Queue for tanks
    std::queue<int> healerQueue; // Queue for healers
    std::queue<int> dpsQueue; // Queue for DPS

    std::mutex mtx; // Mutex for synchronization
    std::condition_variable cv; // Condition variable for signaling

    void processParty(int instanceIndex);
    void printSummary();

public:
    LFGManager(int n, int t, int h, int d, int t1, int t2);
    void start();
};

#endif
