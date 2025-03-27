#include "LFGManager.h"
#include <iostream>

int main() {
    int n, t, h, d, t1, t2;

    // Input values
    std::cout << "Enter maximum number of concurrent instances (n): ";
    std::cin >> n;
    std::cout << "Enter number of tank players in the queue (t): ";
    std::cin >> t;
    std::cout << "Enter number of healer players in the queue (h): ";
    std::cin >> h;
    std::cout << "Enter number of DPS players in the queue (d): ";
    std::cin >> d;
    std::cout << "Enter minimum time before an instance is finished (t1): ";
    std::cin >> t1;
    std::cout << "Enter maximum time before an instance is finished (t2): ";
    std::cin >> t2;

    // Create and start the LFG manager
    LFGManager lfgManager(n, t, h, d, t1, t2);
    lfgManager.start();

    return 0;
}