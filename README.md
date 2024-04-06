# COP4520-Assignment3
John Dufresne - COP4520 Assignment 3 - Due 4/5/2024

## How to run
- ### Problem 1
  ```command
    g++ problem1.cpp -o problem1 -std=c++20
    ./problem1
  ```

- ### Problem 2 - Hourly Report
  ```command
    g++ problem2.cpp -o problem2 -std=c++20
    ./problem2
  ```
  - ### Problem 2 - Minutely Report
  ```command
    g++ problem2shortened.cpp -o problem2shortened -std=c++20
    ./problem2
  ```

## Problem 1: The Birthday Presents Party:
- My solution to problem 1 entailed creating a struct called GiftNode which represents a Gift and holds the gifts id and a pointer to the next gift. I also created a add, delete and check methods for this linked list which used Mutexes and conditional variables to ensure mutual exclusion and limit race conditions. To simulate the unordered bag I created a Vector of size 500000, initialized it, then shuffled it to simulate it being unordered. I then created my 4 threads which called the servantTasks function. The servantTasks function essentially used a variable initialized at -1, and once it entered the while loop of the function - which checks if the number of gifts in the unordered bag is greater than 0, or if the number of gifts in the added bag is greater than 0 - it will increment and mod by 3 to ensure only 3 options can be chose. I then used mutexes surrounding these 3 situations to ensure vectors and variables are mutually excluded. After a bag is added to the list a counter is incremented to keep track of how many gifts were seen and added, as well as a counter for the number of Thank You cards writen to make sure the proper amount of Thank You cards were written. Thank You cards are only incremented if the gift was removed from the list successfully. All in all after multiple tests, all cards and gift counters outputted their correct values and total run time was around 8-10 seconds, keeping in mind each servant task was completed sequentially - task 1, task 2, task3 - to satisfy the requirement that they must alternate tasks. An output log of all the servant task can be found in the Problem1output.txt file.

## Problem 2: Atmospheric Temperature Reading Module:
-  My solution to Problem 2 entails creating 8 threads - 1 for each sensor - and given the requirement of only using 8 threads, once thread is in charge of both taking a temperature reading and making the hourly report. Each sensor is put to sleep until it is time to generate a reading, in which it will generate a reading and safe both the temperature recorded and timestamp of the reading to a vector, which is the shared memory space. The thread in charge of performing the report will keep track of how long it has been since the last report and when it realizes its time for a report, it will grab all the data from the shared memory location, and reset the memory location so old data is not used later on in a future report. All threads are protected through a mutex as well as putting them to sleep to make sure no threads interfere with eachother. The thread in charge of the report also uses a mutex to avoid interference with other threads during the report. Through rigoruous testing, I have ensured that each time a report is conducted, there are 480 entries within the shared memory space, ensuring all sensors are properly reading temperatures. I also use the built in sort function to find min and max temperatures as well as a specialized interval function to find the highest temperature difference in a 10 minute interval. All results are printed to the TemperatureReport.txt file for analysis. Keep in mind, this implementation converts the hourly report into a 1 minute report and all temperature readings are conducted every 1 second, so it is easy to revert back to an hourly report and temperature readings every 1 minute. All in all this function efficiently satisfies all requirements of the problem through the use of locks, sleep methods and efficient logic. 
