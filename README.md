# COP4520-Assignment3
John Dufresne - COP4520 Assignment 3 - Due 4/5/2024

## How to run
- ### Problem 1
  ```command
  
  ```

- ### Problem 2
  ```command
  
  ```

## Problem 1: The Birthday Presents Party:
- For my solution to problem 1 I had to take a couple important factors into account. The fact that guests can have multiple cakes, guests cannot communicate with eachother and guests are randomly chosen. I decided to create a vector holding the threads, and assigning the first thread as a counter, to keep track of how many times it replaces the cupcake. With this in mind, guests entering will only be allowed to have one cupcake each, and if they go through the maze and the cupcake isn't there, they cannot call for another one. This allows the counting thread to keep track of how many times it has to recall another cupcake, hence providing the counter with how many of the guests have had a cupcake. When the counter sees that they have counted as many cupcakes as there were total guests. They alert the minotaur that all the guests have been through the maze and have enjoyed the Minotaurs delicious cupcakes. To ensure mutual exclusion each thread has a lock and after the lock is initialized, they wait for the current guest being called to be same as the index of that thread or guest.

## Problem 2: Atmospheric Temperature Reading Module:
- For my solution to problem 2, I decided to choose choice 3 which was the queue. Some advantages of the queue would be the fact that guests can see how long it will take to view the vase through their position in line. Secondly, this avoids the trouble of line skipping ensuring everybody has an equal oportunity to see the vase. Some disadvtanges of this choice however could be the fact that all the guests do have to wait in-line instead of walking around doing other things while they wait. Another disadvantage is how it is a little more difficult to setup since we're dealing with locks and queues. However choice 3 was optimal in my opinion, and it was also a great learning experience dealing with queues, locks and threads. 
