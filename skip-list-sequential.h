#pragma once

#include "skip-list.h"

class SequentialSkipList : public SkipList
{
  public:
    Node* head;
    int max_levels;

    SequentialSkipList(int total_elements, int min_val);
    ~SequentialSkipList() = default;

    bool contains(int val);
    void insert(int val);
    void remove(int val);
};
