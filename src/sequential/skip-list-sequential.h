#pragma once

#include "../skip-list.h"

//./skip-list -f inputs/test.txt -m 1
class SequentialSkipList : public SkipList
{
public:
    SequentialSkipList(int total_elements);
    ~SequentialSkipList() = default;

    bool contains(int val);
    bool insert(int val);
    bool remove(int val);

    void validate();
private:
    std::shared_ptr<Node> head;
};
