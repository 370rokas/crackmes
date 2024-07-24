#include <condition_variable>
#include <syncstream>
#include <functional>
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                            });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
                });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    void stopAndJoin() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

    ~ThreadPool() {
        stopAndJoin();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

// Hash functions and helper functions

bool hashCheck1(uint64_t v) {
    uint64_t temp = (v ^ 1597463007 ^ (123456789 * v) | (v ^ 1597463007) & (123456789 * v)) % 97;
    return (3 * temp + temp % 7) % 5 != 2;
}

uint64_t generateFunnyNumber(uint64_t v1, uint64_t v2, uint64_t v3) {
    uint64_t theRatherFunnyNumber = 1;
    while (v2) {
        if ((v2 & 1) != 0)
            theRatherFunnyNumber = v1 * theRatherFunnyNumber % v3;
        v1 = v1 * v1 % v3;
        v2 >>= 1;
    }
    return theRatherFunnyNumber;
}

uint64_t enhanceNumbersSillyness(uint64_t v) {
    uint64_t temp = ((((v ^ 11936128518282651045uLL) >> 51) | ((v ^ 11936128518282651045uLL) << 13)) << 47) |
        ((((v ^ 11936128518282651045uLL) >> 51) | ((v ^ 11936128518282651045uLL) << 13)) >> 17);

    return 6364136223846793005uLL * (((temp ^ 6510615555426900570uLL) << 53) | ((temp ^ 6510615555426900570uLL) >> 11))
        % 18446744073709551615uLL;
}

uint64_t createTheUltimateSilly(uint64_t v) {
    uint64_t temp = (((0xC097EF87329E28A5LL * v % 18446744073709551615uLL) >> 53) | ((0xC097EF87329E28A5LL * v
        % 18446744073709551615uLL) << 11)) ^ 6510615555426900570uLL;

    return ((((temp >> 47) | (temp << 17)) << 51) | (((temp >> 47) | (temp << 17)) >> 13)) ^ 11936128518282651045uLL;
}

bool checkHashes(uint64_t usernameHash, uint64_t serialHash, const uint64_t& theFunny) {
    uint64_t hashSumMod = (usernameHash + serialHash) % 1000000007;

    if (hashCheck1(hashSumMod)) {
        return hashSumMod == theFunny;
    }

    return false;
}

uint64_t generateHash(char* source, int mult, uint64_t mod) {
    uint64_t resultingHash = 0;
    uint64_t temp = 1;
    char* currentChar;

    while (*source) {
        currentChar = source++;
        resultingHash = (temp * *currentChar % mod + resultingHash) % mod;
        temp = mult * temp % mod;
    }

    return resultingHash;
}

void processKeyChunk(int start, int end, const uint64_t& usernameHash, const uint64_t& sillyNumber, std::atomic<bool>& found, std::mutex& resultMutex) {
    std::vector<char> part1AllowedChars = { 'K', 'J', 'L' };
    std::vector<char> anyChar = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                                  's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
    std::vector<char> alphanumeric = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
                                 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                                 'u', 'v', 'w', 'x', 'y', 'z' };
    std::vector<char> digits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    std::vector<char> evenDigits = { '0', '2', '4', '6', '8' };
    std::vector<char> oddDigits = { '1', '3', '5', '7', '9' };
    std::vector<char> part5AllowedChars = { 'H', 'B', 'T', 'O', 'P' };

    char key[30] = "";

    for (int p1CurCharPos = 0; p1CurCharPos < 5; p1CurCharPos++) {
        for (int p1Numbers = start; p1Numbers < end; p1Numbers++) {
            if (found.load()) return;  // Check if we should exit early

            snprintf(key, 6, "%05d", p1Numbers);

            for (auto c : part1AllowedChars) {
                key[p1CurCharPos] = c;
                key[5] = '-';

                // PART 2
                for (auto p2c1 : anyChar) {
                    key[6] = p2c1;
                    for (auto p2c2 : anyChar) {
                        key[7] = p2c2;
                        for (auto p2c3 : anyChar) {
                            key[8] = p2c3;
                            for (auto p2c4 : anyChar) {
                                key[9] = p2c4;

                                for (auto p2ed : evenDigits) {
                                    key[10] = p2ed;
                                    key[11] = '-';

                                    // PART 3
                                    for (auto p3c1 : alphanumeric) {
                                        for (auto p3c2 : alphanumeric) {
                                            for (auto p3d1 : digits) {
                                                for (auto p3d2 : digits) {

                                                    if (abs(p3c1 - p3c2) == (p3d1 - '0' + p3d2 - '0')) {
                                                        key[12] = p3c1;
                                                        key[13] = p3c2;
                                                        key[14] = p3d1;
                                                        key[15] = p3d2;

                                                        for (auto p3f : anyChar) {
                                                            key[16] = p3f;
                                                            key[17] = '-';

                                                            // PART 4
                                                            for (auto p4c1 : anyChar) {
                                                                for (auto p4c2 : anyChar) {
                                                                    for (auto p4c3 : anyChar) {
                                                                        for (auto p4c4 : anyChar) {
                                                                            for (auto p4c5 : anyChar) {
                                                                                key[18] = p4c1;
                                                                                key[19] = p4c2;
                                                                                key[20] = p4c3;
                                                                                key[21] = p4c4;
                                                                                key[22] = p4c5;
                                                                                key[23] = '-';

                                                                                // PART 5
                                                                                for (auto p5c1 : part5AllowedChars) {
                                                                                    for (auto p5c2 : part5AllowedChars) {
                                                                                        for (auto p5c3 : part5AllowedChars) {
                                                                                            for (auto p5c4 : part5AllowedChars) {
                                                                                                for (auto p5c5 : part5AllowedChars) {
                                                                                                    if (found.load()) return;  // Check if we should exit early

                                                                                                    key[24] = p5c1;
                                                                                                    key[25] = p5c2;
                                                                                                    key[26] = p5c3;
                                                                                                    key[27] = p5c4;
                                                                                                    key[28] = p5c5;
                                                                                                    key[29] = '\0';

                                                                                                    uint64_t serialHash = generateHash(key, 31, 1000000007);
                                                                                                    if (checkHashes(usernameHash, serialHash, sillyNumber)) {
                                                                                                        if (!found.load()) {
                                                                                                            std::lock_guard<std::mutex> lock(resultMutex);
                                                                                                            if (!found.load()) { // Double-check if found was set in the meantime
                                                                                                                found.store(true);
                                                                                                                std::cout << "Found: " << key << std::endl;
                                                                                                                std::exit(0);
                                                                                                                return;
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

std::string bruteforce(const uint64_t& usernameHash, const uint64_t& sillyNumber) {
    const int numThreads = std::thread::hardware_concurrency();
    ThreadPool pool(numThreads);

    std::atomic<bool> found(false);
    std::mutex resultMutex;

    int numRanges = 5000;
    int rangeSize = numRanges / numThreads;

    // Launch threads
    for (int i = 0; i < numThreads; ++i) {
        int start = i * rangeSize;
        int end = (i == numThreads - 1) ? numRanges : start + rangeSize;
        pool.enqueue([start, end, &usernameHash, &sillyNumber, &found, &resultMutex] {
            processKeyChunk(start, end, usernameHash, sillyNumber, found, resultMutex);
            });
    }

    // Wait until a key is found
    while (!found.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Stop and join thread pool
    pool.stopAndJoin();

    return {};
}

int main() {
    char username[44] = "DZONERZY";

    // Compute the username hash
    const uint64_t usernameHash = generateHash(username, 31, 1000000007);

    // Generate a "magic" number used in checking if the username-serial key hash pair is good
    uint64_t aNumber = generateFunnyNumber(16370, 32771, 49157);
    uint64_t aRatherFunnyNumber = enhanceNumbersSillyness(aNumber);
    const uint64_t theSillyNumber = createTheUltimateSilly(aRatherFunnyNumber);

    // Brute force the key
    std::string serialKey = bruteforce(usernameHash, theSillyNumber);
    if (!serialKey.empty()) {
        std::cout << "Serial key found: " << serialKey << std::endl;
        return 0;
    }

    std::cout << "Serial key not found!" << std::endl;
    return 0;
}
