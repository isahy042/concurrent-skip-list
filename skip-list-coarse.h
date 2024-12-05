#pragma once
#include "skip-list.h"
#include "skip-list-sequential.h"

class CoarseSkipList : public SkipList
{
public:
    CoarseSkipList(int total_elements, int min_val);
    ~CoarseSkipList();

    bool contains(int val);
    void insert(int val);
    void remove(int val);

private:
    SequentialSkipList* skiplist;
    std::mutex mtx;
};