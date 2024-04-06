// John Dufresne - Assignment 3 - Problem 2 (hour converted to minute - minute converted to second)
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

#define NUM_SENSORS 8

struct TemperatureReading {
    int temp; // Temperature value
    chrono::steady_clock::time_point timestamp; // Timestamp of reading

    // Constructor for easy initialization
    TemperatureReading(int t, chrono::steady_clock::time_point ts) : temp(t), timestamp(ts) {}
};

vector<TemperatureReading> readings; // Shared memory - vector holding all the readings
mutex readingsMtx; // mutex for temperature readings
ofstream outFile("TemperatureReport.txt");

// Random number generator function
int getRandomTemp() {
    random_device seed;
    mt19937 rng(seed());
    uniform_int_distribution<mt19937::result_type> dist(-100, 70);
    return dist(rng);
}

// Safely output messages to the file
void safePrint(const string& message) {
    static mutex fileMutex; // Protects access to outFile
    lock_guard<mutex> lock(fileMutex);
    if (outFile.is_open()) {
        outFile << message << "\n";
    }
}

// Function to calculate the largest temperature difference within a 10-minute window
void calculateMaxDifference(const vector<TemperatureReading>& readings, const chrono::steady_clock::time_point& programStartTime) {
    double maxDiff = 0.0;
    chrono::steady_clock::time_point maxDiffStart;

    for (size_t i = 0; i < readings.size() - 1; ++i) {
        for (size_t j = i + 1; j < readings.size(); ++j) {
            auto timeDiff = chrono::duration_cast<chrono::seconds>(readings[j].timestamp - readings[i].timestamp).count();
            if (timeDiff > 10) break; // Checking for more than 10 seconds

            double diff = abs(readings[j].temp - readings[i].temp);
            if (diff > maxDiff) {
                maxDiff = diff;
                maxDiffStart = readings[i].timestamp;
            }
        }
    }

    if (maxDiffStart.time_since_epoch().count() != 0) {
        auto maxDiffStartSeconds = chrono::duration_cast<chrono::seconds>(maxDiffStart - programStartTime).count();
        safePrint("Largest Temperature Difference: " + to_string(maxDiff) + " F (starting at " + to_string(maxDiffStartSeconds) + " seconds)");
    } else {
        safePrint("Unable to calculate largest temperature difference.");
    }
}

// Generates an hourly report including the top 5 highest and lowest temperatures and the largest temperature difference
void hourlyReport(const chrono::steady_clock::time_point& startTime, vector<TemperatureReading> hourlyReadings) {
    // Sort hourlyReadings by temperature
    sort(hourlyReadings.begin(), hourlyReadings.end(), [](const TemperatureReading& a, const TemperatureReading& b) {
        return a.temp < b.temp;
    });

    safePrint("Hourly Report:");
    safePrint("Top 5 Highest Temperatures:");
    for (int i = max(0, static_cast<int>(hourlyReadings.size()) - 5); i < hourlyReadings.size(); ++i) {
        safePrint(to_string(hourlyReadings[i].temp) + " F");
    }

    safePrint("Top 5 Lowest Temperatures:");
    for (int i = 0; i < std::min(5, static_cast<int>(hourlyReadings.size())); ++i) {
        safePrint(to_string(hourlyReadings[i].temp) + " F");
    }

    calculateMaxDifference(hourlyReadings, startTime);
}

// Function for each sensor to collect temperatures
void temperatureSensor(int sensorId) {
    static const auto programStartTime = chrono::steady_clock::now();
    auto lastReportTime = chrono::steady_clock::now();

    while (true) {
        this_thread::sleep_for(chrono::seconds(1)); // Simulate 1-minute intervals

        auto now = chrono::steady_clock::now();
        int temp = getRandomTemp();

        {
            lock_guard<mutex> lock(readingsMtx);
            readings.emplace_back(temp, now);
        }

        // Generate report hourly (simulated as every minute here)
        if (sensorId == 0 && chrono::duration_cast<chrono::minutes>(now - lastReportTime) >= chrono::minutes(1)) {
            vector<TemperatureReading> tempReadings;
            {
                lock_guard<mutex> lock(readingsMtx);
                tempReadings.swap(readings); // Move readings to tempReadings for processing
            }
            hourlyReport(programStartTime, tempReadings);
            lastReportTime = now;
        }
    }
}

int main() {
    vector<thread> sensors;
    for (int i = 0; i < NUM_SENSORS; ++i) {
        sensors.emplace_back(temperatureSensor, i);
    }

    for (auto& sensor : sensors) {
        sensor.join();
    }

    return 0;
}
