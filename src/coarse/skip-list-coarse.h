#pragma once
#include "../skip-list.h"
#include "../sequential/skip-list-sequential.h"
#include <mutex>

class CoarseSkipList : public SkipList
{
public:
    CoarseSkipList(int total_elements);
    ~CoarseSkipList() = default;

    bool contains(int val);
    bool insert(int val);
    bool remove(int val);

    void validate();

private:
    std::shared_ptr<SequentialSkipList> skiplist;
    std::mutex mtx;
};