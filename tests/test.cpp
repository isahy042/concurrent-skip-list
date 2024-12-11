#include "../src/skip-list.h"
#include "../src/sequential/skip-list-sequential.h"
#include "../src/coarse/skip-list-coarse.h"
#include "../src/fine/skip-list-fine.h"
#include "../src/lock-free/skip-list-lock-free.h"
#include <cassert>
#include <set>


#define MAX_SIZE 10000

void test_correctness_synchronous(std::shared_ptr<SkipList> skip_list, int num_ops) {
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
}


void test_performance(std::shared_ptr<SkipList> skip_list, int num_threads) {

}

int main(int argc, char *argv[]) {
    auto skip_list = std::make_shared<SequentialSkipList>(MAX_SIZE);

    test_correctness_synchronous(skip_list, 10000);
}