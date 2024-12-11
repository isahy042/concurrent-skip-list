#include "../src/skip-list.h"
#include "../src/sequential/skip-list-sequential.h"
#include "../src/coarse/skip-list-coarse.h"
#include "../src/fine/skip-list-fine.h"
#include "../src/lock-free/skip-list-lock-free.h"
#include <cassert>
#include <set>
#include <thread>
#include <unistd.h>

#define MAX_SIZE 10000

void random_ops(std::shared_ptr<SkipList> skip_list, int num_ops) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> opDist(0, 1);
    std::uniform_int_distribution<> valueDist(0, MAX_SIZE / 2);

    for (int i = 0; i < num_ops; i++) {
        int op = opDist(gen);
        int value = valueDist(gen);
        bool res;

        switch (op) {
            case 0:
                res = skip_list->insert(value);
                break;
            case 1:
                res = skip_list->remove(value);
                break;
        }
    }
}

void test_correctness_synchronous(std::shared_ptr<SkipList> skip_list, int num_ops) {
    std::cout << "Running synchronous correctness test on skip list with " << num_ops << " operations..." << std::endl;
    // Randomly generate an operation and a number
    std::set<int> keys;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> opDist(0, 2);
    std::uniform_int_distribution<> valueDist(0, MAX_SIZE);

    for (int i = 0; i < num_ops; i++) {
        int op = opDist(gen);
        int value = valueDist(gen);
        bool res;

        switch (op) {
            case 0:
                res = skip_list->insert(value);
                if (res) keys.insert(value);
                break;
            case 1:
                res = skip_list->remove(value);
                if (res) keys.erase(value);
                break;
            case 2:
                ASSERT_MSG(skip_list->contains(value) == (keys.find(value) != keys.end()), "Skip list is not consistent with ground truth.");
                break;
        }
    }
    std::cout << "All operations complete, validating skip list..." << std::endl;
    skip_list->validate();
}

void test_correctness_concurrent(std::shared_ptr<SkipList> skip_list, int num_threads, int num_ops) {
    std::cout << "Running concurrent correctness test on skip list with " << num_threads << " threads and " << num_ops << " operations..." << std::endl;
    // Each thread will perform a set of random ops and then we will validate the skip list at the end
    std::vector<std::thread> threads;
    // We will partition the number of operations across all threads
    for (int i = 0; i < num_threads; i++) {
        int ops_per_thread = (num_ops + num_threads - 1) / num_threads;

        if (i == num_threads - 1) {
            ops_per_thread = num_ops - (num_threads - 1) * ops_per_thread;
        }

        threads.push_back(std::thread(random_ops, skip_list, ops_per_thread));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All operations complete, validating skip list..." << std::endl;
    skip_list->validate();
}


void test_performance(std::shared_ptr<SkipList> skip_list, int num_threads, int num_ops) {
    std::cout << "Running concurrent correctness test on skip list with " << num_threads << " threads and " << num_ops << " operations..." << std::endl;
    // Each thread will perform a set of random ops and then we will validate the skip list at the end
    std::vector<std::thread> threads;

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.push_back(std::thread(random_ops, skip_list, num_ops / num_threads));
    }

    for (auto& t : threads) {
        t.join();
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "All operations complete, validating skip list..." << std::endl;
    skip_list->validate();
    std::cout << "Elapsed time: " << elapsed.count() << "s\n";
}

int main(int argc, char *argv[]) {
    char testing_mode = '1';
    char skip_list_mode = '1';
    int num_threads = 1;
    int num_ops = 10000;

    int opt;
    while ((opt = getopt(argc, argv, "t:m:n:o:")) != -1) {
        switch (opt) {
            case 't':
                testing_mode = *optarg;
                break;
            case 'm':
                skip_list_mode = *optarg;
                break;
            case 'n':
                num_threads = atoi(optarg);
                break;
            case 'o':
                num_ops = atoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -t testing_mode -m skip_list_mode -o number_of_operations [-n num_threads]\n";
                exit(EXIT_FAILURE);
        }
    }

    std::shared_ptr<SkipList> skip_list;

    switch (skip_list_mode) {
        case '1':
            skip_list = std::make_shared<SequentialSkipList>(MAX_SIZE);
            break;
        case '2':
            skip_list = std::make_shared<CoarseSkipList>(MAX_SIZE);
            break;
        case '3':
            skip_list = std::make_shared<FineSkipList>(MAX_SIZE);
            break;
        case '4':
            skip_list = std::make_shared<LockFreeSkipList>(MAX_SIZE);
            break;
        default:
            std::cerr << "Invalid skip list mode. Please choose from 1, 2, 3, or 4.\n";
            exit(EXIT_FAILURE);
    }

    if (testing_mode == '1') {
        test_correctness_synchronous(skip_list, num_ops);
    } else if (testing_mode == '2') {
        test_correctness_concurrent(skip_list, num_threads, num_ops);
    } else if (testing_mode == '3') {
        test_performance(skip_list, num_threads, num_ops);
    } else {
        std::cerr << "Invalid testing mode. Please choose from 1, 2, or 3.\n";
        exit(EXIT_FAILURE);
    }
}