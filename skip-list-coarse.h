#pragma once
#include "skip-list.h"
#include "skip-list-sequential.h"
#include <mutex>

class CoarseSkipList : public SkipList
{
public:
    CoarseSkipList(int total_elements, int min_val);
    ~CoarseSkipList() = default;

    bool contains(int val);
    void insert(int val);
    void remove(int val);

private:
    std::shared_ptr<SequentialSkipList> skiplist;
    std::mutex mtx;
};