// John Dufresne
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>
#include <algorithm>
#include <atomic>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;

const int NUM_SERVANTS = 4;
const int NUM_GIFTS = 500000;

// Node structure for the linked list
struct Gift {
    int tag;
    Gift* next;
    Gift(int t) : tag(t), next(nullptr) {}
};

// Global variables
mutex mtx;                  // Protects linked list and output
mutex Gift_mtx;             // Protects access to gifts vector
condition_variable cv;      // Condition Variable
Gift* head = nullptr;       // Head of linked list
Gift* tail = nullptr;       // Tail of linked list
vector<int> gifts;          // Bag of unordered gifts
vector<int> addedGifts;     // gifts added to linked list, so we can grab them and delete easily
atomic<int> giftsCount(0);  // Gift counter used to make sure 
atomic<int> num_gifts(0);   // Keeps track of number of gifts seen
atomic<int> num_cards(0);   // Keeps track of number of Thank You cards written
ofstream outFile("Problem1output.txt");    

// Safely output messages to the console
void safePrint(const string& message) {
    static mutex fileMutex; // Protects access to outFile
    lock_guard<mutex> lock(fileMutex);
    if(outFile.is_open()) {
        outFile << message << endl;
    }
}

// Function to add a Gift to the linked list
void addGift(int tag) {
    lock_guard<mutex> lock(mtx);
    Gift* newGift = new Gift(tag);
    bool wasEmpty = (head == nullptr);

    if (!head) {
        head = tail = newGift;
    } else {
        tail->next = newGift;
        tail = newGift;
    }

    cv.notify_one(); 
}

// Function to remove a Gift from the linked list
bool removeGift(int tag) {
    unique_lock<mutex> lock(mtx);
    //condition variable to wait to access the linked list
    cv.wait(lock, [&]() { return head != nullptr; });

    Gift* curr = head;
    Gift* prev = nullptr;

    while (curr && curr->tag != tag) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) {
        return false;
    }

    if (!prev) {
        head = curr->next;
    } else {
        prev->next = curr->next;
    }

    if (!curr->next) {
        tail = prev;
    }

    delete curr;
    return true;
}

// Function to check if a Gift is in the linked list
bool checkGift(int tag) {
    lock_guard<mutex> lock(mtx);

    Gift* curr = head;
    while (curr && curr->tag != tag) {
        curr = curr->next;
    }

    return curr != nullptr;
}

// Random number generator
int getRandomInt(int min, int max){
    random_device seed;
    mt19937 rng(seed());
    uniform_int_distribution<mt19937::result_type> dist(min, max);
    return dist(rng);
}

// Servant thread function
void servantTask(int id) {
    int action = -1;
    int tag;
    while (giftsCount > 0 || addedGifts.size() > 0) {
        action = (action+1) % 3;                // Making sure servants alternate through tasks
        {
            lock_guard<mutex> lock(Gift_mtx); // Protect both gifts and addedGifts with the same mutex
            if (action == 0 && !gifts.empty()) { // Add a Gift
                tag = gifts.back();             // Grab tag from end of undordered gifts bag
                gifts.pop_back();               // Remove that gift from the bag
                giftsCount--;                   
                addedGifts.push_back(tag);      // Add gift to addedGifts vector so servants know which gifts have been added
                addGift(tag);                       
                num_gifts++;                    // Increase our total of gifts to keep count
                safePrint("Servant " + to_string(id+1) + ": Added gift with tag " + to_string(tag));
            } 
            else if (action == 1 && !addedGifts.empty()) { // Remove a Gift
                tag = addedGifts.front();                   // Grab tag of the first gift in the list    
                if (removeGift(tag)) {
                    addedGifts.erase(addedGifts.begin());   // If its in the list write the Thank You card and remove it
                    num_cards++;                            // Increase our count of written Thank you cards
                    safePrint("Servant " + to_string(id+1) + ": Thank you card written for tag " + to_string(tag));
                } else {
                    safePrint("Servant " + to_string(id+1) + ": Gift with tag " + to_string(tag) + " not found");
                }
            } 
            else {
                tag = getRandomInt(1,NUM_GIFTS);            // Randomly generate tag for a servant to look for
                if (checkGift(tag)) {
                    safePrint("Servant " + to_string(id+1) + ": Gift with tag " + to_string(tag) + " is in the list");
                } else {
                    safePrint("Servant " + to_string(id+1) + ": Gift with tag " + to_string(tag) + " is not in the list");
                }
            }
        }
    }
}

int main() {
    // Initialize gifts vector with unique tags, and shuffle it so its unordered
    for (int i = 0; i < NUM_GIFTS; i++)
        gifts.push_back(i+1);
    shuffle(gifts.begin(), gifts.end(), default_random_engine(random_device{}()));
    giftsCount = NUM_GIFTS;

    cout << "Servants are starting their work..." << endl;
    auto start_timer = chrono::high_resolution_clock::now(); 

    // Create servant threads
    vector<thread> servants;
    for (int i = 0; i < NUM_SERVANTS; i++) {
        servants.emplace_back(servantTask, i);
    }

    // Join servant threads
    for (auto& servant : servants) {
        servant.join();
    }

    //End timer, find total, and convert to seconds
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start_timer;

    cout << "Number of Gifts: " << num_gifts << endl;
    cout << "Number of Cards: " << num_cards << endl;
    cout << "Runtime: " << duration.count() << " seconds." << endl;
    cout << "Check Problem1output.txt for servants task log" << endl;
    return 0;
}