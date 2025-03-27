#include "LFGManager.h"
#include <iostream>
#include <string>
#include <cctype>
#include <limits>

// Function to validate and get unsigned integer input
unsigned int getUnsignedIntegerInput(const std::string& prompt) {
    std::string input;
    unsigned int value;

    while (true) {
        std::cout << prompt;
        std::cin >> input;

        // Check if the input contains only digits
        bool isValid = true;
        for (char c : input) {
            if (!std::isdigit(c)) {
                isValid = false;
                break;
            }
        }

        // If valid, convert to unsigned integer
        if (isValid) {
            try {
                value = std::stoul(input); // Convert string to unsigned long
                return value;              // Return the validated value
            }
            catch (...) {
                // Catch any conversion errors (though unlikely due to prior checks)
                std::cerr << "Invalid input. Please enter a positive integer.\n";
            }
        }
        else {
            std::cerr << "Invalid input. Please enter a positive integer without letters or special characters.\n";
        }

        // Clear the input stream in case of invalid input
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    unsigned int n, t, h, d, t1, t2;

    // Get and validate inputs
    n = getUnsignedIntegerInput("Enter maximum number of concurrent instances (n): ");

    // Additional validation: Ensure n > 0
    while (n == 0) {
        std::cerr << "Invalid input. The number of instances (n) must be greater than 0.\n";
        n = getUnsignedIntegerInput("Enter maximum number of concurrent instances (n): ");
    }

    t = getUnsignedIntegerInput("Enter number of tank players in the queue (t): ");
    h = getUnsignedIntegerInput("Enter number of healer players in the queue (h): ");
    d = getUnsignedIntegerInput("Enter number of DPS players in the queue (d): ");
    t1 = getUnsignedIntegerInput("Enter minimum time before an instance is finished (t1): ");
    t2 = getUnsignedIntegerInput("Enter maximum time before an instance is finished (t2): ");

    // Additional validation for t2 <= 15
    while (t2 > 15 || t1 > t2) {
        if (t2 > 15) {
            std::cerr << "Invalid input. Maximum time (t2) must be <= 15.\n";
        }
        else if (t1 > t2) {
            std::cerr << "Invalid input. Minimum time (t1) must be <= Maximum time (t2).\n";
        }
        t1 = getUnsignedIntegerInput("Enter minimum time before an instance is finished (t1): ");
        t2 = getUnsignedIntegerInput("Enter maximum time before an instance is finished (t2): ");
    }

    // Create the LFGManager object with the validated inputs
    LFGManager lfgManager(n, t, h, d, t1, t2);

    // Start the LFG manager
    lfgManager.start();

    return 0; // Successful execution
}